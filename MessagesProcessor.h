#include "stdafx.h"

//#define VERBOSE

#ifndef MESSAGESPROCESSOR
#define MESSAGESPROCESSOR

typedef unsigned char byte;

class MessagesProcessor
{
public:
	MessagesProcessor();
	~MessagesProcessor();
	void processMessages(string folder);
	void printIndexes();
	void setDayScan(bool enable)
	{
		dayScan=enable;
	}
	void saveIUKIUSMessages(string outfolder);
private:
	bool dayScan;
	string ReadAllBytes(string filename);
	bool checkIUKIUS(string idx,string msg);
	vector<char> CharReadAllBytes(string filename);
	vector<string> files;
	vector<string> messages;
	vector<string> msg_data;
	vector<string> msg_index;
	vector<string> msg_cfnm;
	void findMSGFiles();
	void addMessage(string msg,string index,string cfnm);
	int getdir(string dir);
	string mFolder;
	string curFileName;
};

#endif