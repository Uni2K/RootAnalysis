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

namespace fs = std::experimental::filesystem;
using namespace std;
struct stat info;
float SP = 0.3125; // ns per bin

float t_max_inRange(TH1F *hWave, float t1, float t2)
{

	hWave->GetXaxis()->SetRange(t1 / SP, t2 / SP);
	float t_max = hWave->GetXaxis()->GetBinCenter(hWave->GetMaximumBin());
	hWave->GetXaxis()->SetRange(1, 1024);
	return t_max;
}
float t_min_inRange(TH1F *hWave, float t1, float t2)
{

	hWave->GetXaxis()->SetRange(t1 / SP, t2 / SP);
	float t_max = hWave->GetXaxis()->GetBinCenter(hWave->GetMinimumBin());
	hWave->GetXaxis()->SetRange(1, 1024);
	return t_max;
}

float min_inRange(TH1F *hWave, float t1, float t2)
{
	TF1 *f1 = new TF1("f1", "pol0", t1, t2);
	double r1 = 0;
	double r2 = 0;

	hWave->GetXaxis()->SetRange(t1 / SP, t2 / SP); //window
	r1 = hWave->GetMinimumBin() * SP - 0.5;
	r2 = r1 + 1;

	hWave->Fit("f1", "QN", "", r1, r2);
	float max = f1->GetParameter(0);
	hWave->GetXaxis()->SetRange(1, 1024);

	return max;
}

Int_t npeaks = 4; //number of peaks to fit

Double_t fpeaks(Double_t *x, Double_t *par)
{
	Double_t result = par[0] + par[1] * x[0];
	for (Int_t p = 0; p < npeaks; p++)
	{
		Double_t norm = par[3 * p + 2];
		Double_t mean = par[3 * p + 3];
		Double_t sigma = par[3 * p + 4];
		result += norm * TMath::Gaus(x[0], mean, sigma);
	}
	//fpeaks = gaus_1+gaus_2+...+gaus_npeaks + linear_func.
	return result;
}

