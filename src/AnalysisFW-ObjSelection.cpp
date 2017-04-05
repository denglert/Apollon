#include "AnalysisFW.h"

//////////////////////////////////////
// -- Object selection functions -- //
//////////////////////////////////////


// - CutJet
bool AnalysisFW::CutJet(Jet* jet)
{
	if ( jet->PT       < cuts.cutJetPtMin  ) return false;
	if ( abs(jet->Eta) > cuts.cutJetEtaMax ) return false;

	return true;
}


// - CutJets
bool AnalysisFW::CutJets(Jet* jet1, Jet* jet2)
{
	if ( jet1->BTag == 0 || jet2->BTag == 0) return false;
	if ( jet1->PT < cuts.cutJetPtMin && jet2->PT < cuts.cutJetPtMin ) return false;
	if ( abs(jet1->Eta) > cuts.cutJetEtaMax || abs(jet2->Eta) > cuts.cutJetEtaMax) return false;

	return true;
}

// - LeptonCount
int AnalysisFW::LeptonCount(int nMuons, int nElectrons)
{
	if(nMuons == 2) return 1;
	if(nElectrons == 2) return 2;

	return 3; //Failure condition

}

// - CutMuon
bool AnalysisFW::CutMuon(Muon* mu)
{
	if ( mu->PT  < cuts.cutMuonPtMin       ) return false;
	if ( abs(mu->Eta) > cuts.cutMuonEtaMax ) return false;

	return true;
}


// - CutMuons
bool AnalysisFW::CutMuons(Muon* mu1, Muon* mu2)
{
	
	if ( mu1->PT  < cuts.cutMuonPtMin || mu2->PT < cuts.cutMuonPtMin     ) return false;
	if ( abs(mu1->Eta) > cuts.cutMuonEtaMax && abs(mu2->Eta) > cuts.cutMuonEtaMax) return false;
	if ( mu1->Charge + mu2->Charge != 0 ) return false;

	return true;
}


// - CutElectron
bool AnalysisFW::CutElectron(Electron* el)
{
	if ( el->PT  < cuts.cutElectronPtMin ) return false;
	if ( abs(el->Eta) > cuts.cutElectronEtaMax     ) return false;

	return true;

}


// - CutElectrons
bool AnalysisFW::CutElectrons(Electron* el1, Electron* el2)
{
	if ( el1->PT  < cuts.cutElectronPtMin || el2->PT < cuts.cutElectronPtMin     ) return false;
	if ( abs(el1->Eta) > cuts.cutElectronEtaMax && abs(el2->Eta) > cuts.cutElectronEtaMax) return false;
	if ( el1->Charge + el2->Charge != 0 ) return false;

	return true;
}


// - CutZcandidate
bool AnalysisFW::CutZcandidate(TLorentzVector const &p)
{
	double m = p.M();

	if ( m < cuts.cutZMinvMin ) return false;
	if ( cuts.cutZMinvMax < m ) return false;

	double pt = p.Pt();

	if ( pt < cuts.cutZPtMin ) return false;
	if ( cuts.cutZPtMax < pt ) return false;

	return true;
}

// - CutBjet
bool AnalysisFW::CutBjet_parton(GenParticle *p)
{

	double pt = p->PT;

	if ( pt < cuts.cutBJetPtMin ) return false;

	return true;
}


// - Cuthcandidate
bool AnalysisFW::Cuthcandidate(TLorentzVector const &p)
{
	double m = p.M();
	if ( m < cuts.cuthMinvMin ) return false;
	if ( cuts.cuthMinvMax < m ) return false;

	return true;
}

// - CutMissingET
bool AnalysisFW::CutMissingET(MissingET* missingET, double Ht)
{

	if ( missingET->MET/sqrt(Ht) > cuts.cutMissingETMax ) return false;

	return true;
}

//
bool AnalysisFW::is_b_from_higgs(GenParticle *particle, TClonesArray *branchParticle )
{
	if (abs(particle->PID) != 5) return false;
	int M1 = particle->M1;
	GenParticle *m = (GenParticle*)branchParticle->At(M1);
	if (abs(m->PID) != 25) return false;
	return true;
}
