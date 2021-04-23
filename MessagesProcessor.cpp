#include "MessagesProcessor.h"
#define KN04_PREFIXES 4

string type_prefix[2] = {"IUK","IUS"};

string kn04_prefix[4] = {"TTAA","TTBB","TTCC","TTDD"};

void v_printf(char* data)
{
#ifdef VERBOSE
	//printf(data);
#endif
}
/*
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}
*/

MessagesProcessor::MessagesProcessor()
{
	dayScan=false;
    overrideGrep = "";
}

MessagesProcessor::~MessagesProcessor()
{
	
}

vector<char> MessagesProcessor::CharReadAllBytes(string filename)
{
	std::ifstream infile(filename.c_str(), ios::binary);
	std::vector<char> buffer;
	infile.seekg(0, infile.end);
	size_t length = infile.tellg();
	infile.seekg(0, infile.beg);

	//read file
	if (length > 0) {
		buffer.resize(length);    
		infile.read(&buffer[0], length);
	}
	return buffer;
}

string MessagesProcessor::ReadAllBytes(std::string path)
{
	//printf("Point6_RAB%s\n",path.c_str());
	std::ifstream file(path.c_str());
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	//printf("Point6_F%s\n",content.c_str());
	return content;
}

void MessagesProcessor::printIndexes()
{
	for (auto i=msg_index.begin();i!=msg_index.end();i++)
    {
		cout << *i <<endl;
	}
}

void MessagesProcessor::addMessage(string msg,string index,string cfnm)
{
	//printf("MSG_%s\n",cfnm.c_str());
	msg_data.push_back(msg);
	msg_index.push_back(index);
	msg_cfnm.push_back(cfnm);
	//cout<<"Adding message " <<index<< msg.length()<<endl;
	//printf("Ok.\n");
}

void MessagesProcessor::processMessages(std::string inputFolder)
{
	//printf("Point2\n");
	mFolder = inputFolder;
	cout << "inputFolder "<<mFolder;
	getdir(inputFolder);
	//printf("Point3\n");
	findMSGFiles();
	//printf("Point5\n");
	string cfnm = "";
	//system("read -rsp $'Press enter to continue...\n'");
	for (auto filename=messages.begin();filename!=messages.end();filename++)
    {
		//system("read -rsp $'Filename...\n'");
		//string l_cmd = "echo '" + *filename +"'";
		//system(l_cmd.c_str());
		//system("read -rsp $'Press enter to continue...\n'");
		//printf("Point6_start\n");
		//system("read -rsp $'Press enter to continue...\n'");
        curFileName = *filename;
		cfnm = curFileName;
		
		//system("read -rsp $'Press enter to continue...\n'");
        vector<char> fileBytes = CharReadAllBytes(curFileName);
		//system("read -rsp $'Press enter to continue...\n'");
		//printf("Point6_OK\n");
        byte state = 0;
        int msgByteCount = -1;
        int idxCount = 0;
        int filesize;
        // state = 0 - Пишем индекс
        // state = 1 - Найден SOH
        // state = 2 - Сообщение прочитано
        string idx = "";
        string message = "";
        string messageBuilder;
        string idxBuilder;
		//printf("Point6_OK2\n");
		//system("read -rsp $'Press enter to continue...\n'");
        for (size_t i = 0; i < fileBytes.size(); i++)
        {
            byte b = fileBytes[i];
			//printf("Point6_ITER\n");
            switch (state)
            {
                case 0:
                    if ((b != 0x01))
                    {
                        // Будем писать размер
                        idxBuilder+=(char)b;
                    }
                    else
                    {
						//printf("Point6_FILESIZE\n");
                        // Найдём SOH, здесь пойдёт уже BUFR
                        string dta = idxBuilder;
                        filesize = atoi(dta.c_str());
                        idxBuilder = "";
                        //messageBuilder.Append(filesize.ToString("D10"));
                        messageBuilder+=(char)0x01;
                        msgByteCount = filesize / 100 - 1;
                        idxCount = 3;
                        idx = "";
                        state = 1;
                        message = "";
                    }
                    break;
                case 1:
                    if ((msgByteCount == 0))
                    {
                        message = messageBuilder;
                        idxBuilder = "";
                        messageBuilder = "";
						//Внезапно оказалось, что idx - не индекс, а непонятно что.
			//cout<<"Adding message:"<<idx<<endl;
                        addMessage(message, idx,curFileName);
                        state = 0;
                        idx = "";
                    }
                    //Прочитаем сообщение
                    if (msgByteCount > 0)
                    {
                        if (idx == "")
                        {
                            if (idxCount > 0)
                                idxCount--;
                            else
                            {
                             
                                idxBuilder+=b;
                                idxBuilder+=fileBytes[i + 1];
                                idxBuilder+=fileBytes[i + 2];
                                idxBuilder+=fileBytes[i + 3];
                                idxBuilder+=fileBytes[i + 4];
                                idx = idxBuilder;
                            }
                        }
                        //message += Convert.ToChar(b);
                        messageBuilder+=(char)b;
                        msgByteCount--;
                    }
                    break;
                default:
                    break;
            }
        }
        if ((msgByteCount == 0))
        {
	    message = messageBuilder;
            idxBuilder = "";
            messageBuilder = "";
//            cout<<"Adding message:"<<idx<<endl;
            addMessage(message, idx,curFileName);
            state = 0;
            idx = "";
        }
    }
	//system("read -rsp $'Finished...\n'");
}

