#pragma once
#include <complex>
#include "TimeSeriesData.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <numeric>

class InterActingModel
{
private:
	TimeSeriesData m_interactingModel;
	TimeSeriesData m_subtractedModel;

public:
	InterActingModel()
	{
		
	}

	/*this fuction substracts the background data from the 
	"interacting" time series data, and then saves the resultant 
	time series on to a text file.*/
	void subtractBackgroundAndSaveData(TimeSeriesData background);

	void loadInteractingDataAndSubFromBkgrougData(TimeSeriesData bkgroungdata);

};

/*this fuction substracts the background data from the
	"interacting" time series data, and then saves the resultant
	time series on to a text file.*/
void InterActingModel::subtractBackgroundAndSaveData(TimeSeriesData background) 
{
	/*checks to see if the background data and the =interacting data 
	have the same size/#-of-Data-Points*/
	if (background.getTimeSeriesSize() != m_interactingModel.getTimeSeriesSize())
	{
		std::cout << "the number of data point des not match between models\n";
		return;
	}

	for (int i = 0; i < m_interactingModel.getTimeSeriesSize(); i++)
	{
		std::pair<time, value> bkPair = background.getNthPairOfTimeseries(i);
		std::pair<time, value> signalPair = m_interactingModel.getNthPairOfTimeseries(i);
		std::pair<time, value> subtractedPair;
		subtractedPair.first = signalPair.first;
		subtractedPair.second = bkPair.second - signalPair.second;
		m_subtractedModel.addTimeseriesPair(subtractedPair.first, subtractedPair.second);
	}

	std::cout << "Now exporting the subtracted data to file\n";
	m_subtractedModel.exportDataToFile();//saves the time serise to file
}


void InterActingModel::loadInteractingDataAndSubFromBkgrougData(TimeSeriesData bkgroungdata)
{
	m_interactingModel.setCycleandSampleNumbers();
	std::cout << "Now loading the interacting data\n";
	m_interactingModel.setFilename();
	m_interactingModel.importDatafromFile();
	subtractBackgroundAndSaveData(bkgroungdata);
	std::cout << "the subtracted value saved to file: "
		<< m_subtractedModel.getFileName();
}