#include "ResultContainer.h"


// -- Initialization -- //
template <typename T>
ResultContainer<T>::ResultContainer ( const char tag_[], const char prepath_[] )
{
	tag  = tag_;
	prepath = prepath_;

//	ResultContainer<T>::Allocate( bins, tag );
};


// -- Set Tag -- //
template <typename T>
void ResultContainer<T>::SetTag  ( const char tag_[]  )
{
	tag = tag_;
};


// -- Set pre-path -- //
template <typename T>
void ResultContainer<T>::SetPrePath ( const char prepath_[] )
{
	prepath = prepath_;
};


// -- Write to ROOT file -- //
template <typename T>
void ResultContainer<T>::WriteToROOTFile ( TFile *f )
{
	f->cd();

   std::set<TObject*>::iterator it;
 	for(it = fCollection.begin(); it != fCollection.end(); ++it)
	{
		(*it)->Write();
	}

};


// -- Allocate objects (TH1D, etc.) -- //
template <typename T>
void ResultContainer<T>::Allocate ( Binning *bins )
{

	TH1::SetDefaultSumw2( );

	// - Allocate multi-dim arrays for the histo->rams
	AllocateArrayXYZ( PtDistr,   bins->nCat,    bins->nMult, bins->nLevel);
	AllocateArrayXYZ( EtaDistr,  bins->nCat,    bins->nMult, bins->nLevel);
	AllocateArrayXY ( MinvDistr, bins->nCat,                 bins->nLevel);
	AllocateArrayXY ( mZhDistr,  bins->nCatmZh,              bins->nLevel);
	AllocateArrayXY ( backDistr,  bins->nCatmZh,              bins->nLevel);
	AllocateArrayX  ( nObj,      bins->nCat                              );
	AllocateArrayX  ( nEvents,   bins->nLevel);

	// Plot settings
	PlotSettings settings;
	settings.PrePath   = prepath;
	settings.Tag       = tag;

	// - Book histo->rams
	loopxyz( iCat, iMult, iLvl, bins->nCat, bins->nMult, bins->nLevel )
	{
		std::string histoname_pt  = tag+"_"+bins->tag_Mult(iMult)+bins->tag_Cat(iCat)+"_pt_" +bins->tag_Level(iLvl)+"-level";
		std::string histoname_eta = tag+"_"+bins->tag_Mult(iMult)+bins->tag_Cat(iCat)+"_eta_"+bins->tag_Level(iLvl)+"-level";

		std::string label_pt   = ";p_{T}("+bins->label_Mult(iMult)+bins->label_Cat(iCat)+") [GeV/c] "+bins->label_Level(iLvl)+"-level;dN/dp_{T}";
		std::string label_eta  = ";#eta("+ bins->label_Mult(iMult)+bins->label_Cat(iCat)+") "        +bins->label_Level(iLvl)+"-level/d#eta;";

		PtDistr [iCat][iMult][iLvl] = new T();
		EtaDistr[iCat][iMult][iLvl] = new T();

		PtDistr [iCat][iMult][iLvl]->SetNameTitle( histoname_pt.c_str() , label_pt.c_str()  );
		EtaDistr[iCat][iMult][iLvl]->SetNameTitle( histoname_eta.c_str(), label_eta.c_str() );

		fCollection.insert(PtDistr   [iCat][iMult][iLvl]);
		fCollection.insert(EtaDistr  [iCat][iMult][iLvl]);

		fPlotMap[ PtDistr [iCat][iMult][iLvl] ] = settings;
		fPlotMap[ EtaDistr[iCat][iMult][iLvl] ] = settings;
	}

	loopxy( iCat, iLvl, bins->nCat, bins->nLevel )
	{
		std::string histoname_minv = tag+"_"+"Minv_di-"+bins->tag_Cat(iCat)+"_"+bins->tag_Level(iLvl)+"-level";
		std::string label_minv 		= ";m_{inv}(di-"+bins->label_Cat(iCat)+") [GeV/c^{2}] "+bins->label_Level(iLvl)+"-level;dN/dm_{inv}";

		MinvDistr[iCat][iLvl] = new T();
		MinvDistr[iCat][iLvl]->SetNameTitle( histoname_minv.c_str(), label_minv.c_str() ) ;

		fCollection.insert( MinvDistr[iCat][iLvl] );
		fPlotMap[ MinvDistr[iCat][iLvl] ] = settings;
	}

	loopxy( iCat, iLvl, bins->nCatmZh, bins->nLevel )
	{
		std::string histoname_mZh = tag+"_"+"mZh_"+bins->tag_Level(iLvl)+"-level";
		std::string label_mZh 	  = ";m_{inv}(Zh) [GeV/c^{2}] "+bins->label_Level(iLvl)+"-level;dN/dm_{inv}";

		mZhDistr[iCat][iLvl] = new T();
		mZhDistr[iCat][iLvl]->SetNameTitle( histoname_mZh.c_str(), label_mZh.c_str() );

		fCollection.insert(mZhDistr[iCat][iLvl]);
		fPlotMap[ mZhDistr[iCat][iLvl] ] = settings;
	}

	// Histograms for background test/analysis cuts test
	loopxy( iCat, iLvl, bins->nCatmZh, bins->nLevel )

	{
		std::string histoname_jjmumu = tag+"_"+"back_jjmumu_"+bins->tag_Cat(iCat)+"_"+bins->tag_Level(iLvl)+"-level";
		std::string label_jjmumu 	  = ";m_{inv}(#mu#mujj) [GeV/c^{2}] "+bins->tag_Cat(iCat)+"_"+bins->label_Level(iLvl)+"-level;dN/dm_{inv}";

		backDistr[iCat][iLvl] = new T();
		backDistr[iCat][iLvl]->SetNameTitle( histoname_jjmumu.c_str(), label_jjmumu.c_str() );

		fCollection.insert(backDistr[iCat][iLvl]);
		fPlotMap[ backDistr[iCat][iLvl] ] = settings;
	}


	loopx( iCat, bins->nCat)
	{
		std::string histoname_nobj = tag+"_"+"n"+bins->tag_Cat(iCat);
		std::string label_nobj 		= ";N_{"+bins->label_Cat(iCat)+"}";

		nObj[iCat] = new T();
		nObj[iCat]->SetNameTitle( histoname_nobj.c_str(), label_nobj.c_str() );

		fCollection.insert( nObj[iCat]);
		fPlotMap[ nObj[iCat] ] = settings;
	}

	loopx( iLvl, bins->nLevel)
	{
		std::string histoname_nevents = tag+"_"+bins->tag_Level(iLvl);
		std::string label_nevents	= ";N_{"+bins->label_Level(iLvl)+"}";

		nEvents[iLvl] = new T();
		nEvents[iLvl]->SetNameTitle( histoname_nevents.c_str(), label_nevents.c_str() );

		fCollection.insert( nEvents[iLvl]);
		fPlotMap[ nEvents[iLvl] ] = settings;

	}

	std::string ZThetaDistr_name = tag+"_"+"ZThetaDistr";

	ZThetaDistr = new T();
	ZThetaDistr->SetNameTitle( ZThetaDistr_name.c_str(), ";#Theta [rad];dN/d#Theta");

	fCollection.insert( ZThetaDistr );
	fPlotMap[ ZThetaDistr ] = settings;

	std::string nJetConstituents_name = tag+"_"+"nJetConstituents";

	nJetConstituents = new T();
	nJetConstituents->SetNameTitle( nJetConstituents_name.c_str(), ";# of jet constitutents;" );

	fCollection.insert( nJetConstituents );
	fPlotMap[ nJetConstituents ] = settings;


}

