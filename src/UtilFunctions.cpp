#include "UtilFunctions.h"

std::string tag_Cat (const int  id)
{
	std::string out;

	switch (id)
	{
		case 0: out = "jet";  return out;
		case 1: out = "bjet"; return out;
		case 2: out = "mu";   return out;
	}
}

std::string tag_Level (const int  id)
{
	std::string out;

	switch (id)
	{
		case 0: out = "gene";     return out;
		case 1: out = "gene-cut"; return out;
		case 2: out = "reco";     return out;
		case 3: out = "reco-cut"; return out;
	}
}


std::string tag_Multiplicity (const int  id)
{
	std::string out;

	switch (id)
	{
		case 0: out = "";    return out;
		case 1: out = "di-"; return out;
	}
}


std::string label_Cat (const int  id)
{
	std::string out;

	switch (id)
	{
		case 0: out = "jet";   return out;
		case 1: out = "b-jet"; return out;
		case 2: out = "#mu";   return out;
	}
}


std::string label_Level (const int  id)
{
	std::string out;

	switch (id)
	{
		case 0: out = "GEN";      return out;
		case 1: out = "GEN-cut";  return out;
		case 2: out = "RECO";     return out;
		case 3: out = "RECO-cut"; return out;
	}
}

std::string label_Multiplicity (const int  id)
{
	std::string out;

	switch (id)
	{
		case 0: out = "";    return out;
		case 1: out = "di-"; return out;
	}
}


void progress_bar(double percent)
{
	std::string bar;
	
	double maxPercent = 100;
	int barWidth      = 50;
	double ratio      = barWidth/maxPercent; 

	for(int i = 0; i < barWidth; i++)
	{
	    if( i < (percent*ratio))
	    {
	        bar.replace(i,1,"=");
	    }
	    else if( i == (percent*ratio))
	    {
	        bar.replace(i,1,">");
	    }
	    else
	    {
	        bar.replace(i,1," ");
	    }
	}
	
	std::cout << "\033[1;34m" << "\r" "[" << bar << "] ";
	std::cout << "\033[0m" << percent << "%" << std::flush;
}

/////////////////////
// --- Binning --- //
/////////////////////

void Binning::LoadBinningConfig( config *conf)
{
	// Bins
	nCat      = getconfig((*conf), "nCat"  );
	nMult     = getconfig((*conf), "nMult" );
	nLevel    = getconfig((*conf), "nLevel");

	nCatmZh   = getconfig((*conf), "nCatmZh");

	nPtBins   = getconfig((*conf), "nPtBins");
	PtMin     = getconfig((*conf), "PtMin"  );
	PtMax     = getconfig((*conf), "PtMax"  );

	nEtaBins  = getconfig((*conf), "nEtaBins");
	EtaMin    = getconfig((*conf), "EtaMin");
	EtaMax    = getconfig((*conf), "EtaMax");

	nMinvBins = getconfig((*conf), "nMinvBins");
	MinvMin   = getconfig((*conf), "MinvMin");
	MinvMax   = getconfig((*conf), "MinvMax");

	nmZhBins  = getconfig((*conf), "nmZhBins");
	mZhMin    = getconfig((*conf), "mZhMin");
	mZhMax    = getconfig((*conf), "mZhMax");

	nObjBins  = getconfig((*conf), "nObjBins");
	ObjMin    = getconfig((*conf), "ObjMin");
	ObjMax    = getconfig((*conf), "ObjMax");

	nEventsBins = getconfig((*conf), "nEventsBins");
	EventMin    = getconfig((*conf), "EventMin");
	EventMax    = getconfig((*conf), "EventMax");


	for(int i=0; i<nCat; i++)
	{
		tagCatName.push_back(   (std::string)getconfig((*conf), Form("tagCatName%d", i) ) );
		labelCatName.push_back( (std::string)getconfig((*conf), Form("labelCatName%d", i) ) );
	}


	for(int i=0; i<nMult; i++)
	{
		tagMultName.push_back(   (std::string)getconfig((*conf), Form("tagMultName%d", i) ) );
		labelMultName.push_back( (std::string)getconfig((*conf), Form("labelMultName%d", i) ) );
	}

	for(int i=0; i<nLevel; i++)
	{
		tagLevelName.push_back(   (std::string)getconfig((*conf), Form("tagLevelName%d", i) ) );
		labelLevelName.push_back( (std::string)getconfig((*conf), Form("labelLevelName%d", i) ) );
	}

}


std::string Binning::tag_Cat (const int  id)
{
	return tagCatName[id];
}


std::string Binning::tag_Level (const int  id)
{
	return tagLevelName[id];
}


std::string Binning::tag_Mult (const int  id)
{
	return tagMultName[id];
}


std::string Binning::label_Cat (const int  id)
{
	return labelCatName[id];
}


std::string Binning::label_Level (const int  id)
{
	return labelLevelName[id];
}

std::string Binning::label_Mult (const int  id)
{
	return labelMultName[id];
}


/////////////////
// --- Cuts -- //
/////////////////


void LoadCutsConfig(    config *conf, Cuts *cuts)
{
	// Cuts
	cuts->cutMuonPtMin      = getconfig((*conf), "cutMuonPtMin");
	cuts->cutMuonEtaMax     = getconfig((*conf), "cutMuonEtaMax");

	cuts->cutElectronPtMin  = getconfig((*conf), "cutElectronPtMin");
	cuts->cutElectronEtaMax = getconfig((*conf), "cutElectronEtaMax");

	cuts->cutJetPtMin       = getconfig((*conf), "cutJetPtMin");
	cuts->cutJetEtaMax      = getconfig((*conf), "cutJetEtaMax");

	cuts->cutZMinvMin       = getconfig((*conf), "cutZMinvMin");
	cuts->cutZMinvMax       = getconfig((*conf), "cutZMinvMax");

	cuts->cutBJetPtMin      = getconfig((*conf), "cutBjetPtMin");

	cuts->cutZPtMin         = getconfig((*conf), "cutZPtMin");
	cuts->cutZPtMax         = getconfig((*conf), "cutZPtMax");

	cuts->cuthMinvMin       = getconfig((*conf), "cuthMinvMin");
	cuts->cuthMinvMax       = getconfig((*conf), "cuthMinvMax");

	cuts->cutMissingETMax	= getconfig((*conf), "cutMissingETMax");
}
