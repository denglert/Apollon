#include "AnalysisFW.h"

//////////////////////
// -- AnalysisFW -- //
//////////////////////

// -- AnalysisFW::Init() -- //
void AnalysisFW::Init()
{

	TH1::SetDefaultSumw2( );

	conf.append(  binConfigFilePath.c_str() ); // - binning configuration 
	conf.append( compConfigFilePath.c_str() ); // - components configuration

	bins.LoadBinningConfig( &conf );
   LoadCutsConfig( &conf, &cuts);

	if( "yes" ==  (std::string) getconfig(conf, "isSmear") )
	{ isSmear = true; std::cout << "Smearing of the spectra activated." << std::endl; }
	else
	{ isSmear = false; }

	components.nComp = (int)    getconfig(conf, "nComp"); // - number of components
	components.lumi  = (double) getconfig(conf,  "lumi"); // - integrated luminosity

	//tag  = (std::string) getconfig(conf,  "tag");
	std::string prepath = workdir;

	histos  = new TH1DContainer[components.nComp];
	hstacks = THStackContainer();

	hstacks.SetTag    ( "stacks" );
	hstacks.SetPrePath( prepath.c_str() );

	hstacks.Allocate( &bins );

	printf("Total integrated luminosity:  %12.6f (fb-1)\n", components.lumi);
	printf("Component name | cross section (fb) | nPhysEvents | nGenEvents | scale\n");

	std::string GlobalTextLabel = (std::string) getconfig(conf, "GlobalTextLabel");
	double GlobalTextPosX       = (double) getconfig(conf, "GlobalTextPosX");
	double GlobalTextPosY       = (double) getconfig(conf, "GlobalTextPosY");
	double GlobalTextSize       = (double) getconfig(conf, "GlobalTextSize");

	// - Extracting component config
	for( int i=0; i < components.nComp; i++ )
	{
		std::string compname  = Form("compname%d",i);
		std::string comppath  = Form("comppath%d",i);
		std::string compxsec  = Form("compxsec%d",i);
		std::string compnev   = Form("compnev%d",i);
		std::string compcolor = Form("compcolor%d",i);

		components.component_name[i]    = (std::string) getconfig(conf, compname.c_str() );
		components.component_path[i]    = (std::string) getconfig(conf, comppath.c_str() );
		components.component_xsec[i]    = (double)      getconfig(conf, compxsec.c_str() );
		components.component_nEvents[i] = (double)      getconfig(conf, compnev.c_str()  );
		components.component_scale[i]   = components.lumi * components.component_xsec[i] / components.component_nEvents[i];
		components.component_color[i]   = (int) getconfig(conf, compcolor.c_str() );

		histos[i].SetTag     ( components.component_name[i].c_str() );
		histos[i].SetPrePath ( prepath.c_str() );
		histos[i].Allocate(  &bins );
		histos[i].SetupBins( &bins ); 

		// Legend labels
		histos[i].SetLegendLabel( Form("#sigma_{tot} (%s) = %.2f fb",components.component_name[i].c_str(), components.component_xsec[i] ) );

		if( !GlobalTextLabel.empty() )
      histos[i].SetTextLabel( GlobalTextLabel.c_str(), GlobalTextPosX, GlobalTextPosY, GlobalTextSize );

		printf("%14s %20.6f %12.2f %12.2f %12.8f\n", components.component_name[i].c_str(), components.component_xsec[i], components.lumi*components.component_xsec[i], components.component_nEvents[i], components.component_scale[i]);
	}

	
	std::cout << "nComp: " << components.nComp << std::endl;

	std::cout << "nCat: "    << bins.nCat << std::endl;
	std::cout << "nLevel: "  << bins.nLevel << std::endl;
	std::cout << "nMult: "   << bins.nMult << std::endl;

	std::cout << "nCatmZh: " << bins.nCatmZh << std::endl;

	std::cout << "nPtBins: " << bins.nPtBins << std::endl;
	std::cout << "PtMin: "   << bins.PtMin   << std::endl;
	std::cout << "PtMax: "   << bins.PtMax   << std::endl;

};



// -- AnalysisFW constructor -- //
AnalysisFW::AnalysisFW( )
{
  	chain = new TChain("Delphes");
}


// -- AnalysisFW::CreateOutputFile() -- //
void AnalysisFW::CreateOutputFile( )
{
	std::string outputfile = workdir+"/ana.root";
	output = new TFile(outputfile.c_str(),"RECREATE");
}


// -- AnalysisFW::WriteOutput() -- //
void AnalysisFW::WriteOutput( )
{

 	for(int i = 0; i < components.nComp; i++)
	{
		histos[i].WriteToROOTFile( output );
	}

//  for(itHisto = fCollection.begin(); itHisto != fCollection.end(); ++itHisto)
//  {
//	 (*itHisto)->Write();
//  }
}


