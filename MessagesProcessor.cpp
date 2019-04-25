#include "MessagesProcessor.h"

string type_prefix[2] = {"IUK","IUS"};

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
        // state = 0 - ����� ������
        // state = 1 - ������ SOH
        // state = 2 - ��������� ���������
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
                        // ����� ������ ������
                        idxBuilder+=(char)b;
                    }
                    else
                    {
						//printf("Point6_FILESIZE\n");
                        // ����� SOH, ����� ����� ��� BUFR
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
						//�������� ���������, ��� idx - �� ������, � ��������� ���.
                        addMessage(message, idx,curFileName);
                        state = 0;
                        idx = "";
                    }
                    //��������� ���������
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
            addMessage(message, idx,cfnm);
        }
    }
}

void MessagesProcessor::findMSGFiles()
{
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

	char date[8];
	sprintf(date,"%04d%02d%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
	string grep(date);
	cout << "Scan is for " << grep <<endl;

	for (vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		string filename = *it;
		string date2 = filename.substr(0,8);
		
		if ((date2==grep)||(!dayScan))
		{
			cout << "File is " << date2 <<endl;
			if (filename.substr(filename.find_last_of(".") + 1)=="msg")
			{
				cout << mFolder+filename << endl;
				messages.push_back(mFolder+filename);

			}
		}
	}
	cout << "SCAN OK!" << endl;
}

bool MessagesProcessor::processPRNFile(string year,string outfolder,int msgtype)
{
	bool result=false;
	getdir(outfolder);
	for (vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		string filename = *it;
		if (filename.substr(filename.find_last_of(".") + 1)!="bin")
		{
			if (filename.substr(filename.find_last_of(".") + 1)=="PRN")
			{
				//�������	
				string idx = filename.substr(0,5);
				string month = filename.substr(10,2);
				string day = filename.substr(12,2);
				//trim(idx);
				string foldername = outfolder + "/" + year + "/"+ idx + "/" + month+ "/" + day;
				string l_cmd = "echo '" + foldername+"'";
				system(l_cmd.c_str());
				l_cmd = "mkdir -p '" + foldername + "'/";
				system(l_cmd.c_str()); //Make index and month folder
				l_cmd = "cp '" + outfolder + "/temp.bin' '" + outfolder + "/" + year + "/"+ idx + "/"+ month + "/" + day + "/'";
				l_cmd += filename.substr(0,filename.find_last_of("."));
				l_cmd += ".bin";
				//system("read -rsp $'Moving BUFR...\n'");
				system(l_cmd.c_str()); //Move BUFR file
				l_cmd = "mv '" + outfolder + "/" + filename+ "' '" + outfolder + "/" + year + "/" + idx + "/" + month + "/" + day + "/" + filename + "." + type_prefix[msgtype-1] + "'";
				system(l_cmd.c_str()); //Move CBUFR result
				result=true;
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

void MessagesProcessor::saveIUKIUSMessages(string outfolder)
{
	//printf("Saving messages...\n");
	std::set<char> delims{'\\','/'};
	for (size_t i = 0; i != msg_data.size(); i++)
    {
		//printf("Checking index number %s\n",msg_index[i].c_str());
	int msgtype = getIUKIUS(msg_index[i],msg_data[i]); //0 1 2
        if (msgtype)//checkIUKIUS(msg_index[i],msg_data[i]))
        {
			string fn = splitpath(msg_cfnm[i],delims).back();
			string extt = fn.substr(0,fn.find_last_of("."));
			string year = fn.substr(0,4); //�� ����� ��������� ������ ���.
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
				//������� ���� *.prn
				bool noresave = processPRNFile(year,outfolder,msgtype); //���� �� ������ �� ������������

				if (!noresave)
				{
					fileName = outfolder + "/" + extt + "_" + msg_index[i] + "_" + type_prefix[msgtype-1] + "_.bin";
					write.open(fileName.c_str(), ios::out | ios::binary);
					for (size_t j = 0; j != msg_data[i].length(); j++)
					{
						write.put(msg_data[i][j]);
					}
					write.close();
					printf("CDENY:%s\n",fileName.c_str());
				}
			}
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



