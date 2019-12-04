#include "stdafx.h"
#include "mainutils.h"
#include "MessagesProcessor.h"

using namespace std;

int main(int argc, char *argv[])
{
	printUsageStrings(argc,argv);
	processInputParameters(argc,argv);
	MessagesProcessor m;
    m.setOverrideGrep(grepStr);
	m.setDayScan(dayFiles);
	m.setCBUFRProcess(cbufrProcess);
	m.processMessages(inputFolder);
	m.printIndexes();
	m.saveIUKIUSMessages(outputFolder);
	return 0;
}