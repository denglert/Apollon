#ifndef UTILFUNCTIONS_H
#define UTILFUNCTIONS_H

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <TH1D.h>
#include <TFile.h>
#include <THStack.h>
#include "config.h"
#include "GraphTools.h"
#include "AllocationUtils.h"

#define loopx(x, nx)                 for( int x = 0; x < nx; x++)
#define loopxy(x, y, nx, ny)         for( int x = 0; x < nx; x++)for( int y = 0; y < ny; y++)
#define loopxyz(x, y, z, nx, ny, nz) for( int x = 0; x < nx; x++)for( int y = 0; y < ny; y++)for( int z = 0; z < nz; z++)

#define MAXNCOMPONENT 10

// -- Binning -- //
class Binning
{

	public:
	void LoadBinningConfig( config *conf );

	int nCat;
	int nMult;
	int nLevel; 

	int nCatmZh;

	std::vector< std::string > tagCatName;
	std::vector< std::string > tagLevelName;
	std::vector< std::string > tagMultName;

	std::vector< std::string > labelCatName;
	std::vector< std::string > labelLevelName;
	std::vector< std::string > labelMultName;


	std::string tag_Cat  (const int  id);
	std::string tag_Mult (const int  id);
	std::string tag_Level  (const int  id);

	std::string label_Cat  (const int  id);
	std::string label_Mult (const int  id);
	std::string label_Level  (const int  id);

	double nPtBins;
	double PtMin;
	double PtMax;

	double nMinvBins;
	double MinvMin;
	double MinvMax;

	double nmZhBins;
	double mZhMin;
	double mZhMax;

	double nEtaBins;
	double EtaMin;
	double EtaMax;

	double nObjBins;
	double ObjMin;
	double ObjMax;

	double nEventsBins;
	double EventMin;
	double EventMax;	  	
};


// -- Cuts -- //
struct Cuts
{
	double cutMuonPtMin;
	double cutMuonEtaMax;

	double cutElectronPtMin;
	double cutElectronEtaMax;

	double cutJetPtMin;
	double cutJetEtaMax;

	double cutZMinvMin;
	double cutZMinvMax;

	double cutZPtMin;
	double cutZPtMax;

	double cutBJetPtMin;

	double cuthMinvMin;
	double cuthMinvMax;

	double cutMissingETMax;
};

void LoadCutsConfig(    config *conf, Cuts *cuts);

// -- Components -- //
struct Components
{
	double lumi;
	int   nComp;

	double      component_xsec    [MAXNCOMPONENT];
	double      component_scale   [MAXNCOMPONENT];
	double      component_nEvents [MAXNCOMPONENT];
	std::string component_name    [MAXNCOMPONENT];
	std::string component_path    [MAXNCOMPONENT];
	int         component_color   [MAXNCOMPONENT];
};


// -- Default labels -- //
std::string label_Cat          ( const int id );
std::string label_Level        ( const int id );
std::string label_Multiplicity ( const int id );
std::string tag_Cat            ( const int id );
std::string tag_Multiplicity   ( const int id );
std::string tag_Level          ( const int id );

// -- Progress bar -- //
void progress_bar( double percent );

#endif
