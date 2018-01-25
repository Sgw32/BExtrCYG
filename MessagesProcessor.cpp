#include "MessagesProcessor.h"

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
	printf("Point6_RAB%s\n",path.c_str());
	std::ifstream file(path.c_str());
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	printf("Point6_F%s\n",content.c_str());
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
	printf("MSG_%s\n",cfnm.c_str());
	msg_data.push_back(msg);
	msg_index.push_back(index);
	msg_cfnm.push_back(cfnm);
	printf("Ok.\n");
}

void MessagesProcessor::processMessages(std::string inputFolder)
{
	printf("Point2\n");
	mFolder = inputFolder;
	cout << "inputFolder "<<mFolder;
	getdir(inputFolder);
	printf("Point3\n");
	findMSGFiles();
	printf("Point5\n");
	string cfnm = "";
	for (auto filename=messages.begin();filename!=messages.end();filename++)
    {
		printf("Point6_start\n");
        curFileName = *filename;
		cfnm = curFileName;
        vector<char> fileBytes = CharReadAllBytes(curFileName);
		printf("Point6_OK\n");
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
		printf("Point6_OK2\n");
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
						printf("Point6_FILESIZE\n");
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
            addMessage(message, idx,cfnm);
        }
    }
}

void MessagesProcessor::findMSGFiles()
{
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );

	char date[8];
	sprintf(date,"%04d%02d%02d",now->tm_year+1900,now->tm_mon,now->tm_mday);
	string grep(date);
	cout << "Scan is for " << grep <<endl;

	for (vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		string filename = *it;
		string date2 = filename.substr(0,8);
		cout << "File is " << date2 <<endl;
		if ((date2==grep)||(!dayScan))
		{
			if (filename.substr(filename.find_last_of(".") + 1)=="msg")
			{
				cout << mFolder+filename << endl;
				messages.push_back(mFolder+filename);
			}
		}
	}
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
			printf("Header OK...\n");
            return true;
		}
    }
	printf("Header%s Index%s...\n",header.c_str(),index.c_str());
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
	printf("Saving messages...\n");
	std::set<char> delims{'\\','/'};
	for (size_t i = 0; i != msg_data.size(); i++)
    {
		printf("Checking index number %s\n",msg_index[i].c_str());
        if (checkIUKIUS(msg_index[i],msg_data[i]))
        {
			string fn = splitpath(msg_cfnm[i],delims).back();
			string extt = fn.substr(0,fn.find_last_of("."));
            string fileName = outfolder + "/" + extt + "_" + msg_index[i] + ".bin";
            ofstream write;
			write.open(fileName.c_str(), ios::out | ios::binary);
			for (size_t j = 0; j != msg_data[i].length(); j++)
			{
				write.put(msg_data[i][j]);
			}
			write.close();
			printf("filename:%s\n",fileName.c_str());
		}
		printf("Check ok.\n");
    }
	printf("Save ok.\n");
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



