#pragma once
#include <array>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>


typedef double time, value;

//this class loads and contains any time serise within a text file as specified on the problem
class TimeSeriesData {
private:
	
	int m_jCycles, m_iSamples;
	std::vector<std::pair<time, value>> m_Timeserise;
	std::string m_fileName;
	int totalNumOfSamples = 0;
public:
	TimeSeriesData(int jCycles, int iSamples) : m_jCycles(jCycles), m_iSamples(iSamples) //Construcrtor
	{}
	TimeSeriesData() : m_jCycles(0), m_iSamples(0)//default Construcrtor
	{}

	
	/*sets the number of cycles and samples 
	per cycles of the time serise data*/
	void setCycleandSampleNumbers();
	
	
	void setNumOfCycles(int a) {m_jCycles = a;}
	void setNumOfSamplePerCycle(int a) { m_iSamples = a; }
	int getTotalNumOfSamples() { return m_Timeserise.size(); }
	
	/*sets the file name for the timeseries fileName.txt*/
	void setFilename();

	int getNumberOfCycles() const{ return m_jCycles; }
	int getNumberOfSamplesPerCycle() const { return m_iSamples; }
	int getTimeSeriesSize() const { return m_Timeserise.size(); }
	std::string getFileName() { return m_fileName; }
	
	
	double getDelta_t();

	/*retruns nth pair of the loaded time serise*/
	std::pair<time, value> getNthPairOfTimeseries(int n) const {return m_Timeserise.at(n);}

	/*retruns a range (startingIndex to endingIndex) of pair of the loaded time serise*/
	std::vector<std::pair<time, value>> getTimeseriesfromNtoM(int startingIndex, int endingIndex) const;
	
	/*adds new time and corrsponding value to the timeserise*/
	void addTimeseriesPair(double time, double value)
	{m_Timeserise.push_back(std::make_pair(time, value));}
	
	/*retruns entire loaded time serise*/
	std::vector<std::pair<time, value>> returnTimeSerise() const
	{return m_Timeserise;}

	/*Imports data from text file*/
	void importDatafromFile();

	/*Exports data to text file*/
	void exportDataToFile();

	//de
	void PrintTimeSeries();
	
};

double TimeSeriesData::getDelta_t()
{
	std::pair<time, value> starttime = m_Timeserise.at(0);
	std::pair<time, value> endtime = m_Timeserise.at(m_Timeserise.size()-1);
	double totalTime = endtime.first - starttime.first;
	double out = (totalTime) / ((double)m_Timeserise.size()-1);
	return out;
}

/*sets the number of cycles and samples
	per cycles of the time serise data*/
void TimeSeriesData::setCycleandSampleNumbers()
{
	std::cout << "Enter the number of Cycles in Data:\n ";
	
	std::cin >> m_iSamples;
	std::cout << "Enter the number of Samples per Cycle in Data:\n ";
	
	std::cin >> m_iSamples;
}


/*sets the file name for the timeseries fileName.txt*/
void TimeSeriesData::setFilename()
{
	std::cout << "Enter the name and .ext of the Data file:\n ";
	std::cin >> m_fileName;

}

/*retruns a range (startingIndex to endingIndex) of pair of the loaded time serise*/
std::vector<std::pair<time, value>> TimeSeriesData::getTimeseriesfromNtoM
								(int startingIndex, int endingIndex) const
{
	std::vector<std::pair<time, value>> output;
	for (int i = startingIndex; i <= endingIndex; i++)
	{
		output.push_back(m_Timeserise.at(i));
	}
	return output;
}

/*Imports data from text file*/
void TimeSeriesData::importDatafromFile()
{
	
	std::ifstream fs;
	fs.open(m_fileName);
	time a = 0;
	value b = 0;
	std::pair <time, value> timeValuePair;

	if (!fs)
	{
		std::cout << "Unable to open file\n";
	}
	else
	{
		std::string str;
		std::getline(fs, str);
		while (fs >> a >> b) {

			timeValuePair = std::make_pair(a, b);
			m_Timeserise.push_back(timeValuePair);
		}

	}
	fs.close();
	std::cout << "# of Time-Value loaded: " << m_Timeserise.size() << std::endl;
}

/*Exports data to text file*/
void TimeSeriesData::exportDataToFile()
{
	setFilename();
	std::ofstream fs;
	fs.open(m_fileName);
	time a = 0;
	value b = 0;
	std::pair<time, value> temppair(0, 0);
	std::vector<std::pair<time, value>>::iterator it;
	if (!fs)
	{
		std::cout << "Unable to open file to write\n";
	}
	else
	{
		fs << "#Time Series\n";
		for (it = m_Timeserise.begin(); it != m_Timeserise.end(); it++)
		{
			temppair = *it;
			fs << temppair.first << "  " << temppair.second << std::endl;
		}

	}
	fs.close();
	std::cout << "# of Time-Value prited to file: " << m_Timeserise.size() << std::endl;
}
//Debug tool
void TimeSeriesData::PrintTimeSeries()
{
	for (int i = 0; i < (int)m_Timeserise.size(); i++)
	{
		std::pair<time, value> curPair = m_Timeserise.at(i);
		std::cout << curPair.first<< " "<<curPair.second << std::endl;
	}
	
}