// -- AnalysisFW::MakePlots() -- //
void AnalysisFW::MakePlots( )
{

  std::string prepath = workdir;


//  Warning: Need to fix this !!!

    std::set<TObject*>::iterator itHisto;
    for(int iComp = 0; iComp < components.nComp; iComp++)
	 {

    	for(itHisto = histos[iComp].fCollection.begin(); itHisto != histos[iComp].fCollection.end(); ++itHisto)
    	{
	  	 	std::string name        = (*itHisto)->GetName();
	  	 	std::string figfullpath = prepath+name;
			TH1D* histo = (TH1D*) (*itHisto);
		  	histo->Scale( components.component_scale[iComp], "width" );
			if ( histo->GetEntries() == 0) { printf("%s seems to be empty. No plot created.\n", histo->GetName()); continue;}
	  	 	CreatePlot( histo, histos[iComp].fPlotMap[histo] );
    	}

	 }

	 // -- Get an array of size 'nComp' consisting of iterators
    std::set<TObject*>::iterator itHistos[components.nComp];
    for(int iComp = 0; iComp < components.nComp; iComp++)
	 {
		itHistos[iComp] = histos[iComp].fCollection.begin();
	 }

	 // -- Loop over objects in hstacks
    std::set<TObject*>::iterator itStack;
    for(itStack = hstacks.fCollection.begin(); itStack != hstacks.fCollection.end(); itStack++)
	 {

		THStack* hstack = (THStack*) (*itStack);

		 bool empty = false;
	    for(int iComp = 0; iComp < components.nComp; iComp++)
		 {
			TH1D* histo = (TH1D*) (*itHistos[iComp]);
//			printf("histo->GetName(): %s\n ", histo->GetName());
			if ( histo->GetEntries() == 0) { empty=true; }
			histo->SetFillColor  ( Colors[iComp] );
			histo->SetMarkerColor( Colors[iComp] );
			histo->SetMarkerStyle( 21 );
			histo->SetStats(false);
			hstack->Add ( histo, "hist" );
			
			// Get THStack hstacks settings
			PlotSettings *settings = &hstacks.fPlotMap[hstack];
			settings->fPlotMap[ histo ] = histos[iComp].fPlotMap[ histo ] ;

			itHistos[iComp]++;
		 }

		 if ( !empty)
  	 	 CreatePlot( hstack, hstacks.fPlotMap[hstack] );

	 }



//  	PlotSettings settings;
//  	TObject *object;
//  	TH1 *histogram;
//  	THStack *stack;
//  	TPaveStats *stats;
//  	TString name;

//  	TCanvas *canvas = GetCanvas();

//  	map<TObject*, PlotSettings>::iterator itPlotMap;
//
//  	for(itPlotMap = fPlotMap.begin(); itPlotMap != fPlotMap.end(); ++itPlotMap)
//  	{
//  	  object = itPlotMap->first;
//  	  settings = itPlotMap->second;
//  	  name = object->GetName();
//  	  histogram = 0;
//  	  stack = 0;
//
//
//  	  if(object->IsA()->InheritsFrom(TH1::Class()))
//  	  {
//  	    histogram = static_cast<TH1*>(object);
//  	  }
//  	  else if(object->IsA()->InheritsFrom(THStack::Class()))
//  	  {
//  	    stack = static_cast<THStack*>(object);
//  	    histogram = static_cast<TH1*>(stack->GetHists()->First());
//  	  }
//
//  	  canvas->SetLogx(settings.logx);
//  	  if(histogram == 0 || histogram->Integral() > 0.0)
//  	  {
//  	    canvas->SetLogy(settings.logy);
//  	  }
//  	  else
//  	  {
//  	    canvas->SetLogy(0);
//  	  }
//
//  	  if(object->IsA()->InheritsFrom(THStack::Class()))
//  	  {
//  	    object->Draw("nostack");
//  	    stack->GetXaxis()->SetTitle(histogram->GetXaxis()->GetTitle());
//  	    stack->GetYaxis()->SetTitle(histogram->GetYaxis()->GetTitle());
//  	    stack->GetXaxis()->SetTitleOffset(1.5);
//  	    stack->GetYaxis()->SetTitleOffset(1.75);
//  	  }
//  	  else
//  	  {
//  	    object->Draw();
//  	  }
//
//  	  canvas->Update();
//
//  	  if(histogram)
//  	  {
//  	    stats = static_cast<TPaveStats*>(histogram->FindObject("stats"));
//  	    if(stats)
//  	    {
//  	      stats->SetX1NDC(0.67);
//  	      stats->SetX2NDC(0.99);
//  	      stats->SetY1NDC(0.77);
//  	      stats->SetY2NDC(0.99);
//  	      stats->SetTextFont(kExRootFont);
//  	      stats->SetTextSize(kExRootFontSize);
//  	      canvas->Draw();
//  	    }
//  	  }
//
//  	  if(settings.attachments)
//  	  {
//  	    TIter iterator(settings.attachments);
//  	    while((object = iterator()))
//  	    {
//  	      object->Draw();
//  	    }
//  	  }
//
//  	  canvas->Print(name + "." + format);
//  	}


}
