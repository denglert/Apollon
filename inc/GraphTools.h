#ifndef GRAPHTOOLS_H
#define GRAPHTOOLS_H

#include <map>
#include <TStyle.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <THStack.h>
#include <TText.h>



const int Colors[5] = {4, 2, 1, 3, 9};

const double leg_pos_x = 0.75;
const double leg_pos_y = 0.80;
const double leg_wid_x = 0.30;
const double leg_wid_y = 0.10;

struct PlotSettings
{
	std::string PrePath;
	std::string FullPath;
	std::string Tag;
	std::string LegendLabel;
	std::string TextLabel;
	int UniqueID;

	bool isLegend    = false;
	bool isTextLabel = false;

	double TextLabelPosX;
	double TextLabelPosY;
	double TextLabelSize;

	std::string DrawingOption = "E";

	int MarkerColor = 1;
	int MarkerStyle = 1;
	int FillColor   = 1;

	double LeftMargin   = 0.14;
	double BottomMargin = 0.15;
	double TopMargin    = 0.10;
	double RightMargin  = 0.10;

	double Legend_Pos_X = 0.60;
	double Legend_Pos_Y = 0.70;
	double Legend_Wid_X = 0.28;
	double Legend_Wid_Y = 0.15;
	double Legend_Text_Size = 0.025;

	std::map<TObject*, PlotSettings> fPlotMap;
};

TStyle* TStyle_Scheme ();

template<class T>
void CreatePlot ( T *histo, PlotSettings const& settings);

//void CreateStackPlot (THStack *hstack, PlotSettings const& settings );

#endif