string MessagesProcessor::getNowYear()
{
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
	char date[8];
	sprintf(date,"%04d",now->tm_year+1900);
	string grep(date);
	return grep;
}

string MessagesProcessor::getNowMonth()
{
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
	char date[8];
	sprintf(date,"%02d",now->tm_mon+1);
	string grep(date);
	return grep;
}

string MessagesProcessor::getNowDay()
{
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
	char date[8];
	sprintf(date,"%02d",now->tm_mday);
	string grep(date);
	return grep;
}

string MessagesProcessor::getNowHour()
{
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
	char date[8];
	sprintf(date,"%02d",now->tm_hour);
	string grep(date);
	return grep;
}

void MessagesProcessor::findMSGFiles()
{
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

	char date[9];
	sprintf(date,"%04d%02d%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
	string grep(date);
    
    if (overrideGrep!="")
    {
        grep = overrideGrep;
        cout << "GREP overrided for:" << grep << endl;
    }
    
	cout << "Scan is for " << grep <<endl;

	for (vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		string filename = *it;
		string date2 = filename.substr(0,8);
		
		if ((date2==grep)||(!dayScan))
		{
			//cout << "File is " << date2 <<endl;
			if (filename.substr(filename.find_last_of(".") + 1)=="msg")
			{
				//cout << mFolder+filename << endl;
				messages.push_back(mFolder+filename);

			}
		}
	}
	cout << "SCAN OK!" << endl;
}

void MessagesProcessor::registerResultInLog(string idx,string path)
{
	string ltime = getNowHour();
	if ((ltime.substr(0,2)=="23")||
		(ltime.substr(0,2)=="22")||
		(ltime.substr(0,2)=="21")||
		(ltime.substr(0,2)=="00")||
		(ltime.substr(0,2)=="01"))
	{
		ltime = "0000";
	}
	else
	{
		ltime = "1200";
	}
	//trim(idx);
	string foldername = outfolder + "/" + getNowYear() + "/" + getNowMonth() + "/" + getNowDay() + "/" + ltime;
	string l_cmd = "mkdir -p '" + foldername + "'/";
	foldername+= + "/result_" + getNowHour() + ".log";
	system(l_cmd.c_str()); 
	ofstream write;
	write.open(foldername.c_str(), ios::out | ios::app );
	write << idx << ";" << path << ";"<< ltime << ";" << getNowHour() << endl;
	write.close();
}

bool MessagesProcessor::processPRNFile(string year,string outfolder,int msgtype)
{
    //PRN это результат работы cbufr, по этим данных даже исходя из названия файла многое можно получить. 
    //PRN впрочем теперь удаляется
    
	bool result=false;
	getdir(outfolder);
	for (vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		string filename = *it;
		if (filename.substr(filename.find_last_of(".") + 1)!="bin")
		{
			if (filename.substr(filename.find_last_of(".") + 1)=="PRN")
			{
				//Получен	
                size_t undline = filename.find_last_of("_");
                
				string idx = filename.substr(0,5);
				string month = filename.substr(undline+5,2);
				string day = filename.substr(undline+7,2);
                string ltime = filename.substr(undline+9,4);
                
                if ((ltime.substr(0,2)=="23")||
                    (ltime.substr(0,2)=="22")||
                    (ltime.substr(0,2)=="21")||
                    (ltime.substr(0,2)=="00")||
                    (ltime.substr(0,2)=="01"))
                {
                    ltime = "0000";
                }
                else
                {
                    ltime = "1200";
                }
				//trim(idx);
				string foldername = outfolder + "/" + year + "/" + month + "/" + day + "/" + ltime;
				string l_cmd = "echo '" + foldername+"'";
				system(l_cmd.c_str());
				l_cmd = "mkdir -p '" + foldername + "'/";
				system(l_cmd.c_str()); //Make index and month folder
				l_cmd = "cp --force --backup=numbered --suffix=bin '" + outfolder + "/temp.bin' '" + foldername + "/'";
				l_cmd += filename.substr(0,filename.find_last_of("."));
				l_cmd += "_"+type_prefix[msgtype-1];
				l_cmd += ".bin";
				//cout<<"Index:"<<idx<<std::endl;
				//system("read -rsp $'Moving BUFR...\n'");
				system(l_cmd.c_str()); //Move BUFR file
				string resFileName = foldername + filename.substr(0,filename.find_last_of(".")) + "_"+type_prefix[msgtype-1] + ".bin";
				registerResultInLog(idx,resFileName);
				l_cmd = "rm '" + outfolder + "/*.PRN'";
				system(l_cmd.c_str()); //Delete CBUFR result
				result=true;
				return true;
			}
		}
	}
	return result;
}

int MessagesProcessor::getIUKIUS(string index,string message)
{
	size_t index_pos = message.find(index);
	string header;
    if (message.length() > index_pos + 2 + 5 + 1+3)
    {
        header = message.substr(index_pos + 2 + 5 + 1, 3);
	cout << "Header:" <<header<<endl;
        if ((header == "IUK"))
		{
			//printf("Header OK...\n");
            return 1;
		}
		if ((header == "IUS"))
		{
			//printf("Header OK...\n");
            return 2;
		}

		
		for (size_t i=0;i!=KN04_PREFIXES;i++)
		{
			size_t test_kn04 = message.find(kn04_prefix[i]);
			if (test_kn04 != std::string::npos)
			{
				printf("Header KN04 %s...\n",header.c_str());
				return 3;
			}
		}
		cout<<"Unknown header:"<<header<<endl;
    }

	
	//printf("Header%s Index%s...\n",header.c_str(),index.c_str());
    return 0;
}

bool MessagesProcessor::checkIUKIUS(string index,string message)
{
	size_t index_pos = message.find(index);
	string header;
    if (message.length() > index_pos + 2 + 5 + 1+3)
    {
        header = message.substr(index_pos + 2 + 5 + 1, 3);
        if ((header == "IUK") || (header == "IUS"))
		{
			//printf("Header OK...\n");
            return true;
		}
    }
	//printf("Header%s Index%s...\n",header.c_str(),index.c_str());
    return false;
}

std::vector<std::string> splitpath(
  const std::string& str
  , const std::set<char> delimiters)
{
  std::vector<std::string> result;

  char const* pch = str.c_str();
  char const* start = pch;
  for(; *pch; ++pch)
  {
    if (delimiters.find(*pch) != delimiters.end())
    {
      if (start != pch)
      {
        std::string str(start, pch);
        result.push_back(str);
      }
      else
      {
        result.push_back("");
      }
      start = pch + 1;
    }
  }
  result.push_back(start);

  return result;
}

void MessagesProcessor::processKN04Message(size_t i, int msgtype)
{
	std::set<char> delims{'\\','/'};

	string fn = splitpath(msg_cfnm[i],delims).back();
	string extt = fn.substr(0,fn.find_last_of("."));
	string year = fn.substr(0,4); //Из имени бюллетеня берётся год, месяц, день
	string month = fn.substr(4,2);
	string day = fn.substr(6,2);
	
	
	string idx = ""; // индекс берется из KN04

	//Найдём TT**

	size_t index_pos = msg_data[i].find("TT");
	idx = msg_data[i].substr(index_pos+11,5);
	string ltime = msg_data[i].substr(index_pos+7,2);
		
	string l_cmd = "echo 'index " + idx+"'";
	system(l_cmd.c_str());

	bool check_index = isdigit(idx[0])&&isdigit(idx[1])&&isdigit(idx[2])&&isdigit(idx[3])&&isdigit(idx[4]);

	if (check_index)
	{
		string foldername = outfolder + "KN04/" + year + "/" + month + "/"+ day + "/";
		l_cmd = "echo '" + foldername+"'";
		system(l_cmd.c_str());
		//l_cmd = "rm -rf '" + foldername + "/'";
		//system(l_cmd.c_str()); //Make index and month folder
		//system("read -rsp $'Press enter to continue...\n'");
		l_cmd = "mkdir -p '" + foldername + "/'";
		system(l_cmd.c_str()); //Make index and month folder

		string fileName = foldername + "/" + year + month + day + ltime + ".rdisnd";
		checkKN04Session(fileName);
		l_cmd = "echo 'fname " + fileName+"'";
		system(l_cmd.c_str());

		ofstream write;
		write.open(fileName.c_str(), ios::out | ios::binary | ios::app );
		for (size_t j = 0; j != msg_data[i].length(); j++)
		{
			write.put(msg_data[i][j]);
		}
		write.close();
	}
}

void MessagesProcessor::checkKN04Session(string filename)
{
	for (auto i=kn04session.begin();i!=kn04session.end();i++)
    {
		if (filename==*i)
			return;
	}
	string l_cmd = "rm -rf " + filename + "";
	system(l_cmd.c_str()); //Make index and month folder
	kn04session.push_back(filename);
}

void MessagesProcessor::processBUFRMessage(size_t i, int msgtype)
{
	std::set<char> delims{'\\','/'};
	string fn = splitpath(msg_cfnm[i],delims).back();
	string extt = fn.substr(0,fn.find_last_of("."));
	string year = fn.substr(0,4); //Из имени бюллетеня берётся год.
	//string date = fn.substr(5,4);
	if (!cbufrProcess)
	{
		string foldername = outfolder + "/" + year + "/"+ msg_index[i];
		string l_cmd = "mkdir -p " + foldername;
		system(l_cmd.c_str());
		string fileName = foldername + "/" + extt + 
		"_" + msg_index[i] + "_" + type_prefix[msgtype-1] + "_.bin";
		ofstream write;
		write.open(fileName.c_str(), ios::out | ios::binary);
		for (size_t j = 0; j != msg_data[i].length(); j++)
		{
			write.put(msg_data[i][j]);
		}
		write.close();
		printf("filename:%s\n",fileName.c_str());
	}
	else
	{
		string foldername = outfolder + "/" + year;
		string l_cmd = "mkdir -p " + foldername;
		system(l_cmd.c_str());
		string fileName = outfolder + "/temp.bin"; //Temp file for cbufr
		ofstream write;
		write.open(fileName.c_str(), ios::out | ios::binary);
		for (size_t j = 0; j != msg_data[i].length(); j++)
		{
			write.put(msg_data[i][j]);
		}
		write.close();
		//system("read -rsp $'Wrote temp...\n'");
		l_cmd = "wine " + outfolder + "/cbufr.exe export " + outfolder + "/temp.bin " + outfolder;
		system(l_cmd.c_str());
		//system("read -rsp $'Processed cbufr...\n'");
		//получим файл *.prn
		//bool noresave = processPRNFile(year,outfolder,msgtype); //Если не найден то пересохраним
		l_cmd = "rm -rf " + outfolder + "/*.PRN";
		system(l_cmd.c_str());
		/*if (!noresave)
		{
			fileName = outfolder + "/" + extt + "_" + msg_index[i] + "_" + type_prefix[msgtype-1] + "_.bin";
			write.open(fileName.c_str(), ios::out | ios::binary);
			for (size_t j = 0; j != msg_data[i].length(); j++)
			{
				write.put(msg_data[i][j]);
			}
			write.close();
			printf("CDENY:%s\n",fileName.c_str());
		}*/
	}
}

void MessagesProcessor::saveIUKIUSMessages(string _outfolder)
{
	outfolder = _outfolder;
	printf("Saving messages...\n");
	//system("read -rsp $'Press enter to continue...\n'");
	for (size_t i = 0; i != msg_data.size(); i++)
    {
		printf("Checking index number %s\n",msg_index[i].c_str());
		int msgtype = getIUKIUS(msg_index[i],msg_data[i]); //0 1 2
		switch (msgtype)
		{
		case 1:
		case 2:
			processBUFRMessage(i,msgtype);
		case 3:
			processKN04Message(i,msgtype);
		default:
			break;
		}
		//printf("Check ok.\n");
    }
	//printf("Save ok.\n");
}

int MessagesProcessor::getdir(string dir)
{
	files.clear();
	DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}



