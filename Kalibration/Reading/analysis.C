//root
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TString.h>
#include <TSpectrum.h>
#include <TPolyMarker.h>
#include <TGraphErrors.h>

//C, C++
#include <math.h>
#include <vector>
#include <assert.h>
#include <stdio.h>

//specific
#include "analysis.h"

using namespace std;

extern float SP;

/******** FUNCTIONS ********/
string checkFilename(TString filename){
  std::string first ("AB"), second ("CD"), str(filename);
  std::string delimeter("_");

  size_t position = str.rfind(delimeter);
  string ending = str.substr(position+1,2);
  if (ending == second){
    return "WC-Version:1.7";
  }
  else{
    return "WC-Version:1.14";
  }
}

float CDF(TH1F* hWave,float thr){
  float peak=hWave->GetMaximum();
  int timePos=1;
  float val = 0;
  while(abs(val)<thr*peak){
    timePos+=1;
    val = hWave->GetBinContent(timePos);
  }
  
  double x1 = SP*(timePos-1);
  double x2 = SP*(timePos);
  double y1 = hWave->GetBinContent(timePos-1);
  double y2 = hWave->GetBinContent(timePos);
  double k = (x2-x1)/(y2-y1);
  return  x1+k*(thr*peak-y1);
}

float CFD2(TH1F* hWave,float thr){
  hWave->GetXaxis()->SetRange(100./SP,150./SP);
  float peak=hWave->GetMaximum();
  int max_bin = hWave->GetMaximumBin();
  int lower_bin = max_bin - 20./SP;
  int timePos=lower_bin;
  float val = 0;
  hWave->GetXaxis()->SetRange(0,1024);
  while(abs(val)<thr*peak){
    timePos+=1;
    val = hWave->GetBinContent(timePos);
  }

  double x1 = SP*(timePos-1);
  double x2 = SP*(timePos);
  double y1 = hWave->GetBinContent(timePos-1);
  double y2 = hWave->GetBinContent(timePos);
  double k = (x2-x1)/(y2-y1);
  return  x1+k*(thr*peak-y1);
}

float CDFinvert(TH1F* hWave,float thr){
  float peak=hWave->GetMaximum();
  int timePos=hWave->GetMaximumBin();
  float val = peak;
  while(val>thr*peak){
    val = hWave->GetBinContent(timePos);
    timePos-=1;
  }
  
  double x1 = SP*(timePos);
  double x2 = SP*(timePos+1);
  double y1 = hWave->GetBinContent(timePos);
  double y2 = hWave->GetBinContent(timePos+1);
  double k = (x2-x1)/(y2-y1);
  return  x1+k*(thr*peak-y1);  
}

float CFDinvert2(TH1F* hWave,float thr){
  hWave->GetXaxis()->SetRange(100./SP,150./SP);
  float peak=hWave->GetMaximum();
  int max_bin = hWave->GetMaximumBin();
  int lower_bin = max_bin - 20./SP;
  int timePos=hWave->GetMaximumBin();
  float val = peak;
  hWave->GetXaxis()->SetRange(0,1024);
  while(val>thr*peak && timePos>lower_bin){
    val = hWave->GetBinContent(timePos);
    timePos-=1;
  }

  double x1 = SP*(timePos);
  double x2 = SP*(timePos+1);
  double y1 = hWave->GetBinContent(timePos);
  double y2 = hWave->GetBinContent(timePos+1);
  double k = (x2-x1)/(y2-y1);
  return  x1+k*(thr*peak-y1);
}

float Integrate_50ns(TH1F* hWave, float BL){
  /*
  Function to integrate the given histogram for 50ns. The range is defined with respect to the position of the maximum in the intervall from 100 to 150ns.
  Procedure:
    - Set the range of the histogram to 100-150ns
    - Find the bin-number of the maximum in that range
    - Determine the bins 20ns before maximum and 30ns after maximum
    - Reset the histogram to full range
    - Assert that the bins correspond to 50ns range
    - Integrate the 50ns subtracting the baseline given as input

  Input: TH1F pointer to histogram, BL as float
  Output: Integral in mV*ns as float
  */

  hWave->GetXaxis()->SetRange(100.0/SP,150.0/SP); //set range to 100-150ns
  int max_bin = hWave->GetMaximumBin(); //bin-number corresponding to maximum in range 100-150ns
  int lower_bin = max_bin - 20.0/SP; //lower integration-limit set to 20ns before maximum
  int upper_bin = max_bin + 30.0/SP; //upper integration-limit set to 30ns before maximum
  hWave->GetXaxis()->SetRange(0,1024); //back to full range

  //consistency-check for 50ns integration range
  float lower_time = hWave->GetXaxis()->GetBinCenter(lower_bin);
  float upper_time = hWave->GetXaxis()->GetBinCenter(upper_bin);
  assert((upper_time-lower_time)==50.0);

  //return 50ns integral with subtracted baseline 
  return hWave->Integral(lower_bin, upper_bin, "width") - BL*50.0;
}

