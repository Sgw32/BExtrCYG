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
	bool processPRNFile(string year,string _outfolder,int msgtype);
	void setDayScan(bool enable)
	{
		dayScan=enable;
	}
	void setCBUFRProcess(bool cb)
	{
		cbufrProcess = cb;
	}
	void processBUFRMessage(size_t i, int msgtype);
	void processKN04Message(size_t i, int msgtype);
	void saveIUKIUSMessages(string outfolder);
	void checkKN04Session(string filename); //We delete KN04 file every time we launch app
    inline void setOverrideGrep(string _grep){overrideGrep = _grep;}
	string getNowYear();
	string getNowMonth();
	string getNowDay();
	string getNowHour();
	void registerResultInLog(string idx,string path);
private:
	string outfolder;
    string overrideGrep;
	bool dayScan;
	bool cbufrProcess;
	string ReadAllBytes(string filename);
	bool checkIUKIUS(string idx,string msg);
	int getIUKIUS(string idx,string msg);
	vector<char> CharReadAllBytes(string filename);
	vector<string> files;
	vector<string> messages;
	vector<string> msg_data;
	map<string,string> kn04_msg_data;
	vector<string> msg_index;
	vector<string> msg_cfnm;
	vector<string> kn04session;
	void findMSGFiles();
	void addMessage(string msg,string index,string cfnm);
	int getdir(string dir);
	string mFolder;
	string curFileName;
	
};

#endif
