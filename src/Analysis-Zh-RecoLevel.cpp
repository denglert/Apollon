#include "AnalysisFW.h"

///////////////////////////////
// -- Category/Level/Mult -- //
///////////////////////////////

enum Category { jet,     bjet,   mu          };
enum Level    { gene, genecut, reco, recocut };
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

	TLorentzVector p_jetA_Const;
	TLorentzVector p_jetB_Const;

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
		
		////////////////
		// -- Jets -- //
		////////////////

		int nJets = branchJet->GetEntries();
		int nBJets = 0;
		histo->nObj[jet]->Fill(nJets);
		// -- First jet iterator -- //
		for(int iJet = 0; iJet < nJets; iJet++)
		{

			p_jetA_Const = TLorentzVector(0.0,0.0,0.0,0.0);

			// Get Jet A
	   	jetA = (Jet*) branchJet->At(iJet);

			// Get number of constituents of the jets
			int nJetAConstituents = jetA->Constituents.GetEntriesFast();
			histo->nJetConstituents->Fill( nJetAConstituents );
		   // - Loop over all of the jet constituents
		   for(int iJetConst = 0; iJetConst < nJetAConstituents; iJetConst++)
		   {
		
				object       = NULL;
		      object = jetA->Constituents.At(iJetConst);
		
				// Check if the constituent is accessible
				if(object == 0) continue;
				if(object->IsA() == GenParticle::Class())
				{
				  particle = (GenParticle*) object;
				  //	  std::cout << "    GenPart pt: " << particle->PT << ", eta: " << particle->Eta << ", phi: " << particle->Phi << std::endl;
				  p_jetA_Const += particle->P4();
				}
				else if(object->IsA() == Track::Class())
				{
				  track = (Track*) object;
				  //	  std::cout << "    Track pt: " << track->PT << ", eta: " << track->Eta << ", phi: " << track->Phi << std::endl;
				  p_jetA_Const += track->P4();
				
				  //  std::cout << "track " << std::endl;
				}
				else if(object->IsA() == Tower::Class())
				{
				  tower = (Tower*) object;
				  //	  std::cout << "    Tower pt: " << tower->ET << ", eta: " << tower->Eta << ", phi: " << tower->Phi << std::endl;
				  p_jetA_Const += tower->P4();
				}
		   }

			// - Single jet distributions
			// without cuts
			// Fill reco level jets
			histo->PtDistr [jet][mono][reco]->Fill( jetA->PT );
			histo->EtaDistr[jet][mono][reco]->Fill( jetA->Eta );


			// Fill gene level jets
			histo->PtDistr [jet][mono][gene]->Fill( p_jetA_Const.Pt()  );
			histo->EtaDistr[jet][mono][gene]->Fill( p_jetA_Const.Eta() );


			// - Single jet distributions
			// with cuts
			if( CutJet(jetA) == true )
			{
				histo->PtDistr [jet][mono][recocut]->Fill( jetA->PT );
				histo->EtaDistr[jet][mono][recocut]->Fill( jetA->Eta );

				histo->PtDistr [jet][mono][genecut]->Fill( p_jetA_Const.Pt()  );
				histo->EtaDistr[jet][mono][genecut]->Fill( p_jetA_Const.Eta() );
			}

			// - Single bjet distributions
			if( jetA->BTag == true )
			{

				nBJets++;
				// without cuts
				histo->PtDistr [bjet][mono][reco]->Fill( jetA->PT );
				histo->EtaDistr[bjet][mono][reco]->Fill( jetA->Eta );

				histo->PtDistr [bjet][mono][gene]->Fill( p_jetA_Const.Pt()  );
				histo->EtaDistr[bjet][mono][gene]->Fill( p_jetA_Const.Eta() );

				// with cuts
				if( CutJet(jetA) == true )
				{
					histo->PtDistr [bjet][mono][recocut]->Fill( jetA->PT );
					histo->EtaDistr[bjet][mono][recocut]->Fill( jetA->Eta );
					histo->PtDistr [bjet][mono][genecut]->Fill( p_jetA_Const.Pt()  );
					histo->EtaDistr[bjet][mono][genecut]->Fill( p_jetA_Const.Eta() );
				}

			}


			// -- Second jet iterator -- //
			for(int jJet = iJet+1; jJet < nJets; jJet++)
			{


				// Get Jet B
	   		jetB = (Jet*) branchJet->At(jJet);
				int nJetBConstituents = jetB->Constituents.GetEntriesFast();

				p_jetB_Const = TLorentzVector(0.0,0.0,0.0,0.0);

				// - Loop over all of the jet constituents
		   	for(int jJetConst = 0; jJetConst < nJetBConstituents; jJetConst++)
		   	{
		
					object = NULL;
		   	   object = jetB->Constituents.At(jJetConst);
		
					// Check if the constituent is accessible
					if(object == 0) continue;
					
					
					if(object->IsA() == GenParticle::Class())
					{
					  particle = (GenParticle*) object;
					  //	  std::cout << "    GenPart pt: " << particle->PT << ", eta: " << particle->Eta << ", phi: " << particle->Phi << std::endl;
					  p_jetB_Const += particle->P4();
					}
					else if(object->IsA() == Track::Class())
					{
					  track = (Track*) object;
					  //	  std::cout << "    Track pt: " << track->PT << ", eta: " << track->Eta << ", phi: " << track->Phi << std::endl;
					  p_jetB_Const += track->P4();
					
				//	  std::cout << "track " << std::endl;
					}
					else if(object->IsA() == Tower::Class())
					{
					  tower = (Tower*) object;
					  //	  std::cout << "    Tower pt: " << tower->ET << ", eta: " << tower->Eta << ", phi: " << tower->Phi << std::endl;
					  p_jetB_Const += tower->P4();

					}
				}

				// Sum of jetA and JetB
				TLorentzVector p_jj      = jetA->P4()   + jetB->P4();
				TLorentzVector p_jj_gene = p_jetA_Const + p_jetB_Const;
				// without cuts
				histo->MinvDistr[jet]    [reco]->Fill( p_jj.M()        );
				histo->EtaDistr [jet][di][reco]->Fill( p_jj.Eta()      );
				histo->PtDistr  [jet][di][reco]->Fill( p_jj.Pt()       );
				histo->MinvDistr[jet]    [gene]->Fill( p_jj_gene.M()   );
				histo->PtDistr  [jet][di][gene]->Fill( p_jj_gene.Pt()  );
				histo->EtaDistr [jet][di][gene]->Fill( p_jj_gene.Eta() );
				// with cuts
				// Cuts on jetA & jetB are satisfied & also on h candidate
				if( (CutJet(jetA) == true) && (CutJet(jetB) == true) && ( Cuthcandidate( p_jj ) ) )
				{
					histo->MinvDistr[jet]    [recocut]->Fill( p_jj.M()  );
					histo->MinvDistr[jet]    [genecut]->Fill( p_jj_gene.M() );
					histo->PtDistr  [jet][di][recocut]->Fill( p_jj.Pt() );
					histo->EtaDistr [jet][di][recocut]->Fill( p_jj.Eta() );
					histo->PtDistr  [jet][di][genecut]->Fill( p_jj_gene.Pt() );
					histo->EtaDistr [jet][di][genecut]->Fill( p_jj_gene.Eta() );
				}


				// without cuts but with bjets
				if( (jetA->BTag == true) && (jetB->BTag == true) )
				{
					histo->MinvDistr[bjet]    [reco]->Fill( p_jj.M()        );
					histo->MinvDistr[bjet]    [gene]->Fill( p_jj_gene.M()   );
					histo->PtDistr  [bjet][di][reco]->Fill( p_jj.Pt()       );
					histo->EtaDistr [bjet][di][reco]->Fill( p_jj.Eta()      );
					histo->PtDistr  [bjet][di][gene]->Fill( p_jj_gene.Pt()  );
					histo->EtaDistr [bjet][di][gene]->Fill( p_jj_gene.Eta() );

					if( (CutJet(jetA) == true) && (CutJet(jetB) == true) && ( Cuthcandidate( p_jj ) ))
					{
						histo->MinvDistr[bjet]    [recocut]->Fill( p_jj.M()        );
						histo->MinvDistr[bjet]    [genecut]->Fill( p_jj_gene.M()   );
						histo->PtDistr  [bjet][di][recocut]->Fill( p_jj.Pt()       );
						histo->EtaDistr [bjet][di][recocut]->Fill( p_jj.Eta()      );
						histo->PtDistr  [bjet][di][genecut]->Fill( p_jj_gene.Pt()  );
						histo->EtaDistr [bjet][di][genecut]->Fill( p_jj_gene.Eta() );
					}
				}


			}

		}

		histo->nObj[bjet]->Fill(nBJets);

		
		///////////////////////////
		// -- Particle branch -- //
		///////////////////////////


		TLorentzVector p_Z;
		TLorentzVector p_h;

		//std::cout << "PID Status MotherPID MotherStatus GrMotherPID GrMotherStatus Pt" << std::endl;
		int nParticles =branchParticle->GetEntries();
		for (int iPart = 0; iPart < nParticles; iPart++)
		{

		  particle = (GenParticle *) branchParticle->At(iPart);
//		  int g0 = particle->M1;
//
//		  printf("%5d %1d ", particle->PID, particle->Status);
//		  if ( g0 == -1 )
//		  { 
//		  printf("%5d %1d ", 0, 0);
//		  }
//		  else
//		  {
//		  	g0mother = (GenParticle *) branchParticle->At(g0);
//
//		   printf("%5d %1d ", g0mother->PID, g0mother->Status);
//		   int g1 = g0mother->M1;
//		  	if ( g1 == -1 )
//			{
//		  		printf("%5d %1d ", 0, 0);
//			}
//			else
//			{
//				g1mother = (GenParticle *) branchParticle->At(g1);
//				printf("%5d %1d ", g1mother->PID, g1mother->Status);
//			}
//
//		  }
//
//		  printf("%5.2f \n", particle->PT);
			
			if ( (particle->PID == 23) && (particle->Status == 3) )
			{ p_Z = particle->P4(); }

			if ( (particle->PID == 25) && (particle->Status == 3) )
			{ p_h = particle->P4(); }

		}

		TLorentzVector p_Zh = p_Z + p_h;
		TVector3 p_Zh_BoostVector = p_Zh.BoostVector();
		p_Z.Boost( -p_Zh_BoostVector );
		p_h.Boost( -p_Zh_BoostVector );
		