float integral(TH1F* hWave,float t1,float t2,float BL){
  float BW = hWave->GetXaxis()->GetBinWidth(1);
  int bin1 = hWave->FindBin(t1);
  int bin2 = hWave->FindBin(t2);
  float c1 = hWave->GetBinContent(bin1);
  float c2 = hWave->GetBinContent(bin2);
  return hWave->Integral(bin1,bin2,"width")-BL*(t2-t1)-c1*(t1-hWave->GetXaxis()->GetBinLowEdge(bin1))-c2*(hWave->GetXaxis()->GetBinUpEdge(bin2)-t2);
}

float* getBL(TH1F* hWave, float* BL, float t1, float t2){
  /*
  Function to calculate the baseline of the given TH1F-Object.
  Input: TH1F-Object to calculate baseline for; bool isNegative; float-array BL for the output
  Output: baseline and rms of baseline written to 1st and 2nd component of BL-array
  The baseline is calculated as the mean of the values in range of (t1,t2) of the TH1F-Object. The rms
  value is also calculated from the same values.
  
  The float-array BL that is used for the output must be declared before the function call using 'float BL[2];'.
  This is to insure that the output is stored on the heap and not deleted when the memory on the stack is freed up.
   
  Dependencies: function uses C++ vector-class and needs the TMath-header
  */
  
  vector<float> amp;
  for (int i = int(t1/SP); i < int(t2/SP); i++){
    
    amp.push_back(hWave->GetBinContent(i+1));
  }
  BL[0] = TMath::Mean(amp.begin(), amp.end());
  BL[1] = TMath::RMS(amp.begin(), amp.end());
  return BL;
}
/* 
__ Baseline Fit______________________________________________________________
Uses fit of a constant to a given range of the event histogram to calculate baseline.
Other then that same as "getBL" only that float-array holds baseline value, error and chi2/ndf.
*/
float* BL_fit(TH1F* hWave, float* BL_chi2, float t1, float t2){

  // TF1 *f_const = new TF1("f_const","[0]",t1,t2);
  // hWave->Fit("f_const","RN");
  TF1 *f_const = new TF1("f_const","pol0",t1,t2);
  hWave->Fit("f_const","RNWQ");

  BL_chi2[0] = f_const->GetParameter(0);
  BL_chi2[1] = f_const->GetParError(0);
  BL_chi2[2] = f_const->GetChisquare()/f_const->GetNDF();
  BL_chi2[3] = f_const->GetProb();

  return BL_chi2;
}

/*
__ Get Amplitude ________________________________________
Returns amplitude value of maximum in 100-150 ns window
using a constant fit over a 0.5 ns range around the maximum.
Value is basline-corrected and converted to units of p.e. 
*/
float PE(TH1F* hWave, float calib_factor, float BL, float t1, float t2){
  TF1* f1 = new TF1("f1","pol0",100,300);
  double r1=0;
  double r2=0;

  hWave->GetXaxis()->SetRange(t1/SP,t2/SP); //window from 100ns-150ns
  r1=hWave->GetMaximumBin()*SP-0.5;
  r2=r1+1;

  hWave->Fit("f1","QN","",r1,r2);
  float pe = (f1->GetParameter(0) - BL) / calib_factor;
  hWave->GetXaxis()->SetRange(1,1024);

  return pe;
}


/*
__ Get Maximum in Range _________________________________
Returns amplitude value of maximum in given range t1-t2
using a constant fit over a 0.5 ns range around the maximum.
*/
float max_inRange(TH1F* hWave, float t1, float t2){
  TF1* f1 = new TF1("f1","pol0",100,300);
  double r1=0;
  double r2=0;

  hWave->GetXaxis()->SetRange(t1/SP,t2/SP); //window
  r1=hWave->GetMaximumBin()*SP-0.5;
  r2=r1+1;

  hWave->Fit("f1","QN","",r1,r2);
  float max = f1->GetParameter(0);
  hWave->GetXaxis()->SetRange(1,1024);

  return max;
}

