#include "stdafx.h"

#ifndef MAINUTILS_H
#define MAINUTILS_H

string inputFolder,outputFolder;

bool printUsageStrings(int argc, char* argv[])
{
	if (argc <= 1)
	{
		std::cout << "usage: " << argv[0] << " /O=out_folder /I=in_folder" << std::endl;
		return 0;
	}
	return 1;
}

void recognizeToken(string arg)
{
	if (arg.find('O') != string::npos)
	{
		outputFolder = arg.substr(arg.find('O') + 3, arg.length() - 3);
		cout<<"OutputFolder:"<<outputFolder<<endl;
	}
	if (arg.find('I') != string::npos)
	{
		inputFolder = arg.substr(arg.find('I') + 3, arg.length() - 3);
		cout<<"InputFolder:"<<inputFolder<<endl;
	}
}

void processInputParameters(int argc, char* argv[])
{
	std::istringstream oss;
	stringstream ss;
	int i;
	inputFolder=".";
	outputFolder="";
	for (i = 1; i != argc; i++)
		recognizeToken(string(argv[i]));
	printf("Point1\n");
}
#endif