//root
#include <TLine.h>
#include <TString.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TMath.h>
#include <TF1.h>
#include <TStyle.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TEfficiency.h>
#include <TLegend.h>
#include <THStack.h>
#include <THistPainter.h>
#include <TText.h>
#include <TSpectrum.h>   // peakfinder
#include <TPolyMarker.h> // peakfinder
#include <TError.h>      // root verbosity level
//#include <TStyle.h>

//C, C++
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <sstream>
//#include <stdlib.h>
//#include <string>
//#include <iomanip>

//specific
#include "geometry.h"
#include "analysis.h"
#include "readFull.h"
#define btoa(x) ((x) ? "true" : "false")

float SP = 0.3125;

vector<float> calib_dummy(16, 1);
vector<float> calib_amp;
vector<float> calib_charge;

vector<float> BL_dummy(9, 0);
vector<float> BL_const;

//Skip events with bad baseline
bool allowEventSkipping = false;
int skipInChannel = 0;

// SWITCH dynamic <-> constant baseline
bool switch_BL = false; // true = dyn, false = const
//Integration Window
bool isDC = true;
//IF the calibration values are correct, otherwise use dummies
bool isCalib = true;

float integralStart = 228;
float integralEnd = 258;

int amp_array_printRate = 5000; //Changed down below
int wavesPrintRate = 5000;
// clibrated sum

int ch0PrintRate = 5000;
int trigPrintRate = 5000;   //100
int signalPrintRate = 5000; //100
double coef = 2.5 / (4096 * 10);
string WCHU("AB"), WCAlexander("CD");

//External Variables - mostly definded in main.C
extern string WCVersion;
extern int runNr;
extern float horizontal;
extern float vertical;
extern float angle;
extern int pdgID;
extern float energy;
extern int isSP;
extern int mp;
extern int safPMT2;
extern int safPMT1;
extern int safSiPM;
extern int trackL;

char *extractValues(char *s)
{
  //printf("input: %s\n", s);
  char *start = s;
  char *end = s;
  while (*s)
  {
    if (*s == '{')
      start = s;
    else if (*s == '}')
      end = s;
    if (start < end && *start)
    {
      *end = 0;
      //  printf("token: %s\n", start+1);
      return start + 1;
      start = s = end;
    }
    s++;
  }
}

double stringToDouble(char *text)
{
  char *unconverted;
  return strtod(text, &unconverted);
}

