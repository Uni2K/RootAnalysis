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
#include "TPaveStats.h"

namespace fs = std::experimental::filesystem;
using namespace std;
struct stat info;
float SP = 0.3125; // ns per bin

int main(int argc, char *argv[])
{
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0000);
	gStyle->SetGridStyle(3);
	gStyle->SetGridWidth(1);
	gStyle->SetGridColor(16);
	gStyle->SetLineScalePS(1);
	gStyle->SetStatW(0.2);
	gStyle->SetStatH(0.2);
	string run_name = (string)argv[1];
	int run_nr = atoi(argv[2]);
	string in_dir = string(argv[3]);

	string out_dir = Form("./percentage_histograms/%s/", run_name.c_str());
	if (!fs::is_directory("./percentage_histograms") || !fs::exists("./percentage_histograms"))
	{
		fs::create_directory("./percentage_histograms");
	}

	if (!fs::is_directory(out_dir) || !fs::exists(out_dir))
	{
		fs::create_directory(out_dir);
	}

	TFile *file = new TFile(in_dir.c_str());
	if (file->IsZombie())
	{
		cout << "PROBLEM with the initialization of the output ROOT ntuple "
			 << in_dir << ": check that the path is correct!!!"
			 << endl;

		exit(-1);
	}
	TTree *tree;
	file->GetObject("T", tree);

	// to show generalized poisson fit
	TCanvas *C1 = new TCanvas("C1", "PercentageAnalysis", 1000, 800);
	C1->Divide(6, 6);

	int channelNumber = 16;
	if (run_name.find("32") != std::string::npos)
	{
		channelNumber = 32;
	}
	gStyle->SetOptFit(0000);

	for (int i = 0; i < channelNumber; ++i)
	{
		C1->cd(i + 1);

		int xmin = 0.5;
		int xmax = 1;
		int nBins = 100;

		TH1F *h = new TH1F("h", "Percentage", nBins, xmin, xmax);
		h->SetLineColorAlpha(kBlack, 0.7);
		h->SetMarkerStyle(7);
		h->SetMarkerColorAlpha(kBlack, 0.6);

		TString cut("");
		tree->Draw(Form("IntegralDifference[%d]>>h", i), cut);
		TF1 *gauss=new TF1("peak", "gaus",0, 1);

		h->Fit("peak", "RQ+");
	
		TLegend *h_leg = new TLegend(0.1, 0.81, 0.35, 0.9);
		h_leg->SetTextSize(0.03);
		double mean=gauss->GetParameter(1);
		h_leg->AddEntry(gauss, Form("Mean: %lf", mean));
		h_leg->Draw();


	}

	C1->Print((out_dir + run_name + ".pdf").c_str());

	return 0;
}