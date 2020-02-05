//root
#include <TH2D.h>   
#include <TEfficiency.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TString.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TF1.h>
#include <TH2D.h>
#include <TEfficiency.h>
#include <TLegend.h>
#include <THStack.h>
#include <THistPainter.h>

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]){


	///////////////// vectors with runnames and corresponding positions/////////////////////

	// vector<string> runs  = {"30_pion6_pos6_AB","32_pion6_pos5_AB","34_pion6_pos4_AB","36_pion6_pos2_AB","40_pion6_pos7_AB","42_pion6_pos8_AB","44_pion6_pos9_AB","46_pion6_pos1_AB","48_pion6_pos10_AB","50_pion6_pos11_AB","68_pion6_pos13_AB","73_pion6_pos16_AB"};
	// vector<string> runs  = {"30_pion6_pos6_CD","32_pion6_pos5_CD","34_pion6_pos4_CD","36_pion6_pos2_CD","40_pion6_pos7_CD","42_pion6_pos8_CD","44_pion6_pos9_CD","46_pion6_pos1_CD","48_pion6_pos10_CD","50_pion6_pos11_CD","68_pion6_pos13_CD","73_pion6_pos16_CD"};
	// vector<int> pos = {6,5,4,2,7,8,9,1,10,11,13,16};

	// vector<string> runs  = {"77_pion6_pos5_angle30_AB", "78_pion6_pos4_angle30_AB", "81_pion6_pos6_angle30_AB", "82_pion6_pos9_angle30_AB", "83_pion6_pos9_angle30_AB"};
	// vector<string> runs  = {"77_pion6_pos5_angle30_CD", "78_pion6_pos4_angle30_CD", "81_pion6_pos6_angle30_CD", "82_pion6_pos9_angle30_CD", "83_pion6_pos9_angle30_CD"};
	// vector<int> pos = {5,4,6,9,9};	



	// vector<string> runs  = {"29_muon6_pos6_AB", "33_muon6_pos4_AB", "35_muon6_pos2_AB", "37_muon6_pos3_AB", "39_muon6_pos7_AB", "41_muon6_pos8_AB", "43_muon6_pos9_AB", "45_muon6_pos1_AB", "47_muon6_pos10_AB", "49_muon6_pos11_AB", "65_muon6_pos5_AB", "66_muon6_pos12_AB", "67_muon6_pos13_AB", "69_muon6_pos14_AB","71_muon6_pos16_AB"};
	// vector<string> runs  = {"29_muon6_pos6_CD", "33_muon6_pos4_CD", "35_muon6_pos2_CD", "37_muon6_pos3_CD", "39_muon6_pos7_CD", "41_muon6_pos8_CD", "43_muon6_pos9_CD", "45_muon6_pos1_CD", "47_muon6_pos10_CD", "49_muon6_pos11_CD", "65_muon6_pos5_CD", "66_muon6_pos12_CD", "67_muon6_pos13_CD", "69_muon6_pos14_CD","71_muon6_pos16_CD"};
	// vector<int> pos = {6,4,2,3,7,8,9,1,10,11,5,12,13,14,16};


	// vector<string> runs  = {"76_muon6_pos5_angle30_AB", "80_muon6_pos6_angle30_AB"};
	// vector<string> runs  = {"76_muon6_pos5_angle30_CD", "80_muon6_pos6_angle30_CD"};
	// vector<int> pos = {5,6};


	// vector<string> runs  = {"56_e5_pos5_AB", "57_e5_pos7_AB", "58_e5_pos8_AB", "59_e5_pos3_AB", "60_e5_pos2_AB", "61_e5_pos10_AB", "62_e5_pos12_AB", "63_e5_pos9_AB", "64_e5_pos1_AB"};
	vector<string> runs  = {"56_e5_pos5_CD", "57_e5_pos7_CD", "58_e5_pos8_CD", "59_e5_pos3_CD", "60_e5_pos2_CD", "61_e5_pos10_CD", "62_e5_pos12_CD", "63_e5_pos9_CD", "64_e5_pos1_CD"};
	vector<int> pos = {5,7,8,3,2,10,12,9,1};



 	ofstream outputstream;
 	//output text file - change name to appropriate parameters
 	outputstream.open("./efficiency_vs_position_13_WOMC_electrons_int.txt");

	for (int i=0; i<runs.size(); i++){
		TFile* file = new TFile(Form("./../runs/%s/out.root",runs.at(i).c_str()));
		// cout << Form("\n./../runs/%s/out.root",runs.at(i).c_str()) << endl;
		TTree* tree;
		file->GetObject("T",tree);
		TCanvas* c = new TCanvas("c");

		TH1D* hDeNum1 = new TH1D("hDeNum1","",28,0,28);
		TString cutDeNum1("");
		TH1D* hNum1 = new TH1D("hNum1","",28,0,28);
		//Set threshold according to DC measurement results
		TString cutNum1("(PE_WOM1_int)>=13");
		tree->Draw("mp>>hNum1",cutNum1);
		tree->Draw("mp>>hDeNum1",cutDeNum1);
		TEfficiency* eff1;
		eff1=new TEfficiency(*hNum1,*hDeNum1);
		eff1->SetStatisticOption(TEfficiency::kFCP);

		//write to txt file
		for (int j = 1; j <= hDeNum1->GetXaxis()->GetNbins(); j++){
			if (eff1->GetEfficiency(j)>0.0){
				outputstream<<Form("%d \t %f \t %f \t %f", pos.at(i), eff1->GetEfficiency(j), eff1->GetEfficiencyErrorLow(j), eff1->GetEfficiencyErrorUp(j))<<endl;
			}
		}
	}	
	outputstream.close();
	return 0;
}