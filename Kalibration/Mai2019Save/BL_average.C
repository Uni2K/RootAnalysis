//Start ohne Compile
//root -l pfad_zu_BL_average.C
//Beide Arrays updaten
//BaselineDatei in read.C eintragen


//root
#include <TH1F.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TString.h>
#include <TGaxis.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TFile.h>
#include <TTree.h>


//C, C++
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h> 

using namespace std;
struct stat st = {0};
void BL_average()
{

	/***** 
	INITIALIZE 
	*****/
	//ROOT STYLES POINTER
	gStyle->SetErrorX(0);
	gStyle->SetOptStat(1110);
	gStyle->SetOptFit(1);
	gStyle->SetGridStyle(3);
	gStyle->SetGridWidth(1);
	gStyle->SetGridColor(16);
	// gStyle->SetImageScaling(30.);
	gStyle->SetLineScalePS(1);
  	
  	
  	// TFile* file0 = new TFile("../a_root_files/a_I3_HV6000_p331_allChannels_allSum.root");
  	// TFile* file0 = new TFile("../a_root_files/27_lOff_8SiPM1_pe_v6.root");
	// TFile* file0 = new TFile("../a_root_files/28_lOff_8SiPM2_pe_v6.root");
	// TFile* file0 = new TFile("../a_root_files/32_8SiPM_17_DarkCount_lOff_HV60_TestbeamConfig_100119_v5.root");
	// TFile* file0 = new TFile("../a_root_files/38_40SiPM_1_sw4_Calib_lOff_Int554_HV60_TestbeamConfig_240119_v3.root");
	// TFile* file0 = new TFile("../a_root_files/39_40SiPM_1_sw2_Calib_lOff_Int574_HV60_TestbeamConfig_240119.root");
	// TFile* file0 = new TFile("../a_root_files/41_40SiPM_1_sw2_Calib_lOff_Int590_HV60_TestbeamConfig_240119.root");
	// TFile* file0 = new TFile("../a_root_files/42_40SiPM_1_sw1_Calib_lOff_Int600_HV60_TestbeamConfig_240119.root");
	// TFile* file0 = new TFile("../a_root_files/43_40SiPM_1_sw1_DarCount_lOff_HV60_TestbeamConfig_240119_scale0_ampW50_dynBL_contGfit_gainW20.root");
	// TFile* file0 = new TFile("../a_root_files/44_40SiPM_2_sw4_Calib_lOff_Int554_HV60_TestbeamConfig_240119_v3.root");
	// TFile* file0 = new TFile("../a_root_files/47_40SiPM_2_sw2_Calib_lOff_Int574_HV60_TestbeamConfig_040219.root");
	// TFile* file0 = new TFile("../a_root_files/50_40SiPM_2_sw1_Calib_lOff_Int600_HV60_TestbeamConfig_040219.root");
	// TFile* file0 = new TFile("../a_root_files/51_8SiPM_1_Calib_lOff_Int536_HV60_TestbeamConfig_040219.root");
	// TFile* file0 = new TFile("../a_root_files/52_8SiPM_2_Calib_lOff_Int536_HV60_TestbeamConfig_040219.root");
	// TFile* file0 = new TFile("../a_root_files/53_8SiPM_17_Calib_lOff_Int530_HV60_TestbeamConfig_040219_scale0_ampW50_dynBL_noGfit_gainW20.root");
	// TFile* file0 = new TFile("../a_root_files/54_8SiPM_17_Calib_lOff_Int530_HV57_TestbeamConfig_040219_scale0_ampW50_dynBL_noGfit_gainW20.root");
	// TFile* file0 = new TFile("../a_root_files/55_8SiPM_17_Calib_lOff_Int530_HV58_TestbeamConfig_040219_scale0_ampW50_dynBL_noGfit_gainW20.root");
	// TFile* file0 = new TFile("../a_root_files/56_8SiPM_17_Calib_lOff_Int530_HV59_TestbeamConfig_040219_scale0_ampW50_dynBL_noGfit_gainW20.root");
	// TFile* file0 = new TFile("../a_root_files/57_8SiPM_17_Calib_lOff_Int530_HV56_TestbeamConfig_040219_scale0_ampW50_dynBL_noGfit_gainW20.root");
	// TString f = 63_8SiPM_17_Calib_lOff_Int530_HV61_TestbeamConfig_040219_scale0_ampW50_dynBL_noGfit_gainW20.root;

	TString dir = "./rootfiles/";
	TString rootfiles[] = {"calib_vb56","calib_vb57","calib_vb58","calib_vb591","calib_vb60","calib_vb61","calib_vb62"};
		string rootfiles_str[] = {"calib_vb56","calib_vb57","calib_vb58","calib_vb591","calib_vb60","calib_vb61","calib_vb62"};

	// TString dir = "../a_root_files/";

	// __ DARK COUNT _____________

	// TString f = "35_40SiPM_1_sw4_DarkCount_lOff_HV60_TestbeamConfig_100119";
	// TString f = "36_40SiPM_2_sw4_DarkCount_lOff_HV60_TestbeamConfig_100119";
	// TString f = "40_40SiPM_1_sw2_DarCount_lOff_HV60_TestbeamConfig_240119";
	// TString f = "43_40SiPM_1_sw1_DarCount_lOff_HV60_TestbeamConfig_240119";
	// TString f = "48_40SiPM_2_sw2_DarkCount_lOff_HV60_TestbeamConfig_040219";

	// TString f = "66_8SiPM_0_DarkCount_lOff_HV60_TestbeamConfig_210319";
	// TString f = "67_8SiPM_1_DarkCount_lOff_HV60_TestbeamConfig_210319";
	// TString f = "70_8SiPM_2_DarkCount_lOff_HV60_TestbeamConfig_210319";

	// TString f = "74_40SiPM_0_DarkCount_lOff_HV60_TestbeamConfig_210319";
	// TString f = "78_40SiPM_0_sw2_DarkCount_lOff_HV60_TestbeamConfig_210319";
	// TString f = "80_40SiPM_0_sw1_DarkCount_lOff_HV60_TestbeamConfig_210319";
	// TString f = "83_8SiPM_17_DarkCount_lOff_HV60_TestbeamConfig_210319";
	// TString f = "86_40SiPM_1_DarkCount_lOff_HV60_TestbeamConfig_210319";
	// TString f = "86_40SiPM_2_sw4_DarkCount_lOff_HV60_TestbeamConfig_210319";



int array_size = sizeof(rootfiles)/sizeof(TString);


		for(int i = 0; i < array_size; i++)
{
TString f=rootfiles[i];



	TFile* file0 = new TFile(dir+f+".root");
	

	TTree* tree_TB17;
	file0->GetObject("T",tree_TB17);

	int Xmin = 0;
	int Xmax = 5;
	int binX = 80;
	float bin_w = (float)Xmax/binX;

	float alpha = 0.1;
	int lineW = 2;

	/***** 
	AVERAGE BASELINE 
	*****/
	//16 Canvas
	TCanvas * C_vec[16];
	//16 Histogramme 
	TH1F * h_vec[16];
	//Linien
	TLine * ln_vec[8];
	//1D Funktionen für 8 Channel 0,1,2,3,4,5,6,7
	TF1 * fit_BL_vec[8];
	float chi2_min[8], mean_BL[8];

	TCanvas * C1 = new TCanvas("BL_Chi2_Dist","",1000,1000);
	TCanvas * C2 = new TCanvas("BL_Value_Dist","",1000,1000);

	C1->Divide(3,3);
	C2->Divide(3,3);

	for (int i = 0; i < 8; ++i)
	{	//0,1,2,3,4,5,6,7

		TString c_name1, c_name2, h_name1, h_name2, draw_cmnd1, draw_cmnd2, cut_cmnd2;
		TString h_title1, h_title2;
		TString h_xtile1, h_xtile2, h_ytitle1, h_ytitle2;
		TString g_fit;
		TString xTitle2;
		
		/***** 
		__ SHOW BL_CHI2 ______________________________
		*****/

		C1->cd(i+1);
		gPad->SetLogy(); gPad->SetGridx(); gPad->SetGridy();

		c_name1.Form("c%d",i);
		h_name1.Form("h%d",i);
		h_title1.Form("ch%d",i);
		//BL_CHI2_used ist ein Branch im Treefile, siehe ROOT Browser, >> Bit shift
		draw_cmnd1.Form("BL_Chi2_used[%d]>>h%d",i,i);
		
		h_vec[i] = new TH1F(h_name1,h_title1,binX,Xmin,Xmax);
		h_vec[i]->SetLineColor(kBlue); h_vec[i]->SetLineWidth(lineW); h_vec[i]->SetFillColorAlpha(kBlue,alpha);
		h_vec[i]->GetXaxis()->SetTitle("Baseline Fit Chi2 Distribution");
		h_vec[i]->GetYaxis()->SetTitle("#Entries");
		//Commandos ausführen
		tree_TB17->Draw(draw_cmnd1);

		/***** 
		__ MINIMUM BL_CHI2 in range_________________
		//Hier Range einstellen. Ablesen bei Plot-> roter Strich, wenn komplett off, kalibrieren.
		*****/

		h_vec[i]->GetXaxis()->SetRange( 1./bin_w,1.8/bin_w);
		chi2_min[i] = h_vec[i]->GetXaxis()->GetBinCenter(h_vec[i]->GetMinimumBin());
		printf("chi2_min_1: %f\n",chi2_min[i]);
		h_vec[i]->GetXaxis()->SetRange(0,5/bin_w);

		ln_vec[i] = new TLine(chi2_min[i],0.1,chi2_min[i],2000);
		ln_vec[i]->SetLineColor(2);
		ln_vec[i]->SetLineWidth(lineW);
		ln_vec[i]->Draw("same");

		/***** 
		__ SHOW BL ___________________________________
		*****/
		//Anderen Diagramme, hier jetzt höhere Zahlen, weil man nur ein Array mit Größe 16 hat und die ersten 8 schon voll sind.
		c_name2.Form("c%d",i+8);
		h_name2.Form("h%d",i+8);
		h_title2.Form("ch%d",i);
		draw_cmnd2.Form("BL_used[%d]>>h%d",i,i+8);
		cut_cmnd2.Form("BL_Chi2_used[%d]<%f",i,chi2_min[i]);
		xTitle2.Form("Baseline (Cut: BL_CHI2<%1.2f) in mV",chi2_min[i]);
		g_fit.Form("fit ch%d",i);

		C2->cd(i+1);
		// gPad->SetLogy(); gPad->SetGridx(); gPad->SetGridy();

		h_vec[i+8] = new TH1F(h_name2,h_title2,binX,-5,5);
		h_vec[i+8]->SetLineColor(kBlue); h_vec[i+8]->SetLineWidth(lineW); h_vec[i+8]->SetFillColorAlpha(kBlue,alpha);
		h_vec[i+8]->GetXaxis()->SetTitle(xTitle2);
		h_vec[i+8]->GetYaxis()->SetTitle("#Entries");
		tree_TB17->Draw(draw_cmnd2,cut_cmnd2,"goff");

		/***** 
		__ FIT AVERAGE BL ___________________________________
		*****/

		// gauss fit around histogram mean
		float h_mean = h_vec[i+8]->GetMean();
		float fit_range = 0.3;

		fit_BL_vec[i] = new TF1(g_fit,"gaus",h_mean-fit_range, h_mean-fit_range);
		h_vec[i+8]->Fit(g_fit,"RQ");

		mean_BL[i] = fit_BL_vec[i]->GetParameter(1);

		h_vec[i+8]->Draw();
		
	} // end loop over channels

	/***** 
	__ PRINT RESULTS ___________________________________
	*****/

	

	string parent_dir="./baseline_histograms/";

  string target_dir = parent_dir+rootfiles_str[i]+"/";

if (stat(parent_dir.c_str(), &st) == -1) {
    mkdir(parent_dir.c_str(), 0700);
}
if (stat(target_dir.c_str(), &st) == -1) {
    mkdir(target_dir.c_str(), 0700);
}

	C1->SaveAs((target_dir+rootfiles_str[i].c_str()+"_Chi2.pdf").c_str());
	C2->SaveAs((target_dir+rootfiles_str[i].c_str()+"_Baseline.pdf").c_str());

  FILE * factor_list;
  factor_list = fopen((target_dir+rootfiles_str[i]+".txt").c_str(),"w");
for (int i = 0; i < 8; ++i)
	{	
		fprintf(factor_list,"Minimum Chi2 in range(0.7,1.5) ch%d = %f\n",i,chi2_min[i] );
	}
	for (int i = 0; i < 8; ++i)
	{	
		fprintf(factor_list,"Mean Baseline ch%d = %f\n",i,mean_BL[i] );
	}
	
	fprintf(factor_list,"Mean Baseline ch1-8 = %f,%f,%f,%f,%f,%f,%f,%f\n",mean_BL[0],mean_BL[1],mean_BL[2],mean_BL[3],mean_BL[4],mean_BL[5],mean_BL[6],mean_BL[7] );

  fclose(factor_list);

  //Summary
 FILE * summary;
 summary = fopen((parent_dir+"Baselines.txt").c_str(),"a");
	


	fprintf(summary,(" vector<float> BL_"+rootfiles_str[i]+" = { %f,%f,%f,%f,%f,%f,%f,%f,0};\n").c_str(),mean_BL[0],mean_BL[1],mean_BL[2],mean_BL[3],mean_BL[4],mean_BL[5],mean_BL[6],mean_BL[7] );

  fclose(summary);



}













}