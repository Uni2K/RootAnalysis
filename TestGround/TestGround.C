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

namespace fs = std::experimental::filesystem;
using namespace std;
struct stat info;

void strip_ext(char *fname)
{
	char *end = fname + strlen(fname);

	while (end > fname && *end != '.')
	{
		--end;
	}

	if (end > fname)
	{
		*end = '\0';
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
void extractIntegerWords(string str)
{
	stringstream ss;

	/* Storing the whole string into string stream */
	ss << str;

	/* Running loop till the end of the stream */
	string temp;
	int found;
	while (!ss.eof())
	{

		/* extracting word by word from stream */
		ss >> temp;

		/* Checking the given word is integer or not */
		if (stringstream(temp) >> found)
			cout << found << " ";

		/* To save from space at the end of string */
		temp = "";
	}
}



void fitConvolution()
{
	TH1F *h_ExpGauss = new TH1F("h_ExpGauss", "Exponential convoluted by gaussian", 100, 50., 10.);
	for (int i = 0; i < 1e6; i++)
	{
		Double_t x = gRandom->Exp(20); //gives a alpha of -0.3 in the exp
		x += gRandom->Gaus(20, 10);
		h_ExpGauss->Fill(x); //probability density function of the addition of two variables is the convolution of 2 dens. functions
	}
	TF1Convolution *f_conv = new TF1Convolution("expo", "gaus", -1, 10, true);
	
	f_conv->SetRange(-5., 6.);
	f_conv->SetNofPointsFFT(1000);
	TF1 *f = new TF1("f", *f_conv, -5., 5., f_conv->GetNpar());
	f->SetParameters(1., -0.3, 0., 1.);
	TCanvas *c1=new TCanvas("c", "c", 800, 1000);
	h_ExpGauss->Fit("f");
	h_ExpGauss->Draw();
	c1->Print("test.pdf");

}

/*******************
__ FUNCTIONS ______
*******************/

int main(int argc, char *argv[])
{
	{
		// Generation of an exponential distribution

		fitConvolution();
		gROOT->Reset();
		gROOT->SetStyle("Plain");
		TCanvas *c1;
		TCanvas *c2;
		TH1F *distribution1;
		TH1F *distribution2;
		TH1F *distribution3;
		TH1F *distribution4;
		TF1 *exp1;

		c1 = new TCanvas("c1", "Exponential distribution", 200, 10, 600, 800);
		//   c1->Divide();
		c2 = new TCanvas("c2", "Exponential distribution", 200, 10, 600, 800);
		c2->Divide(1, 3);

		// Create some histograms.
		distribution1 = new TH1F("Distribution", "Distribution", 100, 0, 1);
		distribution2 = new TH1F("Distribution", "Distribution", 100, 0, 1);
		Int_t m = 100;
		Double_t a = -15;
		Double_t b = 15;
		distribution3 = new TH1F("Exp-Distribution", "Exp-Distribution", m, a, b);
		distribution4 = new TH1F("Exp-Distribution1", "Exp-Distribution2", m, a, b);

		// Use two pseudo-random number generator
		TRandom *random1 = new TRandom;
		TRandom *random2 = new TRandom;
		TRandom *random3 = new TRandom;
		// Random seeds for the random generators are set
		// These are the starting values for the algorithm
		// producing the pseudo-random numbers
		// seed=0: actual computer clock time in seconds
		random1->SetSeed(0);
		random2->SetSeed(5000);
		random3->SetSeed(1);
		// number of generated numbers
		int n = 50000;

		Double_t expo1 = 0;
		Double_t x1 = 0;
		Double_t x2 = 0;
		Double_t lambda = 0.75;
		Double_t Pi = 3.1415;
		Double_t sigma = 1.0;

		Double_t max = lambda * exp(0);
		//  cout << "max: " << max  << endl;

		// Weighting factor to obtain a normalized distribution
		int success = 0;

		Double_t data1;
		Double_t data2;
		Double_t data3;

		Double_t par[2];
		exp1 = new TF1("exp1", "expo", 8, 10);

		for (Int_t i = 0; i < n; i++)
		{
			// Produce n-times uniform random numbers data1 and data2
			data1 = random1->Rndm();
			data2 = random2->Rndm();
			data3 = random3->Rndm();

			x1 = sqrt(-2 * log(data2)) * cos(2 * Pi * data3);
			expo1 = (-1 / (lambda)) * log(data1);
			distribution1->Fill(data1);
			distribution3->Fill(expo1);
			expo1 = expo1 + x1 * sigma;
			distribution4->Fill(expo1);
		}

		c2->cd(1);
		distribution3->Draw();
		c2->cd(2);
		distribution4->Draw("e1p");

		// Plot next figures into canvas c2
		TPad *pad1 = new TPad("pad1", "pad", 0.00, 0.00, 1.00, 1.00);
		c2->cd(3);
		pad1->Draw();
		pad1->SetLogy(1);
		pad1->cd();
		distribution4->Fit("exp1", "R");
		distribution4->Draw("e1p");
		c2->Update();

		// Produce a eps-file
		c2->Print("exponentialgaussian.gif", "gif");
	}

	return 0;
}