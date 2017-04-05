#ifndef ANALYSISFW_H
#define ANALYSISFW_H

#include "classes/DelphesClasses.h"
#include "ExRootAnalysis/ExRootTreeReader.h"
#include <TClonesArray.h>
#include <TRandom3.h>
#include "UtilFunctions.h"
#include "ResultContainer.h"
#include "GraphTools.h"
#include <map>
#include <cmath>

// - AnalysisFW -- //
class AnalysisFW
{

	public:

	AnalysisFW( const char confFilePath_[] );
	AnalysisFW( );

	bool isSmear;

	void Analyzer  ( TChain *chain, ResultContainer<TH1D> *histo );

	void MakePlots( );
	void CreateOutputFile();
	void WriteOutput();

	void Init();

	int LeptonCount  ( int nMuons, int nElectrons);
	bool CutJet      ( Jet *jet     );
	bool CutJets	 ( Jet *jet1, Jet *jet2);
	bool CutMuon     ( Muon *mu     );
	bool CutMuons	 ( Muon *mu1, Muon *mu2);
	bool CutElectron ( Electron *el );
	bool CutElectrons (Electron *el1, Electron *el2);
	bool Cuthcandidate ( TLorentzVector const &el );
	bool CutZcandidate ( TLorentzVector const &el );
	bool CutBjet_parton ( GenParticle *p );
	bool CutMissingET (MissingET *missingET, double Ht);
	bool is_b_from_higgs(GenParticle *p, TClonesArray *branchParticle );
	int nEvents;

	// - input/output handling
	TFile *output;
	TChain *chain;
	ExRootTreeReader *reader;

//	std::map<TObject*, PlotSettings> fPlotMap;

	Binning bins;
	Cuts cuts;
	Components components;

	std::string workdir;

	TH1DContainer *histos;

	THStackContainer hstacks;
//	Histograms *histos;

	std::string  binConfigFilePath;
	std::string compConfigFilePath;

	config conf;

};

#endif
