//root
#include <TString.h>
#include <TH1F.h>

//C, C++
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>

//local
#include "geometry.h"
#include "analysis.h"
#include "readFull.h"

using namespace std;

/* Declarations of global/external variables which are used accross all 
other files (read.C, geometry.C, analysis.C) */
string WCVersion;
int runNr = -999;
int pdgID = -999;
int isSP = -999;
int mp = -999;
int safPMT2 = -999;  //solid angle factor of pmt 2
int safPMT1 = -999;  //solid angle factor of pmt 1
int safSiPM = -999;  //solid angle factor of SiPM
int trackL = -999;   //track length
float energy = -999; // [GeV]
float horizontal = -999;
float vertical = -999;
float angle = -999;
std::vector<float> pmt2Pos = {410, 410};

int main(int argc, char *argv[])
{
  TString inFileList;
  TString inDataFolder;
  TString outFile;
  string runName;
  cout << "NUMBER OF ARGUMENTS IN MAIN: " << argc << endl;
  inFileList = argv[1];
  inDataFolder = argv[2];
  outFile = argv[3];
  runName = argv[4];
  string dynamicBL= argv[5];
  string isDC = argv[6];
  string useCalibValues = argv[7];


 /* cout << "In data file list : " << inFileList << endl
       << "In data path      : " << inDataFolder << endl
       << "Out root file     : " << outFile << endl
       << "RunName    : " << runName << endl
       << "dynamicBL/isDC/useCalib   : " << dynamicBL << isDC << useCalibValues << endl
       << "FIRST:    : " << argv[0] << endl;
*/
  /* Used i.e. for calibration measurements which do not include angluar or 
    positional information as in testbeam measurements. Only information 
    on the local directories is handet to main() */

  // runNr=atoi(argv[4]);

  WCVersion = "AB";
  cout << WCVersion << endl;

  cout << "In data file list : " << inFileList << endl
       << "In data path      : " << inDataFolder << endl
       << "Out root file     : " << outFile << endl;
  readFull(inFileList, inDataFolder, outFile, runName, dynamicBL, isDC,useCalibValues);

  return 0;
}