#include "stdafx.h"
#include "mainutils.h"
#include "MessagesProcessor.h"

using namespace std;

int main(int argc, char *argv[])
{
	printUsageStrings(argc,argv);
	processInputParameters(argc,argv);
	MessagesProcessor m;
	m.setDayScan(dayFiles);
	m.processMessages(inputFolder);
	m.printIndexes();
	m.saveIUKIUSMessages(outputFolder);
	return 0;
}