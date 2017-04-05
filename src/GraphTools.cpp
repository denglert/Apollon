#include "GraphTools.h"
#include <iostream>

TStyle* TStyle_Scheme ()
{

	TStyle* myStyle = new TStyle("myStyle","My own Root Style");
//	myStyle->SetTitleSize(0.3); 
//	myStyle->SetTitleX(0.3); 
	myStyle->SetTitleXOffset(1.5); 
	myStyle->SetTitleYOffset(1.5); 
	myStyle->SetTitleSize(0.04,"xy"); 
	myStyle->SetOptStat(0);
//	myStyle->SetLabelSize(0.06,"xy"); 
//

	
	return myStyle;

}


/////////////////////////
// --- CreatePlot --- ///
/////////////////////////

template<class T>
void CreatePlot ( T *obj, PlotSettings const& settings)
{
	TCanvas canvas("canvas", "canvas", 600, 600);
	gPad->SetLeftMargin( settings.LeftMargin  );
	gPad->SetBottomMargin( settings.BottomMargin );

	obj->Draw( settings.DrawingOption.c_str() );

	std::string prepath = settings.PrePath;
	std::string outfig  = prepath + obj->GetName();
	outfig = outfig + ".pdf";

	TLegend *leg;

	if ( settings.isTextLabel )
	{
		double PosX = settings.TextLabelPosX;
	   double PosY = settings.TextLabelPosY;
	   double Size = settings.TextLabelSize;
	   std::string TextLabel = settings.TextLabel.c_str();
   	TText *t = new TText( PosX, PosY, TextLabel.c_str() );
		t->SetNDC(true);
   	t->SetTextSize(Size);
   	t->Draw("SAME");
   }

	if ( !settings.fPlotMap.empty() )
	{

	leg = new TLegend( settings.Legend_Pos_X, settings.Legend_Pos_Y, (settings.Legend_Pos_X+settings.Legend_Wid_X), (settings.Legend_Pos_Y + settings.Legend_Wid_Y) );

	std::map<TObject*, PlotSettings>::const_iterator itPlotMap;

  	for(itPlotMap = settings.fPlotMap.begin(); itPlotMap != settings.fPlotMap.end(); ++itPlotMap)
  	{
  	  TH1D* histo = (TH1D*) itPlotMap->first;
  	  const PlotSettings *sub_settings = &itPlotMap->second;
	  leg->SetTextSize( settings.Legend_Text_Size );
	  leg->AddEntry (histo, sub_settings->LegendLabel.c_str());
	}



	leg->Draw("SAME");

	}

	canvas.SaveAs(outfig.c_str());
}


template void CreatePlot ( TH1D *histo,    PlotSettings const& settings) ;
template void CreatePlot ( THStack *histo, PlotSettings const& settings) ;


//////////////////////////////
//void CreateStackPlot (THStack *hstack, PlotSettings const& settings )
//{
//	gStyle->SetOptStat(0);
//	TCanvas canvas("canvas", "canvas", 600, 600)
//	gPad->SetLeftMargin( settings.LeftMargin  );
//	gPad->SetBottomMargin( settings.BottomMargin );
//	//hstack->Draw("");
//	hstack->Draw("E");
//	std::string prepath = settings.PrePath;
//	std::string outfig = prepath+hstack->GetName();
//	outfig = outfig+".pdf";
//	canvas.SaveAs(outfig.c_str());
//}
