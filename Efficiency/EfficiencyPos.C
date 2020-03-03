//root
#include <TLine.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TStyle.h>
#include <TRandom.h>
#include <TString.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TMath.h>
#include <TLegend.h>
#include <TCut.h>
#include <THStack.h>
#include <TGaxis.h>
#include <TF1.h>
#include <TError.h> // root verbosity level
#include <TApplication.h>
#include <experimental/filesystem>
#include <TMultiGraph.h>
#include <TF1Convolution.h>
#include <TAttMarker.h>
#include <TGraphAsymmErrors.h>
//C, C++
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <TGraph.h>
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "TEfficiency.h"
#include "TLatex.h"
#include "TPaveLabel.h"
#include <algorithm>

namespace fs = std::experimental::filesystem;
using namespace std;
struct stat info;

string outputFolder = "./EfficiencyPlots";
string effFile = outputFolder + "/Efficiencies.txt";
pair<int, int> pos0 = make_pair(0, 0);
pair<int, int> pos1 = make_pair(160, 160);
pair<int, int> pos2 = make_pair(320, 320);
pair<int, int> pos3 = make_pair(-320, 320);
pair<int, int> pos4 = make_pair(-160, 160);
pair<int, int> pos5 = make_pair(-160, -160);
pair<int, int> pos6 = make_pair(-320, -320);
pair<int, int> pos7 = make_pair(320, -320);
pair<int, int> pos8 = make_pair(160, -160);
pair<int, int> pos9 = make_pair(160, -510);
pair<int, int> pos10 = make_pair(204, -404);
pair<int, int> pos11 = make_pair(310, -360);





pair<int, int> posWOMD = make_pair(310, -510);
float calculateDistance(pair<int, int> p1, pair<int, int> p2)
{
	return sqrt(pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2));
}
float getDistanceFromWOMD(int pos)
{
	//pos of WOM D: x/y -> 310,-510

	switch (pos)
	{
	case 0:
		return calculateDistance(posWOMD, pos0);
	case 1:
		return calculateDistance(posWOMD, pos1);
	case 2:
		return calculateDistance(posWOMD, pos2);
	case 3:
		return calculateDistance(posWOMD, pos3);
	case 4:
		return calculateDistance(posWOMD, pos4);
	case 5:
		return calculateDistance(posWOMD, pos5);
	case 6:
		return calculateDistance(posWOMD, pos6);
	case 7:
		return calculateDistance(posWOMD, pos7);
	case 8:
		return calculateDistance(posWOMD, pos8);
	case 9:
		return calculateDistance(posWOMD, pos9);
	case 10:
		return calculateDistance(posWOMD, pos10);
	case 11:
		return calculateDistance(posWOMD, pos11);
	default:
		return -1000;
	}
}

vector<string> split(const string &str, const string &delim)
{
	vector<string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos)
			pos = str.length();
		string token = str.substr(prev, pos - prev);
		if (!token.empty())
			tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}

