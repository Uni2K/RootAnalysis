#include <stdio.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>

void removeSubstring(char *s, const char *toremove)
{
	while (s = strstr(s, toremove))
		memmove(s, s + strlen(toremove), 1 + strlen(s + strlen(toremove)));
}

int main(int argc, char const *argv[])
{
	//Argument: ./runs/Fast//22_muon6_pos4/0/out.root/T||./runs/Fast//22_muon6_pos4/1/out.root/T||./runs/Fast//22_muon6_pos4/2/out.root/T||
	//std::cout<<argv[0]<<"  ::::::::::: "<<argv[1]<<"::::::::"<<argc <<std::endl;
	TString inFileList;
	TString inDataFolder;
	TString outFile;
	TChain *chain = new TChain("T");
	char const *str = argv[1];

	// TESTING char const *str="./runs/Fast//22_muon6_pos4/0/out.root/T||./runs/Fast//22_muon6_pos4/1/out.root/T||./runs/Fast//22_muon6_pos4/2/out.root/T||./runs/Fast//22_muon6_pos4/3/out.root/T||./runs/Fast//22_muon6_pos4/4/out.root/T||./runs/Fast//22_muon6_pos4/5/out.root/T||./runs/Fast//22_muon6_pos4/6/out.root/T||./runs/Fast//22_muon6_pos4/7/out.root/T||./runs/Fast//22_muon6_pos4/8/out.root/T||./runs/Fast//22_muon6_pos4/9/out.root/T";
	char delimiter[] = "||";
	char *ptr;
	char *st = strdup(str);

	ptr = strtok(st, delimiter);
	printf("Pre Abschnitt gefunden (ROOT MERGER): %s \n", ptr);
	chain->Add(ptr);

	std::cout << "LISTE: " << str << std::endl;

	//ÜBERGEBE DIE SCHEISSE EINFACH ALS CHAR CONST IN ARGV, JEDE EINZELNE ROOT DATEI

	while (ptr != NULL)
	{

		//std::cout<< (const char *)ptr[0]=='\0'
		ptr = strtok(NULL, delimiter);

		if (ptr != NULL)
		{
			printf("Abschnitt gefunden (ROOT MERGER): %s \n", ptr);
			chain->Add(ptr);
		}
	}

	chain->Merge(argv[2]);

	return 0;
}