void readFull(TString _inFileList, TString _inDataFolder, TString _outFile, string _runName, string dynamicBL_, string isDC_, string useCalibValues_)
{
  //float integralStart =145;
  //float integralEnd = 175;
  if (dynamicBL_ == "0")
  {
    switch_BL = false;
  }
  else
  {
    switch_BL = true;
  }
  if (isDC_ == "0")
  {
    isDC = false;
  }
  else
  {
    isDC = true;
  }
  if (useCalibValues_ == "0")
  {
    isCalib = false;
  }
  else
  {
    isCalib = true;
  }
  cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::" << endl;
  cout << "DYNAMIC BASELINE: " << switch_BL << endl;
  cout << "isDC: " << isDC << endl;
  cout << "use Calib Values: " << isCalib << endl;
  cout << "runName: " << _runName << endl;
  cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::" << endl;

  /*
  Function that is used by main(). read() does most of the analysis of the data. The raw .bin files 
  are read, converted into root-histograms and then the analysis calls are done. Most of the longer
  functions are defined in alanysis.C.
  The read() function the saves all the events (event by event) of that particular run to a root tree
  which is the saved in /runs/runName/out.root.
  */
  TF1 *f_const;
  vector<float> calib_amp = calib_dummy;
  vector<float> calib_charge = calib_dummy;
  string workingDir;

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
  {
    printf("Current working dir: %s\n", cwd);
    workingDir = cwd;
  }
  else
  {
    perror("getcwd() error");
  }

  //READ CALIB AMP----------------------------------------------------------------------------
  if (isCalib)
  {
    string calib_file = "/src/calibration_amp.txt";
    string calib_path = workingDir + calib_file;
    FILE *file = fopen(calib_path.c_str(), "r");

    char line[256];

    while (fgets(line, sizeof(line), file))
    {
      if ((strstr(line, _runName.c_str()) != NULL))
      {
        calib_amp.clear();
        char *array[128];
        int i = 0;
        array[i] = strtok(extractValues(line), ",");
        calib_amp.push_back(stringToDouble(array[0]));

        while (array[i] != NULL)
        {
          i = i + 1;
          array[i] = strtok(NULL, ",");
          calib_amp.push_back(stringToDouble(array[i]));
          if (i == 8)
            break; //not 100% correct splitting so abort at 8 values otherwise segment violation
        }
        break;
      }
    }
    fclose(file);
  }
  //-------------------------------------------------------------------------------------------------

  //READ CALIB CHARGE----------------------------------------------------------------------------
  if (isCalib)
  {
    string calib_file = "/src/calibration_charge.txt";
    string calib_path = workingDir + calib_file;
    FILE *file = fopen(calib_path.c_str(), "r");

    char line[256];

    while (fgets(line, sizeof(line), file))
    {
      if ((strstr(line, _runName.c_str()) != NULL))
      {
        calib_charge.clear();
        char *array[128];
        int i = 0;
        array[i] = strtok(extractValues(line), ",");
        calib_charge.push_back(stringToDouble(array[0]));

        while (array[i] != NULL)
        {
          i = i + 1;
          array[i] = strtok(NULL, ",");
          calib_charge.push_back(stringToDouble(array[i]));
          if (i == 8)
            break; //not 100% correct splitting so abort at 8 values otherwise segment violation
        }
        break;
      }
    }
    fclose(file);
  }
  //-------------------------------------------------------------------------------------------------

  //READ BASELINE----------------------------------------------------------------------------
  if (!switch_BL)
  {
    string calib_file = "/src/Baselines.txt";
    string calib_path = workingDir + calib_file;
    FILE *file = fopen(calib_path.c_str(), "r");

    char line[256];

    while (fgets(line, sizeof(line), file))
    {
      if ((strstr(line, _runName.c_str()) != NULL))
      {
        BL_const.clear();
        char *array[128];
        int i = 0;
        array[i] = strtok(extractValues(line), ",");
        BL_const.push_back(stringToDouble(array[0]));

        while (array[i] != NULL)
        {
          i = i + 1;
          array[i] = strtok(NULL, ",");
          BL_const.push_back(stringToDouble(array[i]));
          if (i == 8)
            break; //not 100% correct splitting so abort at 8 values otherwise segment violation
        }
        break;
      }
    }
    fclose(file);
  }
  else
  {
    BL_const = BL_dummy;
  }
  //-------------------------------------------------------------------------------------------------

  if (!isCalib)
  {
    calib_amp = calib_dummy;
    calib_charge = calib_dummy;
  }
  //Aktivieren, falls keine Baselines gemessen, z.B. für das erstmalige Auslesen der ROOT Daten
  // BL_const=BL_dummy;

  if (isDC)
  {
    amp_array_printRate = 5000;
    wavesPrintRate = 5000;
  }
  else
  {
    amp_array_printRate = 5000;
    wavesPrintRate = 5000;
  }

  printf(("USED BL ARRAY: %f,%f,%f,%f,%f,%f,%f,%f FOR: " + _runName + "\n").c_str(), BL_const[0], BL_const[1], BL_const[2], BL_const[3], BL_const[4], BL_const[5], BL_const[6], BL_const[7], BL_const[8]);
  printf(("USED AMP CALIB ARRAY: %f,%f,%f,%f,%f,%f,%f,%f FOR: " + _runName + "\n").c_str(), calib_amp[0], calib_amp[1], calib_amp[2], calib_amp[3], calib_amp[4], calib_amp[5], calib_amp[6], calib_amp[7], calib_amp[8]);
  printf(("USED CHARGE CALIB ARRAY: %f,%f,%f,%f,%f,%f,%f,%f FOR: " + _runName + "\n").c_str(), calib_charge[0], calib_charge[1], calib_charge[2], calib_charge[3], calib_charge[4], calib_charge[5], calib_charge[6], calib_charge[7], calib_charge[8]);
  printf("IS DARKCOUNT: %s | Dynamic Baseline: %s | Is Calibrated: %s ", btoa(isDC), btoa(switch_BL), btoa(isCalib));

  /*Create root-file and root-tree for data*/
  TFile *rootFile = new TFile(_outFile, "RECREATE");
  if (rootFile->IsZombie())
  {
    cout << "PROBLEM with the initialization of the output ROOT ntuple "
         << _outFile << ": check that the path is correct!!!"
         << endl;
    exit(-1);
  }
  TTree *tree = new TTree("T", "USBWC Data Tree");

  tree->SetAutoSave(0LL);
  tree->SetAutoFlush(0LL);

  TTree::SetBranchStyle(0);

  gStyle->SetLineScalePS(1); // high resolution plots

  /*Declare & define the variables that are to be saved in the root-tree or that are used during the analysis.*/
  Int_t EventNumber = -999;
  Int_t LastEventNumber = -999;
  Float_t SamplingPeriod = -999;
  Double_t EpochTime = -999;
  Int_t Year = -999;
  Int_t Month = -999;
  Int_t Day = -999;
  Int_t Hour = -999;
  Int_t Minute = -999;
  Int_t Second = -999;
  Int_t Millisecond = -999;
  Float_t trigT = -999; //t_trig = (t0+t1+t2+t3)/4
  Float_t tPMT1 = -999;
  Float_t tPMT2 = -999;
  Float_t tPMT2i = -999;
  Float_t tSUMp = -999;
  Float_t tSUMm = -999;
  Float_t trigTp = -999; //t_trig' = [(t0+t1)-(t2+t3)]/4
  Float_t t0t1 = -999;   //t0t1 = [(t0-t1)]
  Float_t t2t3 = -999;   //t2t3 = [(t2-t3)]
  Int_t isVeto = -999;   //variable to define veto, 1 if veto, 0 if not, -999 if undefined
  Int_t isTrig = -999;

  Int_t isLastEvt = -999;
  Int_t isGoodSignal_5 = -999;
  Float_t trigGate = -999;
  Int_t nCh = -1;
  int nActiveCh = -1;
  Int_t ChannelNr[16];
  Int_t WOMID[16]; //1=A, 2=B, 3=C, 4=D

  float amp_array;       // calibrated amplitude, baseline-shifted sum signal
  float charge_array;    // calibrated charge, baseline-shifted sum signal
  float t_amp_array;     // point in time of sum signal
  float chPE_amp[16];    // single channel amplitude at sum signal
  float chPE_charge[16]; // single channel charge at sum signal

  Float_t t_amp_array_invCFD = -999;
  Float_t t_amp_array_invCFD_wrtTrig = -999;

  std::vector<float> amp(16, -999);
  std::vector<float> amp_inRange(16, -999);
  std::vector<float> max(16, -999);
  std::vector<float> min(16, -999);
  Float_t t[16];
  Float_t tSiPM[16];

  float Integral_0_300[16]; //array used to store Integral of signal from 0 to 300ns
  float Integral_inRange[16];
  float Integral[16];
  float Integral_mVns[16];

  float BL_output[4];        //array used for output getBL-function
  Float_t BL_lower[16];      //store baseline for 16 channels for 0-75ns range
  Float_t BL_RMS_lower[16];  //store rms of baseline for 16 channels for 0-75ns range
  Float_t BL_Chi2_lower[16]; //store chi2/dof of baseline-fit for 16 channels for 0-75ns range
  Float_t BL_pValue_lower[16];
  Float_t BL_upper[16];      //store baseline for 16 channels for 220-320ns range
  Float_t BL_RMS_upper[16];  //store rms of baseline for 16 channels for 220-320ns range
  Float_t BL_Chi2_upper[16]; //store chi2/dof of baseline-fit for 16 channels for 220-320ns range
  Float_t BL_pValue_upper[16];

  Float_t BL_used[16];
  Float_t BL_Chi2_used[16];
  Float_t BL_pValue_used[16];

  int nPeaks = 4; // maximum number of peaks to be stored by peakfinder; has to be set also when creating branch
  Double_t peakX[16][nPeaks];
  Double_t peakY[16][nPeaks];

  int NumberOfBins;
  Int_t EventIDsamIndex[16];
  Int_t FirstCellToPlotsamIndex[16];
  std::vector<TH1F *> hChSum;
  for (int i = 0; i < 16; i++)
  {
    TString name("");
    name.Form("hChSum_%d", i);
    TH1F *h = new TH1F("h", ";ns;Amplitude, mV", 1024, -0.5 * SP, 1023.5 * SP);
    h->SetName(name);
    hChSum.push_back(h);
  }
  std::vector<TH1F *> hChShift;
  for (int i = 0; i < 16; i++)
  {
    TString name("");
    name.Form("hChShift_%d", i);
    TH1F *h = new TH1F("h", ";ns;Amplitude, mV", 1024, -0.5 * SP, 1023.5 * SP);
    h->SetName(name);
    hChShift.push_back(h);
  }
  std::vector<TH1F> hChtemp;
  for (int i = 0; i < 16; i++)
  {
    TString name("");
    name.Form("hChtemp_%d", i);
    TH1F h("h", ";ns;Amplitude, mV", 1024, -0.5 * SP, 1023.5 * SP);
    h.SetName(name);
    hChtemp.push_back(h);
  }
  std::vector<TH1F> hChShift_temp;
  for (int i = 0; i < 16; i++)
  {
    TString name("");
    name.Form("hChShift_temp_%d", i);
    TH1F h("h", ";ns;Amplitude, mV", 1024, -0.5 * SP, 1023.5 * SP);
    h.SetName(name);
    hChShift_temp.push_back(h);
  }
  Short_t amplValues[16][1024];
  TH1F hCh("hCh", "dummy;ns;Amplitude, mV", 1024, -0.5 * SP, 1023.5 * SP);

  TString plotSaveFolder = _outFile;
  plotSaveFolder.ReplaceAll((_runName + ".root").data(), "");

  TCanvas cWaves("cWaves", "cWaves", 1000, 700);
  cWaves.Divide(4, 4);
  TCanvas csumWOMA("csumWOMA", "csumWOMA", 1000, 700);
  csumWOMA.Divide(4, 2);
  TCanvas csumWOMB("csumWOMB", "csumWOMB", 1000, 700);
  csumWOMB.Divide(3, 3);
  TCanvas cCh0("cCh0", "cCh0", 1500, 900);
  cCh0.Divide(2, 2);
  TCanvas cTrig("cTrig", "cTrig", 1500, 900);
  cTrig.Divide(2, 2);
  TCanvas cSignal("cSignal", "cSignal", 1500, 900);
  cSignal.Divide(2, 2);

  // clibrated sum
  TCanvas C_amp_array("C_amp_array", "C_amp_array", 1000, 700);
  C_amp_array.Divide(3, 3);

  /*Create branches in the root-tree for the data.*/
  tree->Branch("EventNumber", &EventNumber, "EventNumber/I");
  tree->Branch("SamplingPeriod", &SamplingPeriod, "SamplingPeriod/F");
  tree->Branch("EpochTime", &EpochTime, "EpochTime/D");
  tree->Branch("Year", &Year, "Year/I");
  tree->Branch("Month", &Month, "Month/I");
  tree->Branch("Day", &Day, "Day/I");
  tree->Branch("Hour", &Hour, "Hour/I");
  tree->Branch("Minute", &Minute, "Minute/I");
  tree->Branch("Second", &Second, "Second_/I");
  tree->Branch("Millisecond", &Millisecond, "Millisecond/I");
  tree->Branch("trigT", &trigT, "trigT/F");
  tree->Branch("tPMT1", &tPMT1, "tPMT1/F");
  tree->Branch("tPMT2", &tPMT2, "tPMT2/F");
  tree->Branch("tPMT2i", &tPMT2i, "tPMT2i/F");
  tree->Branch("tSUMp", &tSUMp, "tSUMp/F");
  tree->Branch("tSUMm", &tSUMm, "tSUMm/F");
  tree->Branch("runNr", &runNr, "runNr/I");      //run number in google table
  tree->Branch("horiz", &horizontal, "horiz/F"); // horizontal position of the box units: [cm]
  tree->Branch("vert", &vertical, "vert/F");     //vertical position of the box, units: [cm]
  tree->Branch("angle", &angle, "angle/F");
  tree->Branch("pdgID", &pdgID, "pdgID/I");
  tree->Branch("energy", &energy, "energy/F");
  tree->Branch("isSP", &isSP, "isSP/I");
  tree->Branch("mp", &mp, "mp/I");
  tree->Branch("safPMT2", &safPMT2, "safPMT2/I"); //solid angle factor
  tree->Branch("safPMT1", &safPMT1, "safPMT1/I"); //solid angle factor
  tree->Branch("safSiPM", &safSiPM, "safSiPM/I"); //solid angle factor
  tree->Branch("trackL", &trackL, "trackL/I");    //track length
  tree->Branch("isLastEvt", &isLastEvt, "isLastEvt/I");
  tree->Branch("trigGate", &trigGate, "trigGate/F");
  tree->Branch("trigTp", &trigTp, "trigTp/F");
  tree->Branch("t0t1", &t0t1, "t0t1/F"); //t0t1 = [(t0-t1)]
  tree->Branch("t2t3", &t2t3, "t2t3/F");
  tree->Branch("isVeto", &isVeto, "isVeto/I");
  tree->Branch("isTrig", &isTrig, "isTrig/I");
  tree->Branch("isGoodSignal_5", &isGoodSignal_5, "isGoodSignal_5/I");

  // CHANNEL INFO (but everything that is nCH-dependend below)
  tree->Branch("nCh", &nCh, "nCh/I");
  tree->Branch("WOMID", WOMID, "WOMID[nCh]/I");
  tree->Branch("ch", ChannelNr, "ch[nCh]/I");
  // AMPLITUDE
  tree->Branch("amp", amp.data(), "amp[nCh]/F");
  tree->Branch("amp_inRange", amp_inRange.data(), "amp_inRange[nCh]/F");
  tree->Branch("max", max.data(), "max[nCh]/F");
  tree->Branch("min", min.data(), "min[nCh]/F");
  // INTEGRAL
  tree->Branch("Integral_0_300", Integral_0_300, "Integral_0_300[nCh]/F");
  tree->Branch("Integral_inRange", Integral_inRange, "Integral_inRange[nCh]/F");
  tree->Branch("Integral", Integral, "Integral[nCh]/F");
  tree->Branch("Integral_mVns", Integral_mVns, "Integral_mVns[nCh]/F");
  // TIMING
  tree->Branch("t", t, "t[nCh]/F");
  tree->Branch("tSiPM", tSiPM, "tSiPM[nCh]/F");
  // BASELINE
  tree->Branch("BL_lower", BL_lower, "BL_lower[nCh]/F");
  tree->Branch("BL_RMS_lower", BL_RMS_lower, "BL_RMS_lower[nCh]/F");
  tree->Branch("BL_Chi2_lower", BL_Chi2_lower, "BL_Chi2_lower[nCh]/F");
  tree->Branch("BL_pValue_lower", BL_pValue_lower, "BL_pValue_lower[nCh]/F");
  tree->Branch("BL_upper", BL_upper, "BL_upper[nCh]/F");
  tree->Branch("BL_RMS_upper", BL_RMS_upper, "BL_RMS_upper[nCh]/F");
  tree->Branch("BL_Chi2_upper", BL_Chi2_upper, "BL_Chi2_upper[nCh]/F");
  tree->Branch("BL_pValue_upper", BL_pValue_upper, "BL_pValue_upper[nCh]/F");
  tree->Branch("BL_used", BL_used, "BL_used[nCh]/F");
  tree->Branch("BL_Chi2_used", BL_Chi2_used, "BL_Chi2_used[nCh]/F");
  tree->Branch("BL_pValue_used", BL_pValue_used, "BL_pValue_used[nCh]/F");
  // PEAKFINDER
  tree->Branch("peakX", peakX, "peakX[nCh][4]/D");
  tree->Branch("peakY", peakY, "peakY[nCh][4]/D");
  // CALIBRATED SUM
  tree->Branch("t_amp_array_invCFD", &t_amp_array_invCFD, "t_amp_array_invCFD/F");
  tree->Branch("t_amp_array_invCFD_wrtTrig", &t_amp_array_invCFD_wrtTrig, "t_amp_array_invCFD_wrtTrig/F");
  tree->Branch("amp_array", &amp_array, "amp_array/F");
  tree->Branch("t_amp_array", &t_amp_array, "t_amp_array/F");
  tree->Branch("charge_array", &charge_array, "charge_array/F");
  tree->Branch("chPE_amp", chPE_amp, "chPE_amp[nCh]/F");
  tree->Branch("chPE_charge", chPE_charge, "chPE_charge[nCh]/F");

  tree->Branch("EventIDsamIndex", EventIDsamIndex, "EventIDsamIndex[nCh]/I");
  tree->Branch("FirstCellToPlotsamIndex", FirstCellToPlotsamIndex, "FirstCellToPlotsamIndex[nCh]/I");

  /*Start reading the raw data from .bin files.*/
  int nitem = 1;
  ifstream inList;
  TString fileName;
  inList.open(_inFileList);
  assert(inList.is_open());

  // clibrated sum
  int amp_array_PrintStatus = -1;

  int wavePrintStatus = -1;
  int sumWOMAPrintStatus = -1;
  int sumWOMBPrintStatus = -1;
  int ch0PrintStatus = -1;
  int trigPrintStatus = -1;
  int signalPrintStatus = -1;

  bool skipThisEvent = false;
  int skippedCount = 0;
  while (inList >> fileName)
  {
    fileName = _inDataFolder + fileName;
    cout << endl;
    cout << fileName << endl;
    FILE *pFILE = fopen(fileName.Data(), "rb");
    if (pFILE == NULL)
    {
      fputs("File error", stderr);
      assert(0);
    }
    fseek(pFILE, 0, SEEK_END);
    int totFileSizeByte = ftell(pFILE);
    rewind(pFILE);
    cout << "totFileSizeByte = " << totFileSizeByte << endl;
    int size_of_header;
    /*During 2018 testbeam measurements two WaveCatchers were used. One from the Berlin group
    and one from Alexander from Geneva. As these two Wavecatchers had two different versions
    there are two types of raw data files that have different header lengths.*/
    if (WCVersion == WCHU)
    {
      size_of_header = 328;
    }
    else if (WCVersion == WCAlexander)
    {
      size_of_header = 327;
    }
    char header[size_of_header];
    nitem = fread(header, 1, size_of_header, pFILE);

    cout << "Header:\n"
         << header << endl;

    char *word;
    word = strtok(header, " \n");
    while (word != NULL)
    {
      if (strcmp("ACQUIRED:", word) == 0)
      {
        word = strtok(NULL, " \n");
        nActiveCh = atoi(word);
        break;
      }
      word = strtok(NULL, " \n");
    }

    if (nActiveCh > 9)
    {
      cout << endl;
      char dummy;
      nitem = fread(&dummy, 1, 1, pFILE);
    }

    int whileCounter = 0;
    /*Loop over events. Events are processed and analysed one by one in order.*/
    while (nitem > 0)
    { //event loop
      skipThisEvent = false;
      std::vector<TObject *> eventTrash;
      whileCounter++;
      nitem = fread(&EventNumber, sizeof(int), 1, pFILE);
      nitem = fread(&EpochTime, sizeof(double), 1, pFILE);
      nitem = fread(&Year, sizeof(unsigned int), 1, pFILE);
      nitem = fread(&Month, sizeof(unsigned int), 1, pFILE);
      nitem = fread(&Day, sizeof(unsigned int), 1, pFILE);
      nitem = fread(&Hour, sizeof(unsigned int), 1, pFILE);
      nitem = fread(&Minute, sizeof(unsigned int), 1, pFILE);
      nitem = fread(&Second, sizeof(unsigned int), 1, pFILE);
      nitem = fread(&Millisecond, sizeof(unsigned int), 1, pFILE);
      if (WCVersion == WCHU)
      {
        nitem = fread(&nCh, sizeof(unsigned int), 1, pFILE); // since V2.8.14 the number of stored channels is written for each event
      }
      else if (WCVersion == WCAlexander)
      {
        nCh = 16;
      }

      if (EventNumber % 100 == 0)
      {
        printf("POS, ev, y-m-d-h-min-s-ms, nActive-nCh: %ld, %d, %d-%d-%d-%d-%d-%d-%d, %d-%d \n", ftell(pFILE), EventNumber, Year, Month, Day, Hour, Minute, Second, Millisecond, nActiveCh, nCh);
      }

      float MeasuredBaseline[16];
      float AmplitudeValue[16];
      float ComputedCharge[16];
      float RiseTimeInstant[16];
      float FallTimeInstant[16];
      float RawTriggerRate[16];
      float floatR = -1;

      /*Loop over individual channels. For each event the data from every channel is 
      processed and analysed one by one in order*/

      for (int i = 0; i < nCh; i++)
      {
        nitem = fread(&ChannelNr[i], sizeof(int), 1, pFILE);
        nitem = fread(&EventIDsamIndex[i], sizeof(int), 1, pFILE);
        nitem = fread(&FirstCellToPlotsamIndex[i], sizeof(int), 1, pFILE);
        nitem = fread(&floatR, 1, 4, pFILE);
        MeasuredBaseline[i] = floatR;
        nitem = fread(&floatR, 1, 4, pFILE);
        AmplitudeValue[i] = floatR;
        nitem = fread(&floatR, 1, 4, pFILE);
        ComputedCharge[i] = floatR;
        nitem = fread(&floatR, 1, 4, pFILE);
        RiseTimeInstant[i] = floatR;
        nitem = fread(&floatR, 1, 4, pFILE);
        FallTimeInstant[i] = floatR;
        nitem = fread(&floatR, 1, 4, pFILE);
        RawTriggerRate[i] = floatR;
        ChannelNr[i] = i;

        /*
        __ Set WOMID _________________________________________________________
        The labeling of the WOMs in the box was done using the letters A,B,C,D. For convinience these letters are here replaced by the numbers 1-4 which is stored in the root-tree for every channel and every event.
        */
        if (WCVersion == WCAlexander)
        {
          if (i <= 6)
          {
            WOMID[i] = 3;
          }
          else if (i >= 7 && i <= 14)
          {
            WOMID[i] = 4;
          }
        }
        else
        {
          if (i <= 6)
          {
            WOMID[i] = 1;
          }
          else if (i >= 7 && i <= 14)
          {
            WOMID[i] = 2;
          }
        }

        TString title("");
        title.Form("ch %d, ev %d", i, EventNumber);
        hCh.Reset();
        hCh.SetTitle(title);

        /*
        __ Waveform Histogram _______________________________________________
        Writing the signal amplitude values into the root-histogram hCh.
        */
        if (i == 15)
        {
          for (int j = 0; j < 1024; j++)
          {
            nitem = fread(&amplValues[i][j], sizeof(short), 1, pFILE);
            hCh.SetBinContent(j + 1, -(amplValues[i][j] * coef * 1000));
          }
        }
        else
        {
          for (int j = 0; j < 1024; j++)
          {
            nitem = fread(&amplValues[i][j], sizeof(short), 1, pFILE);
            hCh.SetBinContent(j + 1, (amplValues[i][j] * coef * 1000));
          }
        }

        /*The error of each value in each bin is set to 0.5 mV.*/
        for (int j = 1; j <= hCh.GetXaxis()->GetNbins(); j++)
        {
          hCh.SetBinError(j, 0.5);
        }

        /*Analysis if the event/signal starts.*/
        max[i] = hCh.GetMaximum();
        min[i] = hCh.GetMinimum();

        /*Saving the histogram of that event into a temporary histogram hChtemp. These histograms are available outside of the channel-loop. If analysis using the signals/events of multiple channels needs to be done, this can be accomplished by using hChtemp after the channel-loop.*/
        hChtemp.at(i) = hCh;

        /*
        __ Baseline Fit _______________________________________________________
        Calculate baseline values infront and after the triggered signal
        Triggered signal is expected in the range fromm 100 to 150 ns
        */
        BL_fit(&hChtemp.at(i), BL_output, 0.0, 75.0); //FRONT
        BL_lower[i] = BL_output[0];
        BL_RMS_lower[i] = BL_output[1];
        BL_Chi2_lower[i] = BL_output[2];
        BL_pValue_lower[i] = BL_output[3];
        BL_fit(&hChtemp.at(i), BL_output, 270.0, 320.0); //END  Default: 220-329
        BL_upper[i] = BL_output[0];
        BL_RMS_upper[i] = BL_output[1];
        BL_Chi2_upper[i] = BL_output[2];
        BL_pValue_upper[i] = BL_output[3];

        // determine "best" baseline
        if (BL_Chi2_upper[i] <= BL_Chi2_lower[i])
        {
          BL_used[i] = BL_upper[i];
          BL_Chi2_used[i] = BL_Chi2_upper[i];
          BL_pValue_used[i] = BL_pValue_upper[i];
        }
        else
        {
          BL_used[i] = BL_lower[i];
          BL_pValue_used[i] = BL_pValue_lower[i];
          BL_Chi2_used[i] = BL_Chi2_lower[i];
        }

        if (i == skipInChannel && allowEventSkipping)
        {
          if (BL_Chi2_used[i] > 1)
          {
            skipThisEvent = true;
            skippedCount = skippedCount + 1;
          }
          else
          {
            skipThisEvent = false;
          }
        }

        // SWITCH dynamic <-> constant baseline
        float BL_shift;
        if (switch_BL)
        {
          BL_shift = BL_used[i];
        }
        else
        {
          BL_shift = BL_const[i];
        }

        /*
        __ Peakfinder _________________________________________________________
        Implemented to search double-muon-event candiates
        Set maximum number of peaks stored in beginning of script -> nPeaks
        peakX/Yarray[nCh][nPeaks] stores peak coordinates as branches in tree
        Switch on/off with pfON
        -> when off:  set peakX/Yarray[nCh][nPeaks] to zero
        */
        gErrorIgnoreLevel = kError; // suppress root terminal output

        bool pfON = false;
        if (i < 15)
        {
          pfON = false;
        }                     // switch on/off peakfinder
        int sigma = 10;       // sigma of searched peaks
        Double_t thrPF = 0.1; // peakfinder threshold
        TPolyMarker pm;       // store polymarker showing peak position, print later
                              // peakfinder(&hCh, 0, 130, nPeaks, sigma, thrPF, peakX[i], peakY[i], &pm, pfON);
        peakfinder(&hCh, 0, 320, nPeaks, sigma, thrPF, peakX[i], peakY[i], &pm, pfON);

        gErrorIgnoreLevel = kUnset; // return to normal terminal output

        // baseline-correct Y-values and convert to units of p.e.
        if (pfON)
        {
          for (int j = 0; j < nPeaks; ++j)
          {
            peakY[i][j] = amp2pe(peakY[i][j], calib_amp[i], BL_shift);
          }
        }

        // printf("X: %d %f %f %f %f \n",i,peakX[i][0],peakX[i][1],peakX[i][2],peakX[i][3]);
        // printf("Y: %d %f %f %f %f \n",i,peakY[i][0],peakY[i][1],peakY[i][2],peakY[i][3]);

        /*
        __ CFD _____________________________________________________________
        Setting the signal time by using a constant fraction disriminator method.
        The SiPM and the trigger sinals are handled differently using different thresholds.
        */
        if (i == 9)
        { //trigger
          t[i] = CDF(&hCh, 0.5);
        }
        else
        { //SiPMs
          t[i] = CFD2(&hCh, 0.35);
          if (t[i] < 95)
          {
            t[i] = CFDinvert2(&hCh, 0.35);
          }
        }

        /*
        __Print Raw Data to .txt ______________________________________________
        Select channel. Prints histogram raw data in a two column text file
        */

        // if (i==4 && BL_chi2[4]<1.7 && BL_chi2[4]>0.7)
        // if (i==4)
        // {
        //   TString histDataName;
        //   histDataName.Form("Ch%d_hist_data.txt",i);
        //   TString path2hist_data;
        //   path2hist_data.Form("%s/%s",(const char*)plotSaveFolder,(const char*)histDataName);
        //   FILE * histOut;
        //   histOut = fopen(path2hist_data,"a"); // produces overhead, maybe put this infront of loop

        //   Int_t nbins_x = hCh.GetNbinsX(); // bins at k==0 and k==nbins_x seem to have BinContent==0
        //   for (Int_t k=1; k<=nbins_x; k++)
        //   {
        //     fprintf(histOut,"%.4f %.8f\n",
        //     hCh.GetBinLowEdge(k)+hCh.GetBinWidth(k)/2,
        //     hCh.GetBinContent(k));
        //   }
        //   fclose(histOut);
        // }

        // clibrated sum
        // if(EventNumber%amp_array_printRate==0 && (i!=9)){
        if (EventNumber % amp_array_printRate == 0 && (i < 8))
        {
          C_amp_array.cd(i + 1);
          hCh.DrawCopy();
        }

        /*
        __ Integral & Amplitude ________________________________________
        There are several definitions of the integral of a signal used here. Those are:
        - Integral_0_300: Integration over the entire time window (~320ns)
        - Integral: Integration over a smaller time window (~50ns) relative to the trigger
        Additionally the number of p.e. is now calculated using the amplitude
        and the calibration factors in the calib_amp-vactor. The function 'PE' calculates the amplitude of the signal, subtracts the better BL value and divides by the calibration factor.
        */
        // Integral_0_300[i] = (hCh.Integral(1, 1024, "width")-0.0*1024*SP);

        Integral[i] = Integrate_50ns(&hCh, BL_shift) / calib_charge.at(i); // difined 50 ns window

        // calibrated, BL-shifted charge
        if (isDC)
          Integral_inRange[i] = integral(&hCh, 50, 75, BL_shift) / calib_charge.at(i); // for DC runs
        else
          Integral_inRange[i] = integral(&hCh, integralStart, integralEnd, BL_shift) / calib_charge.at(i);
        // Integral_inRange[i] = integral(&hCh, 145, 170, BL_shift) / calib_charge.at(i);

        // calibrated, BL-shifted amplitude at maximum in window
        if (isDC)
          amp[i] = PE(&hCh, calib_amp.at(i), BL_shift, 50.0, 100.0); // for DC runs
        else
          amp[i] = PE(&hCh, calib_amp.at(i), BL_shift, integralStart, integralEnd);
        //amp[i] = PE(&hCh, calib_amp.at(i), BL_shift, 135.0, 185.0);
        // reduced window
        if (isDC)
          amp_inRange[i] = PE(&hCh, calib_amp.at(i), BL_shift, 50.0, 75.0); // for DC runs
        else
          amp_inRange[i] = PE(&hCh, calib_amp.at(i), BL_shift, integralStart, integralEnd);
        //amp_inRange[i] = PE(&hCh, calib_amp.at(i), BL_shift, 145.0, 165.0);
        //amp_inRange[i] = PE(&hCh, calib_amp.at(i), BL_shift, 100.0, 150.0);

        /*
        __ Printing Wafevorms ____________________________________________
        The signals for events can be printed to a .pdf file called waves.pdf. The rate at which the events are drawn to waves.pdf is set via the variable wavesPrintRate. Additional requirements can be set in the if-statement to look at specific events only.
        The entire if-statement so far also plots lines at the found signal maximum, the corresponding integration limit, as well as the BL values to each of the histograms.
        */
        if (EventNumber % wavesPrintRate == 0)
        {
          cWaves.cd(1 + 4 * (i % 4) + (i) / 4);
          hCh.DrawCopy();
          hCh.GetXaxis()->SetRange((t[i] - 20) / SP, (t[i] + 30) / SP);
          int max_bin = hCh.GetMaximumBin();
          int lower_bin = max_bin - 20.0 / SP;
          int upper_bin = max_bin + 30.0 / SP;
          // double x = h->GetXaxis()->GetBinCenter(binmax);
          float max_time = hCh.GetXaxis()->GetBinCenter(max_bin);
          float lower_time = hCh.GetXaxis()->GetBinCenter(lower_bin);
          float upper_time = hCh.GetXaxis()->GetBinCenter(upper_bin);
          hCh.GetXaxis()->SetRange(0, 1024);
          TLine *ln4 = new TLine(0, BL_lower[i], 75, BL_lower[i]);
          TLine *ln5 = new TLine(270, BL_upper[i], 320, BL_upper[i]);
          TText *text = new TText(.5, .5, Form("%f %f", BL_lower[i], BL_upper[i]));
          ln4->SetLineColor(2);
          ln5->SetLineColor(2);
          ln4->Draw("same");
          ln5->Draw("same");
          text->Draw("same");
          if (pfON)
          {
            pm.Draw();
          } // print peakfinders polymarker
        }
        // End of loop over inividual channels
      }

      // continue;

      /*
      __ TIMING _____
      */
      trigT = t[9];
      for (int i = 0; i <= 15; i++)
      {
        tSiPM[i] = t[i] - trigT;
        /*
        if (tSiPM[i+7] < -66){
          t[i+7] = CDFinvert(&hChtemp.at(i+7),0.33);
          tSiPM[i+7] = t[i+7] - trigT;
        }
        */
      }

      /*
      __ FILLING SUM HISTOGRAMS ________________________
      __ Calibrated SUM SIGNAL of all channels ______________
      */
      TH1F hSum("hSum", "Calibrated Sum ;ns;Amplitude in npe", 1024, -0.5 * SP, 1023.5 * SP);
      for (int hSumIndex = 0; hSumIndex < 8; hSumIndex++)
      {
        // SWITCH dynamic <-> constant baseline
        float BL_shift;
        if (switch_BL)
        {
          BL_shift = BL_used[hSumIndex];
        }
        else
        {
          BL_shift = BL_const[hSumIndex];
        }
        free(f_const);
        f_const = new TF1("f_const", "pol0", 0, 320);
        f_const->SetParameter(0, BL_shift);

        hChtemp.at(hSumIndex).Add(f_const, -1);
        hChtemp.at(hSumIndex).Scale(1.0 / calib_amp.at(hSumIndex));

        hSum.Add(&hChtemp.at(hSumIndex), 1);
      }

      // get point of amplitude maximum in 50 ns window
      //amp_array = max_inRange(&hSum, 50., 100.0);
      // t_amp_array = t_max_inRange(&hSum, 50.0, 100.0);
      amp_array = max_inRange(&hSum, integralStart, integralEnd);
      t_amp_array = t_max_inRange(&hSum, integralStart, integralEnd);

      // amp_array = 8.0/7.0 * max_inRange(&hSum, 100.0, 150.0);

      C_amp_array.cd(9);
      hSum.DrawCopy();

      // get single channel amplitude/charge at time of sum maximum
      for (int i = 0; i < 8; i++)
      {

        chPE_amp[i] = amp_atTime(&hChtemp.at(i), t_amp_array);
        // SWITCH dynamic <-> constant baseline
        float BL_shift;
        if (switch_BL)
        {
          BL_shift = BL_used[i];
        }
        else
        {
          BL_shift = BL_const[i];
        }
        // reverse amplitude calibration before integration
        hChtemp.at(i).Scale(calib_amp.at(i));
        chPE_charge[i] = integral(&hChtemp.at(i), t_amp_array - 10, t_amp_array + 15, 0) / calib_charge.at(i);
        // chPE_charge[i] = integral(&hChtemp.at(i), t_amp_array-10, t_amp_array+15, BL_shift) / calib_charge.at(i);
      }
      // summed charge
      charge_array = chPE_charge[0] + chPE_charge[1] + chPE_charge[2] + chPE_charge[3] + chPE_charge[4] + chPE_charge[5] + chPE_charge[6] + chPE_charge[7];
      // timing
      t_amp_array_invCFD = CFDinvert2(&hSum, 0.4);
      t_amp_array_invCFD_wrtTrig = trigT - t_amp_array_invCFD;

      /* end */

      /*Saving the plotted signals/events to a new page in the .pdf file.*/
      if (EventNumber % wavesPrintRate == 0)
      {
        if (wavePrintStatus < 0)
        {
          cWaves.Print((TString)(plotSaveFolder + "/waves.pdf("), "pdf");
          wavePrintStatus = 0;
        }
        else
          cWaves.Print((TString)(plotSaveFolder + "/waves.pdf"), "pdf");
      }
      if (EventNumber % trigPrintRate == 0)
      {
        if (trigPrintStatus < 0)
        {
          cTrig.Print((TString)(plotSaveFolder + "/trig.pdf("), "pdf");
          trigPrintStatus = 0;
        }
        else
          cTrig.Print((TString)(plotSaveFolder + "/trig.pdf"), "pdf");
      }
      if (EventNumber % signalPrintRate == 0)
      {
        if (signalPrintStatus < 0)
        {
          cSignal.Print((TString)(plotSaveFolder + "/signal.pdf("), "pdf");
          signalPrintStatus = 0;
        }
        else
          cSignal.Print((TString)(plotSaveFolder + "/signal.pdf"), "pdf");
      }

      // clibrated sum
      if (EventNumber % amp_array_printRate == 0)
      {
        if (amp_array_PrintStatus < 0)
        {
          C_amp_array.Print((TString)(plotSaveFolder + "/amp_array.pdf("), "pdf");
          amp_array_PrintStatus = 0;
        }
        else
          C_amp_array.Print((TString)(plotSaveFolder + "/amp_array.pdf"), "pdf");
      }

      /*Writing the data for that event to the tree.*/
      if (!skipThisEvent)
      {
        tree->Fill();
      }
      // cout<<"BASELINE: "<<skipThisEvent<<"     "<< skippedCount<<endl;

      // cout<<"SIZE OF: "<<sizeof(amp_array)<< endl;
      //tree->Print();
    }
    auto nevent = tree->GetEntries();

    cout << "EVENTS:  " << nevent << endl;
    cout << "SKIPPED EVENTS:  " << skippedCount << endl;

    fclose(pFILE);
  }
  //tree->Print();

  /*Clearing objects and saving files.*/
  inList.close();
  cWaves.Clear();
  cWaves.Print((TString)(plotSaveFolder + "/waves.pdf)"), "pdf");
  cCh0.Print((TString)(plotSaveFolder + "/ch0.pdf)"), "pdf");
  cTrig.Print((TString)(plotSaveFolder + "/trig.pdf)"), "pdf");
  cSignal.Print((TString)(plotSaveFolder + "/signal.pdf)"), "pdf");

  // clibrated sum
  C_amp_array.Clear();
  C_amp_array.Print((TString)(plotSaveFolder + "/amp_array.pdf)"), "pdf");

  rootFile = tree->GetCurrentFile();
  rootFile->Write();
  rootFile->Close();
}