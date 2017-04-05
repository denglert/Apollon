#include "AnalysisFW.h"

///////////////////////////////
// -- Category/Level/Mult -- //
///////////////////////////////

enum Category { Z, h, Zh };
enum Level    { parton, Z_cuts, bb, bb_cuts, Z_and_bb_cuts };
enum Mult     { defa     };
enum Zh       { Zh_ };

////////////////////
//                //
// -- Analyzer -- //
//                //
////////////////////

// --- Prerequisites:
//   - needs ExRootTreeReader reader set up already.

void AnalysisFW::Analyzer( TChain *chain, ResultContainer<TH1D> *histo)
{
	std::cout << std::endl;
	std::cout << "--- AnalysisFW::Analyzer --- started." << std::endl;
	std::cout << std::endl;

	// -  Create object of class ExRootTreeReader
	reader = NULL;
	reader = new ExRootTreeReader(chain);
	if (reader == NULL)
	{std::cerr << "Error!" << std::endl << "ExRootTreeReader didn't manage to load in the chain" << std::endl;}

	// - Get pointers to branches used in this analysis
	TClonesArray *branchParticle 				= reader->UseBranch("Particle");
	GenParticle *particle = NULL;

	// - Smearing
	TRandom3 *rand   = new TRandom3();
	const double res = 0.025;

	TLorentzVector p_h;
	TLorentzVector p_Z;

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


//		printf("-------------------------\n");
//		printf("---    iEv: %03d   ---\n", iEv);
//		printf("-------------------------\n");

		int nParticles = branchParticle->GetEntries();

 		int iPart = 0;

		bool hFound = false;
		bool h_bb   = false;
		bool ZFound = false;
		bool Z_cut_pass = false;
		bool b_cut_pass = false;


		while ( ! ( hFound && ZFound) && (iPart < nParticles) )
		{
	
			  particle = (GenParticle *) branchParticle->At(iPart);
	
			  int PID    = particle->PID;
			  int Status = particle->Status;
			
				if ( (particle->PID == 25) && (particle->Status == 3) )
				{ 

					  p_h = particle->P4();
					  hFound = true;

					  histo->PtDistr [h][defa][parton]->Fill( p_h.Pt()  );
					  histo->EtaDistr[h][defa][parton]->Fill( p_h.Eta() );

					  int D1_id = particle->D1;
					  int D2_id = particle->D2;

					  GenParticle *D1;
					  GenParticle *D2;

					  int D1_pid;
					  int D2_pid;

					  if ( (D1_id != -1) && (D1_id != -1) )
					  {
			  		  		D1 = (GenParticle *) branchParticle->At(D1_id);
			  		  		D2 = (GenParticle *) branchParticle->At(D2_id);
					  		D1_pid = D1->PID;
                 		D2_pid = D2->PID;
							// Debuggg 
							std::cerr << "D1_pid " << D1_pid << std::endl;
							std::cerr << "D2_pid " << D2_pid << std::endl;

						   if (  abs(D1->PID) == 5 && abs(D1->PID) == 5)
							{ 
								
								h_bb = true;
		
		
								histo->PtDistr [h][defa][h_bb]->Fill( p_h.Pt()  );
								histo->EtaDistr[h][defa][h_bb]->Fill( p_h.Eta() );
		
		
								if ( CutBjet_parton(D1) && CutBjet_parton(D2) )
								{
		
									b_cut_pass = true;
									histo->PtDistr [h][defa][bb_cuts]->Fill( p_h.Pt()  );
									histo->EtaDistr[h][defa][bb_cuts]->Fill( p_h.Eta() );
		
								}
		
							}

					  }

//		  			printf("h found: %5d %1d\n", PID, Status);
				}

				if ( (particle->PID == 23) && (particle->Status == 3) )
				{ 
					p_Z = particle->P4();
					ZFound = true;

					histo->PtDistr  [Z][defa][parton]->Fill( p_Z.Pt()  );
					histo->EtaDistr [Z][defa][parton]->Fill( p_Z.Eta() );

					if ( CutZcandidate(p_Z) )
					{

						Z_cut_pass = true;

						histo->PtDistr  [Z][defa][Z_cuts]->Fill( p_Z.Pt()  );
						histo->EtaDistr [Z][defa][Z_cuts]->Fill( p_Z.Eta() );

					}

//		  			printf("Z found: %5d %1d\n", PID, Status);
				}
				
				iPart++;
//	    		printf("iPart/nParticles: %d / %d |||| ZFound: %d | hFound: %d\n", iPart, nParticles, ZFound, hFound);
	
		}



		TLorentzVector p_Zh = p_Z + p_h;	
		double m_Zh = p_Zh.M();

		if ( isSmear )
		{
			double x = rand->Gaus( 0.0, res );
			m_Zh = m_Zh * (1.0 + x);
		}

		histo->mZhDistr[Zh_][defa] -> Fill( m_Zh );
	
		if ( Z_cut_pass )
		{
			histo->mZhDistr[Zh_][Z_cuts] -> Fill( m_Zh );
		}


		if ( b_cut_pass )
		{
			histo->mZhDistr[Zh_][bb_cuts] -> Fill( m_Zh );
		}

		if ( b_cut_pass & Z_cut_pass )
		{
			histo->mZhDistr[Zh_][Z_and_bb_cuts] -> Fill( m_Zh );
		}



	}

//				if ( (particle->PID == 23) && (particle->Status == 3) )
//				{ p_Z = particle->P4(); }


	std::cout << std::endl;
	std::cout << "AnalysisFW::Analyzer finished." << std::endl;

}
