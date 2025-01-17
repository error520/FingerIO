#include "GammaUtil.h"
using namespace std;
#include<string>
int  GammaUtil::readByLine(string& sFileName, vector<float>& vecSrc)
{
	ifstream infile;
	//string str = "D:/visual studio 2017/Projects/ButterworthFilter/ButterworthFilter/src.txt";
	infile.open(sFileName.c_str());
	if (!infile.is_open())
	{
		cout << "not open true" << endl;
		return -1;
	}

	string line;
	while (!infile.eof()) {
		std::getline(infile, line);
		float fTemp = atof(line.c_str());
		vecSrc.push_back(fTemp);
		//cout << line << endl;
	}
	infile.close();
	return vecSrc.size();
}


int  GammaUtil::readByLine(string& sFileName, float ** pData)
{
	ifstream infile;
	vector<float> vecSrc;
	//string str = "D:/visual studio 2017/Projects/ButterworthFilter/ButterworthFilter/src.txt";
	infile.open(sFileName.c_str());
	if (!infile.is_open())
	{
		cout << "not open true" << endl;
		return -1;
	}

	string line;
	while (!infile.eof()) {
		std::getline(infile, line);

		float fTemp = atof(line.c_str());
		vecSrc.push_back(fTemp);
		//cout << line << endl;
	}
	infile.close();

	float* src = new float[vecSrc.size()];
	for (int i = 0; i < vecSrc.size(); i++)
	{
		src[i] = vecSrc[i];
	}

	*pData = src; 
	return vecSrc.size();
}


void  GammaUtil::writeDataToFile(string& sFileName, Mat& imageMat, bool replace)
{	
	std::ofstream m_logfile;
	if (replace)
	{
		m_logfile.open(sFileName.c_str(), std::ios::out);
	}
	else
	{
		m_logfile.open(sFileName.c_str(), std::ios::app);
	}
	
	ostringstream streamStr;
	for (size_t i = 0; i < imageMat.rows; i++)
	{
		for (size_t j = 0; j < imageMat.cols; j++)
		{
			streamStr << imageMat.at<float>(i, j) << "|";
		}
		streamStr << endl;
	}
	m_logfile << streamStr.str() << std::flush;
	m_logfile.close();
}

void  GammaUtil::writeLineToFile(std::ofstream& m_logfile, const string& str)
{
	ostringstream streamStr;
	streamStr << str << endl;
	//m_logfile << streamStr.str() << std::flush;
}

void  GammaUtil::writeUcharDataToFile(string& sFileName, Mat& imageMat, bool replace)
{
	std::ofstream m_logfile;
	if (replace)
	{
		m_logfile.open(sFileName.c_str(), std::ios::out);
	}
	else
	{
		m_logfile.open(sFileName.c_str(), std::ios::app);
	}
	ostringstream streamStr;
	for (size_t i = 0; i < imageMat.rows; i++)
	{
		for (size_t j = 0; j < imageMat.cols; j++)
		{
			int temp = imageMat.at<uchar>(i, j);
			streamStr << temp << "|";
		}
		streamStr << endl;
	}
	m_logfile << streamStr.str() << std::flush;
	m_logfile.close();
}


void  GammaUtil::writeVecPointYToFile(string& sFileName, vector<int>& vecPointY, bool replace)
{
	std::ofstream m_logfile;
	if (replace)
	{
		m_logfile.open(sFileName.c_str(), std::ios::out);
	}
	else
	{
		m_logfile.open(sFileName.c_str(), std::ios::app);
	}
	ostringstream streamStr;
	for (size_t i = 0; i < vecPointY.size(); i++)
	{
		streamStr << vecPointY[i] << "|";
	}
	m_logfile << streamStr.str() << std::flush;
	m_logfile.close();
}

void  GammaUtil::writeVecDoublePointYToFile(string& sFileName, vector<double>& vecPointY, bool replace)
{
	std::ofstream m_logfile;
	if (replace)
	{
		m_logfile.open(sFileName.c_str(), std::ios::out);
	}
	else
	{
		m_logfile.open(sFileName.c_str(), std::ios::app);
	}
	ostringstream streamStr;
	for (size_t i = 0; i < vecPointY.size(); i++)
	{
		streamStr << vecPointY[i] << " ";
	}
	streamStr << endl;
	m_logfile << streamStr.str() << std::flush;
	m_logfile.close();
}