// -- Set legend labels for all histograms -- //
template <typename T>
void ResultContainer<T>::SetLegendLabel ( const char label_[] )
{
	std::map<TObject*, PlotSettings>::iterator itPlotMap;

  	for(itPlotMap = fPlotMap.begin(); itPlotMap != fPlotMap.end(); ++itPlotMap)
  	{
  	  PlotSettings *settings = &itPlotMap->second;
	  settings->LegendLabel = label_;
	}
}

// -- Set text labels for all histograms -- //
template <typename T>
void ResultContainer<T>::SetTextLabel ( const char label_[], double PosX, double PosY, double TextSize  )
{
	std::map<TObject*, PlotSettings>::iterator itPlotMap;

  	for(itPlotMap = fPlotMap.begin(); itPlotMap != fPlotMap.end(); ++itPlotMap)
  	{
  	  PlotSettings *settings = &itPlotMap->second;
	  settings->TextLabel      = label_;
	  settings->TextLabelPosX = PosX;
	  settings->TextLabelPosY = PosY;
	  settings->TextLabelSize = TextSize;
	  settings->isTextLabel   = true;
	}
}


// -- Setup bins -- //
void TH1DContainer::SetupBins ( Binning *bins )
{

	loopxyz( iCat, iMult, iLvl, bins->nCat, bins->nMult, bins->nLevel )
	{
		PtDistr [iCat][iMult][iLvl]->SetBins( bins->nPtBins,  bins->PtMin,  bins->PtMax  );
		EtaDistr[iCat][iMult][iLvl]->SetBins( bins->nEtaBins, bins->EtaMin, bins->EtaMax );
	}

	loopxy( iCat, iLvl, bins->nCat, bins->nLevel )
	{
		MinvDistr[iCat][iLvl]->SetBins( bins->nMinvBins, bins->MinvMin, bins->MinvMax ) ;
	}


	loopxy( iCat, iLvl, bins->nCatmZh, bins->nLevel )
	{
		mZhDistr[iCat][iLvl]->SetBins( bins->nmZhBins, bins->mZhMin, bins->mZhMax);
	}


	loopxy( iCat, iLvl, bins->nCatmZh, bins->nLevel )
	{
		backDistr[iCat][iLvl]->SetBins( bins->nmZhBins, bins->mZhMin, bins->mZhMax);
	}

	loopx( iCat, bins->nCat)
	{
		nObj[iCat]->SetBins( bins->nObjBins, bins->ObjMin, bins->ObjMax );
	}

	loopx( iLvl, bins->nLevel)
	{
		nEvents[iLvl]->SetBins( bins->nEventsBins, bins->EventMin, bins->EventMax );
	}

	ZThetaDistr     ->SetBins( 100, -0.1, 3.15 );
	nJetConstituents->SetBins(  20,  0.0, 20.0 );
}


// -- ResultContainer class template specializations
template class ResultContainer<TH1D>;
template class ResultContainer<THStack>;