int main(int argc, char *argv[])
{
	gStyle->SetOptStat(0);
	gStyle->SetGridStyle(3);
	gStyle->SetGridWidth(1);
	gStyle->SetGridColor(16);
	gStyle->SetLineScalePS(1);
	gStyle->SetStatW(0.2);
	gStyle->SetStatH(0.2);
	gStyle->SetLineStyleString(11, "5 5");
	gErrorIgnoreLevel = kError;

	//Output Plot Folder
	string outDir = outputFolder + "/";
	if (!fs::is_directory(outputFolder) || !fs::exists(outputFolder))
	{
		fs::create_directory(outputFolder);
	}
	if (!fs::is_directory(outDir) || !fs::exists(outDir))
	{
		fs::create_directory(outDir);
	}

	std::ifstream inFile(effFile);
	int numberOfLines = std::count(std::istreambuf_iterator<char>(inFile),
								   std::istreambuf_iterator<char>(), '\n');

	ifstream data_store(effFile);

	TCanvas *effCanvas = new TCanvas("effCanvas", "Sum Histogram", 1000, 1000);
	effCanvas->SetGrid();

	map<int, map<int, vector<float>>> dataMap; //Key= Energy , Val: Runs<ValueTypes<Values>>>

	TMultiGraph *mg = new TMultiGraph();

	for (string lineWithDataMeasurement; getline(data_store, lineWithDataMeasurement);)
	{

		string effName = lineWithDataMeasurement.substr(0, lineWithDataMeasurement.find("=")); //12_pos3_angle0_e52_ch32
		if (effName.find("dc") != std::string::npos) {
				continue;
			}
		vector<string> rawData = split(effName, "_");
		string runPosRaw = rawData[1];

		int runPos = stoi(runPosRaw.substr(3, runPosRaw.size() - 1));
		int runEnergy = stoi(rawData[3].substr(1, rawData[3].size()));
		string dataStr = lineWithDataMeasurement.substr(lineWithDataMeasurement.find("=") + 1, lineWithDataMeasurement.size());
		vector<string> data = split(dataStr, "/");
		float eff = stof(data[0]);
		float errP = stof(data[1]);
		float errM = stof(data[2]);

		vector<float> valueTypes;

		valueTypes.push_back(eff);
		valueTypes.push_back(errP);
		valueTypes.push_back(errM);

		map<int, vector<float>> c = dataMap[runEnergy];
		c[runPos] = valueTypes;
		dataMap[runEnergy] = c;
	}
		TLegend *h_leg = new TLegend(0.01, 0.3, 0.3, 0.45);
		h_leg->SetTextSize(0.02);

	
		cout<<"test2"<<endl;

	int colorCounter=1;
	for (std::pair<int, map<int, vector<float>>> element : dataMap)
	{
		int energy = element.first;
		map<int, vector<float>> data = element.second;
		printf("Doing Energy: %d \n", energy);
		vector<float> x;
		vector<float> y;
		vector<float> yErrU;
		vector<float> yErrL;

		for (std::pair<int, vector<float>> posRun : data)
		{
			int runPos = posRun.first;
			vector<float> dat = posRun.second;
			x.push_back(getDistanceFromWOMD(runPos));
			y.push_back(dat[0]);
			yErrU.push_back(dat[1]);
			yErrL.push_back(dat[2]);
			printf("Doing Position: %d , Eff: %1.4f (%1.4f,%1.4f)\n", runPos, dat[0], dat[1], dat[2]);
		}

		const Int_t n = x.size();
		TGraphAsymmErrors *gr = new TGraphAsymmErrors(n, &(x[0]), &(y[0]), 0, 0, &(yErrL[0]), &(yErrU[0]));
		gr->SetTitle("Energy");
		gr->SetLineColor(colorCounter);
		gr->SetLineWidth(1.5);
		gr->SetMarkerColor(colorCounter);
		gr->SetMarkerSize(1.0);
		gr->SetMarkerStyle(20);
		gr->GetXaxis()->SetLabelSize(0.035);
		gr->GetYaxis()->SetLabelSize(0.035);
		h_leg->AddEntry(gr, Form("Energy: %1.2f GeV",energy/10.0), "p");

		TF1 *f1 = new TF1("fit","-[0]*exp(x*[2])+[1]",0,1000);
		f1->SetParameter(0,0.01);
		f1->SetParameter(1,100);
		f1->SetParameter(2,0.009);
		//gr->Fit("fit");



		mg->Add(gr);
		gStyle->SetTitleSize(0.2);
		mg->SetTitle("Efficiency Measurement");
		
		colorCounter++;
	}
	mg->Draw("AP ");

	TAxis *yaxisP = mg->GetYaxis();
	TAxis *xaxisP = mg->GetXaxis();
	yaxisP->SetLabelSize(0.02);
	yaxisP->SetTitle("Efficiency [%]");
	yaxisP->SetTitleSize(0.025);
	xaxisP->SetLabelSize(0.02);
	xaxisP->SetTitle("Distance [mm]");
	xaxisP->SetTitleSize(0.025);
	h_leg->SetTextFont(42);
	h_leg->Draw();

	effCanvas->Print((outDir + "PosEfficiency.pdf").c_str());

	return 0;
}