void  GammaUtil::writePointYToFile(string& sFileName, float *pfPointY, int iSize, bool replace)
{
	std::ofstream m_logfile;
	if (replace)
	{
		m_logfile.open(sFileName.c_str(), std::ios::out);
	}
	else
	{
		m_logfile.open(sFileName.c_str(), std::ios::app);
	}
	ostringstream streamStr;
	for (size_t i = 0; i < iSize; i++)
	{
		streamStr << pfPointY[i] << endl;
	}
	m_logfile << streamStr.str() << std::flush;
	m_logfile.close();
}


void  GammaUtil::writeVecPointYToFile(string& sFileName, vector<float>& vecPointY, bool replace)
{
	std::ofstream m_logfile;
	if (replace)
	{
		m_logfile.open(sFileName.c_str(), std::ios::out);
	}
	else
	{
		m_logfile.open(sFileName.c_str(), std::ios::app);
	}
	ostringstream streamStr;
	for (size_t i = 0; i < vecPointY.size(); i++)
	{
		streamStr << vecPointY[i] << endl;
	}
	m_logfile << streamStr.str() << std::flush;
	m_logfile.close();
}


void  GammaUtil::lessThenSetValue(vector<float>& vData, float fData)
{
	for (size_t i = 0; i < vData.size(); i++)
	{
		if (abs(vData[i] - fData) < 5)
		{
			vData[i] = fData;
		}

		//remove the last frame
		if (i ==(vData.size() -1))
		{
			vData.erase(vData.begin() + i);
		}
	}

}

void  GammaUtil::finishOneAction(vector<float>& vAcc, int iWatingTime, int& iType, int& iWord, vector<float>& vfShift)
{
	if (vAcc.size() >= iWatingTime)
	{
		bool bEnd = true;
		for (size_t i = 0; i < vAcc.size(); i++)
		{

			if (vAcc[i] > 15)
			{
				bEnd = false;
				break;
			}
		}

		if (bEnd)
		{
			iWord = iType;		
			vfShift.erase(vfShift.begin(), vfShift.begin() + iWatingTime+1);	
		}
	}
}


void  GammaUtil::finishOneActionV3(vector<float>& vAcc, int iWatingTime, int& iType, int& iWord, vector<float>& vfShift, int iMark)
{
	if (vAcc.size() >= iWatingTime && iMark == 0)
	{	
		vfShift.clear()	;		
	}
}

void  GammaUtil::cleanData(int iType, SegInfo& stSegInfo, vector<float>& vfShift, vector<float>& vAcc)
{	
	/*string fileName = "/storage/emulated/0/data/files/phase/cleandata.txt";
	ostringstream streamStr;
	streamStr << vSegX.size() << "|" << vfShift.size() << "|" << vAcc.size() << "|" << endl;
	streamStr << "vSegX:";
	for (int i = 0; i< vSegX.size(); i++)
	{
		streamStr << vSegX[i] << "|";
	}
	streamStr << endl;
	writeLineToFile(fileName, streamStr.str(), false);*/

	vfShift.erase(vfShift.begin(),(vfShift.begin()+ stSegInfo.iEnd+1));
	vAcc.erase(vAcc.begin(), vAcc.begin() + stSegInfo.iEnd +1);
	stSegInfo.iEnd = 0;
	stSegInfo.iStart= 0;
}

void  GammaUtil::writeLineToFile(const string& sFileName, const string& str, bool replace)
{
	std::ofstream m_logfile;
	if (replace)
	{
		m_logfile.open(sFileName.c_str(), std::ios::out);
	}
	else
	{
		m_logfile.open(sFileName.c_str(), std::ios::app);
	}

	ostringstream streamStr;
	streamStr << str << endl;
	m_logfile << streamStr.str() << std::flush;
	m_logfile.close();
}