/*
__ Get Time at Maximum in Range _________________________________
Returns time value of maximum amplitude in given range t1-t2
*/
float t_max_inRange(TH1F* hWave,float t1, float t2){

  hWave->GetXaxis()->SetRange(t1/SP,t2/SP);
  float t_max = hWave->GetXaxis()->GetBinCenter( hWave->GetMaximumBin() );
  hWave->GetXaxis()->SetRange(1,1024);
  return t_max;
}

/*
__ Get Maximum at Point in Time _________________________________
Returns amplitude value of given point in time
using a constant fit over a 0.5 ns range around that time.
*/
float amp_atTime(TH1F* hWave,float t_max){
  
  TF1* f1 = new TF1("f1","pol0",100,300);
  hWave->Fit("f1","QN","",t_max-0.5,t_max+0.5);
  float max = f1->GetParameter(0);

  return max;
}

/*
__ Convert mV to npe _______________________________________________
Value is basline-corrected and converted to units of p.e. 
*/
double amp2pe_u_l(double y, float calib_factor, float BL_upper, float BL_lower, float BL_Chi2_upper, float BL_Chi2_lower)
{
  if (BL_Chi2_upper <= BL_Chi2_lower)
      {y = (y-BL_upper) / calib_factor;}
  else{y = (y-BL_lower) / calib_factor;}

  return y;
}

double amp2pe(double y, float calib_factor, float BL_used)
{
  y = (y-BL_used) / calib_factor;
  return y;
}

double correction_function(double x){
  return (-142.761 + 0.976471* x ) - (-6498.75 + 2.76626*x - 
    0.000141752*TMath::Power(x,2) + 
    4.03526*TMath::Power(10,-9)*TMath::Power(x,3) - 
    6.92814*TMath::Power(10,-14)*TMath::Power(x,4) + 
    7.54846*TMath::Power(10,-19)*TMath::Power(x,5) - 
    5.33119*TMath::Power(10,-24)*TMath::Power(x,6) + 
    2.42945*TMath::Power(10,-29)*TMath::Power(x,7) - 
    6.88509*TMath::Power(10,-35)*TMath::Power(x,8) +
    1.10249*TMath::Power(10,-40)*TMath::Power(x,9) -
    7.61427*TMath::Power(10,-47)*TMath::Power(x,10));
}

/*
__ Peakfinder ________________________________________________________________
Find up to nPeaks peaks in the waveform hWave. Uses TSpectrum class.
X,Y coordinates are stored in X/Yarray. These arrays should have length [nPeaks].
Peakfinder is applied inside range t1<->t2 in ns.
pfMarker: stores grafics info to print markers to show positions of found peaks
pfON: switch on/off peakfinder algorithm 
Peakfinder parameters:
  sigma: set corresponding to width of searched peaks
  thr: all peaks that are below thr*(maximum amplitude of wf) will be ignored
  nPeaks: maximum number of peaks that will be stored
*/
void peakfinder(TH1F *hWave, float t1, float t2, int nPeaks, int sigma, double thr, double *Xarray, double *Yarray, TPolyMarker *pfMarker, bool pfON)
{
  if (pfON)
  { 
    hWave->GetXaxis()->SetRange(t1/SP,t2/SP);

    // search peaks
    // gErrorIgnoreLevel = kError; // suppress root terminal output 
    TSpectrum *s = new TSpectrum(nPeaks);
    Int_t nfound = s->Search(hWave,sigma,"nodraw", thr);

    // retrieve polymarker showing peak position to draw in pdf
    TList *functions = hWave->GetListOfFunctions();
    TPolyMarker *pm;
    pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
    pm->Copy(*pfMarker);  

    Double_t *xpos = s->GetPositionX();
    Double_t *ypos = s->GetPositionY();

    for (int k = 0; k < nPeaks; ++k)
      { 
        // store coordinate if peak is found
        if (k < nfound) {Xarray[k]=xpos[k]; Yarray[k]=ypos[k];}
        else {Xarray[k]=-999; Yarray[k]=-999;}
      }
  }
  // if switched off set to zero
  else{ for (int k = 0; k < nPeaks; ++k){ Xarray[k]=-999; Yarray[k]=-999; } }

  hWave->GetXaxis()->SetRange(1,1024);
}

float estimateNL(TH1F *hWave, float t){
  hWave->GetXaxis()->SetRange(1,t/SP);
  TGraphErrors *g = new TGraphErrors(hWave);
  float RMS = g->GetRMS(2);
  delete g;
  hWave->GetXaxis()->SetRange(1,1024);
  return RMS;
}