/*******************
__ FUNCTIONS ______
*******************/
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

	string out_dir = Form("./oscillation_histograms/%s/", run_name.c_str());
	if (!fs::is_directory("./oscillation_histograms") || !fs::exists("./oscillation_histograms"))
	{
		fs::create_directory("./oscillation_histograms");
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
	TCanvas *C1 = new TCanvas("C1", "OscillationAnalysis", 1000, 800);
	C1->Divide(6, 6);

	int channelNumber = 16;
	if (run_name.find("32") != std::string::npos)
	{
		channelNumber = 32;
	}

	for (int i = 0; i < channelNumber; ++i)
	{
		C1->cd(i + 1);
		TH1F *sumHist;
		string name = Form("hChSum_%d", i);
		cout << "DOING: " << name << endl;
		file->GetObject(name.c_str(), sumHist);
		sumHist->Draw("hist");

		float t_amp = t_max_inRange(sumHist, 0.0, 320.0);
		float minY = sumHist->GetMinimum();
		float maxY = sumHist->GetMaximum();

		TSpectrum *s = new TSpectrum(npeaks, 15); // TSpectrum class finds candidates peaks
		Int_t nfound = s->Search(sumHist, 0, "goff", 0.001);

		//3.--------- Background Estimation
		TF1 *fline = new TF1("fline", "pol1", 0, 100); // linear function (pol1) defined between [0,650]
		fline->SetLineColor(4);						   // 4 = "blue"
		fline->SetLineWidth(3);
		fline->SetLineStyle(2);
		sumHist->Fit("fline", "RQ+");
		fline->Draw("same");

		Double_t par[100]; //Array of fitting parameters
		// Loop on the found peaks for  setting initial parameters. Peaks at the background level will be disregarded from the fitting
		par[0] = fline->GetParameter(0);
		par[1] = fline->GetParameter(1);
		npeaks = 0;
		int p = 0;
		Double_t *xpeaks = s->GetPositionX(); //array with X-positions of the centroids found by TSpectrum
		cout << "xPEAKS" << xpeaks << endl;

		for (p = 0; p < nfound; p++)
		{
			Double_t xp = xpeaks[p];
			Int_t bin = sumHist->GetXaxis()->FindBin(xp);
			Double_t yp = sumHist->GetBinContent(bin);
			//Condition for valid peaks: height-sqrt(height) has to be greater than the bckgnd:
			if (yp - TMath::Sqrt(yp) < fline->Eval(xp))
				continue;
			if (xp < (t_amp - 100))
			{
				cout << "XP: " << xp << "  T_AMP: " << t_amp << endl;
				continue;
			}
			if (xp > (320))
			{
				cout << "XP2: " << xp << "  T_AMP: " << t_amp << endl;
				continue;
			}

			par[3 * npeaks + 2] = yp; //height
			par[3 * npeaks + 3] = xp; //centroid
			par[3 * npeaks + 4] = 50; //sigma -I've choosed it as same for all peaks-
			npeaks++;
		}

		printf("Found %d useful peaks to fit\n", npeaks);
		//printf("Parameter No 1,2 ---> linear background Fit\n");
		//printf("Parameters No 3+3*n ---> Gauss Constants\n");
		//printf("Parameters No 4+3*n ---> Gauss Means\n");
		//printf("Parameters No 5+3*n ---> Gauss Sigmas\n");

		TLegend *h_leg = new TLegend(0.65, 0.65, 0.99, 0.9);
		h_leg->SetTextSize(0.02);

		TF1 *peak_single[npeaks];
		Double_t par_single[3 * npeaks];
		Double_t chi2ndf[npeaks];
		Double_t meanError[npeaks];

		for (int i = 0; i < npeaks; ++i)
		{
			// run 47
			// float range = 12; // symmetric fit range for individual peaks
			double pos_peak = par[3 + 3 * i];
			float range = 5;
			peak_single[i] = new TF1("peak", "gaus", pos_peak - range, pos_peak + range);

			sumHist->Fit("peak", "RQ+");
			peak_single[i]->Draw("same");
			chi2ndf[i] = peak_single[i]->GetChisquare() / peak_single[i]->GetNDF();
			meanError[i] = peak_single[i]->GetParError(1);
			peak_single[i]->GetParameters(&par_single[3 * i]);
		}

		int j = 0;
		float periods[npeaks - 1];
		float periodErrors[npeaks - 1];
		float meanPeriod = 0;
		float meanPeriodError = 0;
		float means[npeaks];
		for (j = 1; j <= npeaks; j++)
		{
			means[j - 1] = par_single[1 + (j - 1) * 3];
			cout << j - 1 << "  " << means[j - 1] << endl;

			if (j < npeaks)
			{
				float period = par_single[1 + (j)*3] - par_single[1 + (j - 1) * 3];
				float periodError = sqrt(pow(meanError[j], 2) + pow(meanError[j - 1], 2));
				periods[j - 1] = period;
				periodErrors[j - 1] = periodError;
				h_leg->AddEntry((TObject *)0, Form("Period (%d-%d): %f +- %f", j - 1, j, period, periodError), "");
			}
		}

		float minSearchLeft = 1000;
		float minSearchRight = 1000;

		for (j = 0; j < npeaks; j++)
		{
			meanPeriod = meanPeriod + periods[j];
			meanPeriodError = meanPeriodError + pow(periodErrors[j], 2) + pow(periodErrors[j - 1], 2);
			if (means[j] < minSearchLeft)
			{
				minSearchLeft = means[j];
			}
			else if (means[j] < minSearchRight)
			{
				minSearchRight = means[j];
			}
			else
			{
			}
		}

		//WITHOUT STATISTICS ERROR
		meanPeriod = meanPeriod / (npeaks - 1);
		meanPeriodError = sqrt(meanPeriodError) / sqrt((npeaks - 1));

		h_leg->AddEntry((TObject *)0, Form("Mean Period: %f +- %f", meanPeriod, meanPeriodError), "");

		//Find first Minimum -> Search Between 0,1 Maxima
		//	TF1 *putc = new TF1("inversepeak", "-sqrt([0]^2)*exp(-0.5*((x-[1])/[2])^2)+[3]", par_single[1 + 3]-20, par_single[1 + 3]);
		if (npeaks > 1)
		{
			TF1 *putc = new TF1("inversepeak", "pol4", par_single[1 + 3] - 20, par_single[1 + 3]);
			putc->SetLineColor(kGreen);
			putc->SetParameter(0, 10);
			putc->SetParameter(1, 155);
			putc->SetParameter(2, 5);
			sumHist->Fit("inversepeak", "RQ+");
			putc->Draw("same");
		
		//	float minInRange = t_min_inRange(sumHist, minSearchLeft, minSearchRight);
		float minInRange = sumHist->GetFunction("inversepeak")->GetMinimumX(par_single[1 + 3] - 20, par_single[1 + 3]);

		TLine *minLine = new TLine(minInRange, minY, minInRange, maxY);
		minLine->SetLineColor(3);
		minLine->SetLineWidth(1);
		minLine->Draw();

		float entireSignalRight = 3 * meanPeriod + minInRange;
		TLine *entireSignalRightLine = new TLine(entireSignalRight, minY, entireSignalRight, maxY);
		entireSignalRightLine->SetLineColor(7);
		entireSignalRightLine->SetLineWidth(1);
		entireSignalRightLine->Draw();


		h_leg->AddEntry((TObject *)0, Form("First Minimum: %f", minInRange), "");
		h_leg->AddEntry((TObject *)0, Form("Distance to max: %f", (minInRange - means[0])), "");
		h_leg->AddEntry((TObject *)0, Form("Distance to end: %f", (entireSignalRight-means[0])), "");


		}
		h_leg->Draw();
	}

	C1->Print((out_dir + run_name + ".pdf").c_str());

	return 0;
}