//		printf("%5.2f %5.2f %5.2f %5.2f\n", p_Zh.Px(), p_Zh.Py(), p_Zh.Pz(), p_Zh.E() );
//		printf("%5.2f %5.2f %5.2f %5.2f\n", p_Z.Px(), p_Z.Py(), p_Z.Pz(), p_Z.E() );
//		printf("%5.2f %5.2f %5.2f %5.2f\n", p_h.Px(), p_h.Py(), p_h.Pz(), p_h.E() );
      
//    Q: Why is theta Theta_{Z} = 0 ???
//		if(  p_Z.Theta() == 0.0 )
//		{
//			// Debuggg 
//			std::cerr << "cica " << std::endl;
//		}
		histo->ZThetaDistr->Fill( p_Z.Theta() );


		/////////////////
		// -- Muons -- //
		/////////////////

		int nMuons = branchMuon->GetEntries();
		histo->nObj[mu]->Fill(nMuons);

		// - If event contains at least 2 electrons
		
		for(int iMuon = 0; iMuon < nMuons; iMuon++)
		{

	   	muA     = (Muon*) branchMuon->At(iMuon);
			muA_gen = NULL;
		   muA_gen = (GenParticle*) muA->Particle.GetObject();


			// - Single mu distributions
			// without cuts
			histo->PtDistr [mu][mono][reco]->Fill( muA->PT );
			histo->EtaDistr[mu][mono][reco]->Fill( muA->Eta );
			histo->PtDistr [mu][mono][gene]->Fill( muA_gen->PT );
			histo->EtaDistr[mu][mono][gene]->Fill( muA_gen->Eta );

			// with cuts
			if( CutMuon(muA) == true )
			{
				histo->PtDistr [mu][mono][recocut]->Fill( muA->PT );
				histo->EtaDistr[mu][mono][recocut]->Fill( muA->Eta );
				histo->PtDistr [mu][mono][genecut]->Fill( muA_gen->PT );
				histo->EtaDistr[mu][mono][genecut]->Fill( muA_gen->Eta );
			}

			// - Second muon loop
			for(int jMuon = iMuon+1; jMuon < nMuons; jMuon++)
			{
				
	   		muB     = (Muon*) branchMuon->At(jMuon);
		   	muB_gen = (GenParticle*) muB->Particle.GetObject();

				TLorentzVector p_mumu = muA->P4() + muB->P4();
				TLorentzVector p_mumu_gen = muA_gen->P4() + muB_gen->P4();
				
				// without cuts
				histo->MinvDistr[mu]    [reco]->Fill( p_mumu.M()   );
				histo->PtDistr  [mu][di][reco]->Fill( p_mumu.Pt()  );
				histo->EtaDistr [mu][di][reco]->Fill( p_mumu.Eta() );

				histo->MinvDistr[mu]    [gene]->Fill( p_mumu_gen.M()   );
				histo->PtDistr  [mu][di][gene]->Fill( p_mumu_gen.Pt()  );
				histo->EtaDistr [mu][di][gene]->Fill( p_mumu_gen.Eta() );

				// with cuts
				if( (CutMuon(muA) == true) && (CutMuon(muB) == true) && (CutZcandidate(p_mumu) == true) )
				{
					histo->MinvDistr[mu]    [recocut]->Fill( p_mumu.M()   );
					histo->PtDistr  [mu][di][recocut]->Fill( p_mumu.Pt()  );
					histo->EtaDistr [mu][di][recocut]->Fill( p_mumu.Eta() );

					histo->MinvDistr[mu]    [genecut]->Fill( p_mumu_gen.M()   );
					histo->PtDistr  [mu][di][genecut]->Fill( p_mumu_gen.Pt()  );
					histo->EtaDistr [mu][di][genecut]->Fill( p_mumu_gen.Eta() );
				}



			}

		}


		//////////////////
		// -- jjmumu -- // 
		//////////////////

		for(int iJet = 0; iJet < nJets; iJet++)
		for(int jJet = iJet+1; jJet < nJets; jJet++)
		for(int iMuon = 0; iMuon < nMuons; iMuon++)
		for(int jMuon = iMuon+1; jMuon < nMuons; jMuon++)
		{
					
	   	jetA = (Jet*) branchJet->At(iJet);
	   	jetB = (Jet*) branchJet->At(jJet);
	   	muA = (Muon*) branchMuon->At(iMuon);
	   	muB = (Muon*) branchMuon->At(jMuon);
		muA_gen = (GenParticle*) muA->Particle.GetObject();
		muB_gen = (GenParticle*) muB->Particle.GetObject();


			int nJetAConstituents = jetA->Constituents.GetEntriesFast();
			int nJetBConstituents = jetB->Constituents.GetEntriesFast();

			p_jetB_Const = TLorentzVector(0.0,0.0,0.0,0.0);

			// - Loop over all of the jet constituents
		   for(int iJetConst = 0; iJetConst < nJetAConstituents; iJetConst++)
		   {
		
				object = NULL;
		      object = jetA->Constituents.At(iJetConst);
		
				// Check if the constituent is accessible
				if(object == 0) continue;
				
				
				if(object->IsA() == GenParticle::Class())
				{
				  particle = (GenParticle*) object;
				  //	  std::cout << "    GenPart pt: " << particle->PT << ", eta: " << particle->Eta << ", phi: " << particle->Phi << std::endl;
				  p_jetA_Const += particle->P4();
				}
				else if(object->IsA() == Track::Class())
				{
				  track = (Track*) object;
				  //	  std::cout << "    Track pt: " << track->PT << ", eta: " << track->Eta << ", phi: " << track->Phi << std::endl;
				  p_jetA_Const += track->P4();
				
				}
				else if(object->IsA() == Tower::Class())
				{
				  tower = (Tower*) object;
				  //	  std::cout << "    Tower pt: " << tower->ET << ", eta: " << tower->Eta << ", phi: " << tower->Phi << std::endl;
				  p_jetA_Const += tower->P4();

				}
			}

			// - Loop over all of the jet constituents
		   for(int jJetConst = 0; jJetConst < nJetBConstituents; jJetConst++)
		   {
		
				object = NULL;
		      object = jetB->Constituents.At(jJetConst);
		
				// Check if the constituent is accessible
				if(object == 0) continue;
				
				
				if(object->IsA() == GenParticle::Class())
				{
				  particle = (GenParticle*) object;
				  //	  std::cout << "    GenPart pt: " << particle->PT << ", eta: " << particle->Eta << ", phi: " << particle->Phi << std::endl;
				  p_jetB_Const += particle->P4();
				}
				else if(object->IsA() == Track::Class())
				{
				  track = (Track*) object;
				  //	  std::cout << "    Track pt: " << track->PT << ", eta: " << track->Eta << ", phi: " << track->Phi << std::endl;
				  p_jetB_Const += track->P4();
				
				}
				else if(object->IsA() == Tower::Class())
				{
				  tower = (Tower*) object;
				  //	  std::cout << "    Tower pt: " << tower->ET << ", eta: " << tower->Eta << ", phi: " << tower->Phi << std::endl;
				  p_jetB_Const += tower->P4();

				}
			}

			TLorentzVector p_jj     = jetA->P4() + jetB->P4();
			TLorentzVector p_mumu   = muA->P4() + muB->P4();
			TLorentzVector p_jjmumu     = p_jj + p_mumu;
			TLorentzVector p_jjmumu_gen = p_jetA_Const + p_jetB_Const + muA_gen->P4() + muB_gen->P4();

			histo->mZhDistr[jet][reco]->Fill( p_jjmumu.M()     );
			histo->mZhDistr[jet][gene]->Fill( p_jjmumu_gen.M() );

			if ( (jetA->BTag == true) && (jetB->BTag == true) ) 
			{
				histo->mZhDistr[bjet][reco]->Fill( p_jjmumu.M()     );
				histo->mZhDistr[bjet][gene]->Fill( p_jjmumu_gen.M() );
			}

			if ( (CutJet(jetA) == true) && (CutJet(jetB) == true) && (CutMuon(muA) == true) && (CutMuon(muB)== true) && (Cuthcandidate(p_jj)) && (CutZcandidate(p_mumu))  )
			{
				histo->mZhDistr[jet][recocut]->Fill( p_jjmumu.M()     );
				histo->mZhDistr[jet][genecut]->Fill( p_jjmumu_gen.M() );

				if ( (jetA->BTag == true) && (jetB->BTag == true) ) 
				{
					histo->mZhDistr[bjet][recocut]->Fill( p_jjmumu.M()     );
					histo->mZhDistr[bjet][genecut]->Fill( p_jjmumu_gen.M() );
				}
			}

		}

		////////////////////////////////
		// -- background test setup-- // 
		////////////////////////////////


		int nElectrons = branchElectron->GetEntries();
		int zCandidate = 0;

		//Check exactly two leptons in event
		if(nMuons == 2){zCandidate=1;}
		else if(nElectrons == 2){zCandidate=2;}
		else continue;
		
		missingET = (MissingET*) branchMissingET->At(0);		
		
		for(int iJet = 0; iJet < nJets; iJet++)
		for(int jJet = iJet+1; jJet < nJets; jJet++)

		{

	   	jetA = (Jet*) branchJet->At(iJet);
	   	jetB = (Jet*) branchJet->At(jJet);

			int nJetAConstituents = jetA->Constituents.GetEntriesFast();
			int nJetBConstituents = jetB->Constituents.GetEntriesFast();

			p_jetB_Const = TLorentzVector(0.0,0.0,0.0,0.0);

			// - Loop over all of the jet constituents
		   for(int iJetConst = 0; iJetConst < nJetAConstituents; iJetConst++)
		   {
		
				object = NULL;
		      object = jetA->Constituents.At(iJetConst);
		
				// Check if the constituent is accessible
				if(object == 0) continue;
				
				
				if(object->IsA() == GenParticle::Class())
				{
				  particle = (GenParticle*) object;
				  //	  std::cout << "    GenPart pt: " << particle->PT << ", eta: " << particle->Eta << ", phi: " << particle->Phi << std::endl;
				  p_jetA_Const += particle->P4();
				}
				else if(object->IsA() == Track::Class())
				{
				  track = (Track*) object;
				  //	  std::cout << "    Track pt: " << track->PT << ", eta: " << track->Eta << ", phi: " << track->Phi << std::endl;
				  p_jetA_Const += track->P4();
				
				}
				else if(object->IsA() == Tower::Class())
				{
				  tower = (Tower*) object;
				  //	  std::cout << "    Tower pt: " << tower->ET << ", eta: " << tower->Eta << ", phi: " << tower->Phi << std::endl;
				  p_jetA_Const += tower->P4();

				}
			}

			// - Loop over all of the jet constituents
		   for(int jJetConst = 0; jJetConst < nJetBConstituents; jJetConst++)
		   {
		
				object = NULL;
		      object = jetB->Constituents.At(jJetConst);
		
				// Check if the constituent is accessible
				if(object == 0) continue;
				
				
				if(object->IsA() == GenParticle::Class())
				{
				  particle = (GenParticle*) object;
				  //	  std::cout << "    GenPart pt: " << particle->PT << ", eta: " << particle->Eta << ", phi: " << particle->Phi << std::endl;
				  p_jetB_Const += particle->P4();
				}
				else if(object->IsA() == Track::Class())
				{
				  track = (Track*) object;
				  //	  std::cout << "    Track pt: " << track->PT << ", eta: " << track->Eta << ", phi: " << track->Phi << std::endl;
				  p_jetB_Const += track->P4();
				
				}
				else if(object->IsA() == Tower::Class())
				{
				  tower = (Tower*) object;
				  //	  std::cout << "    Tower pt: " << tower->ET << ", eta: " << tower->Eta << ", phi: " << tower->Phi << std::endl;
				  p_jetB_Const += tower->P4();

				}
			}


			if (zCandidate == 1)
			{
			muA = (Muon*) branchMuon->At(0);
			muB = (Muon*) branchMuon->At(1);
		   	muA_gen = (GenParticle*) muA->Particle.GetObject();
		   	muB_gen = (GenParticle*) muB->Particle.GetObject();
			
			TLorentzVector p_jj     = jetA->P4() + jetB->P4();
			TLorentzVector p_mumu   = muA->P4() + muB->P4();
			TLorentzVector p_jjmumu     = p_jj + p_mumu;
			TLorentzVector p_jjmumu_gen = p_jetA_Const + p_jetB_Const + muA_gen->P4() + muB_gen->P4();
			Ht = muA->PT + muB->PT + jetA->PT + jetB->PT;


			if ( (CutJets(jetA, jetB) == true) && (CutMuons(muA, muB) == true) && (Cuthcandidate(p_jj)) & (CutZcandidate(p_mumu)) && (CutMissingET(missingET, Ht)))
			{
				histo->backDistr[jet][recocut]->Fill( p_jjmumu.M()     );
				histo->backDistr[jet][genecut]->Fill( p_jjmumu_gen.M() );


			}
			}

			else if (zCandidate == 2)
			{
			elA = (Electron*) branchElectron->At(0);
			elB = (Electron*) branchElectron->At(1);
		   	elA_gen = (GenParticle*) elA->Particle.GetObject();
		   	elB_gen = (GenParticle*) elB->Particle.GetObject();

			TLorentzVector p_jj     = jetA->P4() + jetB->P4();
			TLorentzVector p_mumu   = elA->P4() + elB->P4();
			TLorentzVector p_jjmumu     = p_jj + p_mumu;
			TLorentzVector p_jjmumu_gen = p_jetA_Const + p_jetB_Const + elA_gen->P4() + elB_gen->P4();
		//	Ht = elA->PT + elB->PT + jetA->PT + jetB->PT;


			if ( (CutJets(jetA, jetB) == true) && (CutElectrons(elA, elB)) && (Cuthcandidate(p_jj)) && (CutZcandidate(p_mumu)) && (CutMissingET(missingET, Ht))  )
			{
				histo->backDistr[jet][recocut]->Fill( p_jjmumu.M()     );
				histo->backDistr[jet][genecut]->Fill( p_jjmumu_gen.M() );

			}
			}

		}


		
		} // end-of-iEv-loop


//	   p_mumujj = p_mumu + p_jj;
	

	std::cout << std::endl;
	std::cout << "AnalysisFW::Analyzer finished." << std::endl;

}
