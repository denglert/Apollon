#include "AnalysisFW.h"

///////////////////////////////
// -- Category/Level/Mult -- //
///////////////////////////////

enum Category { jet, lepton, bjet  };
enum Level    { etmisscut, leptoncut, jetcut, zmasscut, hmasscut };
enum Mult     { mono,      di                };

////////////////////
//                //
// -- Analyzer -- //
//                //
////////////////////

// --- Prerequisites:
//   - needs ExRootTreeReader reader set up already.

void AnalysisFW::Analyzer( TChain *chain, ResultContainer<TH1D> *histo)
{
	std::cout << "AnalysisFW::Analyzer started." << std::endl;

	// -  Create object of class ExRootTreeReader
	reader = new ExRootTreeReader(chain);

	// - Get pointers to branches used in this analysis
	TClonesArray *branchParticle 				= reader->UseBranch("Particle");
	TClonesArray *branchElectron 				= reader->UseBranch("Electron");
	TClonesArray *branchPhoton 				= reader->UseBranch("Photon");
	TClonesArray *branchMuon   				= reader->UseBranch("Muon");
	TClonesArray *branchTrack 					= reader->UseBranch("Track");
	TClonesArray *branchTower 					= reader->UseBranch("Tower");
	TClonesArray *branchEFlowTrack         = reader->UseBranch("EFlowTrack");
	TClonesArray *branchEFlowTower         = reader->UseBranch("EFlowTower");
	TClonesArray *branchEFlowMuon          = reader->UseBranch("EFlowMuon");
	TClonesArray *branchEFlowNeutralHadron = reader->UseBranch("EFlowNeutralHadron");
	TClonesArray *branchJet  		         = reader->UseBranch("Jet");
	TClonesArray *branchMissingET		= reader->UseBranch("MissingET");
	// - Create physics objects
	Muon *muA 				 = NULL;
	Muon *muB 				 = NULL;
	Electron *elA				 = NULL;
	Electron *elB				 = NULL;
	GenParticle *muA_gen  = NULL;
	GenParticle *muB_gen  = NULL;
	GenParticle *elA_gen  = NULL;
	GenParticle *elB_gen  = NULL;
	Jet *jetA 				 = NULL;
	Jet *jetB 				 = NULL;
	Jet *jetCandidate			 = NULL;
	MissingET *missingET = NULL;
	GenParticle *particle = NULL;
	GenParticle *g0mother = NULL;
	GenParticle *g1mother = NULL;
	GenParticle *g2mother = NULL;
	GenParticle *g3mother = NULL;
	GenParticle *g4mother = NULL;
	Track *track 			 = NULL;
	Tower *tower 			 = NULL;
	TObject *object       = NULL;

	// - Lorentz Four-momenta
	TLorentzVector p_mumu; 
	TLorentzVector p_jj; 
	TLorentzVector p_mumujj; 
	TLorentzVector momentum;


	// - Scalar sum of Z+h constituent pTs
	double Ht;

	////////////////////////
	//                    //
	// --- Event loop --- //
	//                    //
	////////////////////////
	
	Long64_t nEntries = reader->GetEntries();
	if (nEvents == -1) {nEvents = nEntries;}
	std::cout << "nEvents: " << nEvents << std::endl;
	// - Loop over all events -- //
	for(Int_t iEv = 0; iEv < nEvents; ++iEv)
	{
	
		double percent = iEv/double(nEvents)*100.0;
  		progress_bar (percent);
	
		// - Load Event -- //
		reader->ReadEntry(iEv);
		
		////////////////////////////////
		//////    -- CutFlow --   ////// 
		////////////////////////////////

		int nJets = branchJet->GetEntries();
		int nMuons = branchMuon->GetEntries();
		int nElectrons = branchElectron->GetEntries();
		int zCandidate = 0;
		int nBJets = 0;
		missingET = (MissingET*) branchMissingET->At(0);		
		jetA = NULL;
		jetB = NULL;

		//Check exactly two leptons in event
		zCandidate = LeptonCount(nMuons, nElectrons);

		for(int iJet = 0; iJet < nJets; iJet++)

		{

			jetCandidate = (Jet*) branchJet->At(iJet);
			if(jetCandidate->BTag == 1)
			{
				nBJets++;
				if(nBJets == 1)
				{ jetA = jetCandidate; }

				if(nBJets == 2)
				{ jetB = jetCandidate; }

				if(nBJets == 3)
				{ break; }				
			}
		}


		if(nBJets == 2)
		{
			if(!CutJets(jetA,jetB))
			{
				continue;
			}
		}
		else
		{
			continue;
		}
		//Passed jetcuts
		histo->nEvents[jetcut]->Fill(1.);

		if (zCandidate == 1)
		{

			muA = (Muon*) branchMuon->At(0);
			muB = (Muon*) branchMuon->At(1);
	
			if(!CutMuons(muA, muB))
			{
				continue;
			}	
		
			TLorentzVector p_jj     = jetA->P4() + jetB->P4();
			TLorentzVector p_mumu   = muA->P4() + muB->P4();
			Ht = muA->PT + muB->PT + jetA->PT + jetB->PT;

		}

		if (zCandidate == 2)
		{
			elA = (Electron*) branchElectron->At(0);
			elB = (Electron*) branchElectron->At(1);
			if(!CutElectrons(elA,elB))
			{
				continue;
			}

			TLorentzVector p_jj     = jetA->P4() + jetB->P4();
			TLorentzVector p_mumu   = elA->P4() + elB->P4();
			Ht = elA->PT + elB->PT + jetA->PT + jetB->PT;

		}

		if (zCandidate == 3)
		{
			continue;
		}
		//Passed lepton cuts
		histo->nEvents[leptoncut]->Fill(1.);

		if (!CutMissingET(missingET, Ht))
		{
			continue;
		}

		//Passed missing et cut
		histo->nEvents[etmisscut]->Fill(1.);

		if(!CutZcandidate(p_mumu))
		{
			continue;
		}
		//Passed z mass cut
		histo->nEvents[zmasscut]->Fill(1.);
	
		if(!Cuthcandidate(p_jj))
		{
			continue;
		}
		//Passed h mass cut
		histo->nEvents[hmasscut]->Fill(1.);
		
		} // end-of-iEv-loop


//	   p_mumujj = p_mumu + p_jj;
	

	std::cout << std::endl;
	std::cout << "AnalysisFW::Analyzer finished." << std::endl;


	std::cout << "Number of events: " << nEvents << "\n";
	std::cout << "After jet cuts: " << histo->nEvents[jetcut]->GetEntries() << "\n";
	std::cout << "After lepton cuts: " << histo->nEvents[leptoncut]->GetEntries() << "\n";
	std::cout << "After missing Et cuts: " << histo->nEvents[etmisscut]->GetEntries() << "\n";
	std::cout << "After z mass cuts: " << histo->nEvents[zmasscut]->GetEntries() << "\n";
	std::cout << "After h mass cuts: " << histo->nEvents[hmasscut]->GetEntries() << "\n";
}
