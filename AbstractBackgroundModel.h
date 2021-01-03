#pragma once
#include <complex>
#include "TimeSeriesData.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <numeric>
#include "fftw3.h"//used for FFT. see FFTW.org


class AbstractBackgroundModel {
public:
	/**
	* @returns the i-th sample of the modeled background signal
	*/
	virtual double evaluateBackground(int i) const = 0;
	/**
	* Refines the model using a new cycle of the background signal
	* @param sampleData a time series representing one new cycle of a signal
	*/
	virtual void addCycle() = 0;
	/**
	* Generate a modeled background signal
	* @param nCycles number of cycles that the generated signal should contain
	* @returns a time series consisting of the modelled background cycle
	* repeated nCycles times.
	*/
	virtual TimeSeriesData createData(int nCycles, int mSamplesPerCycle) = 0;
};


class MockFittedBackgroundModel : public AbstractBackgroundModel {
private:
	const double A;
	const double f;
	const double offset;
	const double phase;
	double delta_t;
public:
	MockFittedBackgroundModel(
		double A = 0.05, double f = 1.0e3, double offset = 0.45,
		double phase = 5.0, double dt = 5e-06
	) : A(A), f(f), offset(offset), phase(phase), delta_t(dt) {}
	
	double evaluateBackground(int i) const override {
		
		return A * sin(2.0 * M_PI * f * i * delta_t + phase) + offset;
	}
	void addCycle() override {}

	TimeSeriesData createData(int nCycles, int mSamplesPerCycle)
	{
		TimeSeriesData output;
		//sets number cycles and num of samples per cycle
		output.setNumOfCycles(nCycles);
		output.setNumOfSamplePerCycle(mSamplesPerCycle);
		double curTime = 0;
		double curValue = 0;
		

		//determining total number of data points
		int numOfDatapoints = output.getNumberOfCycles() * output.getNumberOfSamplesPerCycle();
		for (int a = 0; a < numOfDatapoints; a++)
		{
			curValue = evaluateBackground(a);
			output.addTimeseriesPair(curTime, curValue);
			curTime = curTime + delta_t;
		}
		output.exportDataToFile();
		return output;
	}

};

class FittedBackgroundModel : public AbstractBackgroundModel
{
public:
	FittedBackgroundModel() {}

	double evaluateBackground(int i) const override 
	{return amplitude * cos(2.0 * M_PI * frequency * i * delta_t + phase) + offset;}

	//the function averages the imported background data of N-cycles to a single cycle
	TimeSeriesData avgOfBackgroundDataOfNCycles(TimeSeriesData backGroundData);

	/*makes a model of the background sinusodial using FFT
	and sets the values of delta_t, frequency, offset, phase, amplitude*/
	void makeModelFromAvgBackgroundData(TimeSeriesData in);

	//no need for this method as the cyles can be averaged 
	//with avgOfBackgroundDataOfNCycles() function
	void addCycle() override;
	
	//creates a time-serise(for N cycles) based on the background data model 
	TimeSeriesData createData(int nCycles, int mSamplesPerCycle);

	TimeSeriesData loadBackgroundData(int nCycles, int mSamples);

	TimeSeriesData loadBackgroundData();

	/*TimeSeriesData generateTimeSeriesFromModel(int nCycles, int mSamples);*/


private:
	
	double delta_t = 0;
	double frequency = 0;
	double offset = 0;
	double phase = 0;
	double amplitude = 0;
};

//the function averages the imported background data of N-cycles to a single cycle
TimeSeriesData FittedBackgroundModel::
			avgOfBackgroundDataOfNCycles(TimeSeriesData backGroundData)
{
	int cycles = backGroundData.getNumberOfCycles();
	int samplesPerCycle = backGroundData.getNumberOfSamplesPerCycle();
	int totalNumOfSamples = backGroundData.getTotalNumOfSamples();


	TimeSeriesData avgedBackgroundData;//output avged timeserise data
	avgedBackgroundData.setNumOfCycles(1);//output set at 1 cycle
	avgedBackgroundData.setNumOfSamplePerCycle
	(backGroundData.getNumberOfSamplesPerCycle());

	/*this portion of the code takes the j-th data
	point of each cycle (i) and averages them*/
	double tempTime = 0;
	for (int j = 0; j < samplesPerCycle; j++)
	{
		double tempSumValue = 0;
		for (int i = 0; i < cycles; i++)
		{
			std::pair<double, double> a
				= backGroundData.getNthPairOfTimeseries(j + i * 200);
			tempSumValue += a.second;
		}
		tempSumValue = tempSumValue / cycles;
		std::pair<time, value> tempPair;
		avgedBackgroundData.addTimeseriesPair(tempTime, tempSumValue);
		tempTime += backGroundData.getDelta_t();
	}

	return avgedBackgroundData;

}


void FittedBackgroundModel::
				makeModelFromAvgBackgroundData(TimeSeriesData in)
{
	delta_t = in.getDelta_t();//delta t calculated within TimeSeriesData

	/*extracts the time-value pair containing the last data point.
	the timevalue is then used to calculate the sample-freq.*/
	std::pair<time, value> tempPair = in.getNthPairOfTimeseries(in.getTimeSeriesSize() - 1);
	double sampleFrequency = ((double)in.getTimeSeriesSize() - 1) / tempPair.first;

	/*the calculation of the freq.-bin (unit Hz/bin)*/
	double delta_f = sampleFrequency / (((double)in.getTimeSeriesSize()) * 2.0);//devided by 2 due to nyquest-limit
	
	/*threshold value for the K value*/
	double threshold ;
	std::cout << "\nEnter the threshold number for K value:\n";
	std::cin >> threshold;
	std::vector<double> fftwIn(in.getTimeSeriesSize());//input vector for the FFTW
	std::vector<std::complex<double>> fftwOut(in.getTimeSeriesSize() / 2 + 1);//output vector for the FFTW

	//Using FFTW Lib. available at FFTW.org
	//FFTW plan for 1-D real to complex FFT
	fftw_plan p = fftw_plan_dft_r2c_1d(in.getTimeSeriesSize(), fftwIn.data(),
		reinterpret_cast<fftw_complex*>(fftwOut.data()), FFTW_ESTIMATE);


	/*this section parses out the value-column only
	within the time serise for fftw*/
	for (int i = 0; i < in.getTimeSeriesSize(); i++)
	{
		std::pair<double, double> tempPair2 = in.getNthPairOfTimeseries(i);
		fftwIn[i] = (tempPair2.second);
	}

	/*this calulates the offset (vertiacl shift) of the signal*/
	if (fftwIn.size() != 0)
	{
		offset = (double)(std::accumulate(fftwIn.begin(),
			fftwIn.end(), 0.0)) / (double)fftwIn.size();
	}

	fftw_execute(p);
	
	//operate on the fourier transform to find the K value larger then theshold
	for (int K = 1; K < ((int)fftwOut.size()/2); ++K) { //starts in 1 to ignore DC
		if (std::abs(fftwOut[K]) > threshold)
		{
			frequency = K * delta_f * 2;
			amplitude = std::abs(fftwOut[K]);
			phase = std::arg(fftwOut[K]);
			std::cout << "Frequency: " << frequency
				<< " Amplitude: " << amplitude << " Phase: " << phase << '\n';
		}
	}

	fftw_destroy_plan(p); //destructor 

}


//creates a time-serise(for N cycles) based on the background data model 
//and saves to a text file
TimeSeriesData FittedBackgroundModel::
				createData(int nCycles, int mSamplesPerCycle)
{
	TimeSeriesData output;
	//sets number cycles and num of samples per cycle
	output.setNumOfCycles(nCycles);
	output.setNumOfSamplePerCycle(mSamplesPerCycle);
	double curTime = 0;
	double curValue = 0;
	
	//determining total number of data points
	int numOfDatapoints = output.getNumberOfCycles() * output.getNumberOfSamplesPerCycle();
	for (int a = 0; a < numOfDatapoints; a++)
	{
		curValue = evaluateBackground(a);
		output.addTimeseriesPair(curTime, curValue);
		curTime = curTime + delta_t;
	}
	
	return output;
}

TimeSeriesData FittedBackgroundModel::
				loadBackgroundData(int nCycles, int mSamples)
{
	TimeSeriesData b1;
	b1.setNumOfCycles(nCycles);
	b1.setNumOfSamplePerCycle(mSamples);
	b1.setFilename();
	b1.importDatafromFile();
	return b1;
}

TimeSeriesData FittedBackgroundModel::loadBackgroundData()
{
	TimeSeriesData b1;
	b1.setCycleandSampleNumbers();
	b1.setFilename();
	b1.importDatafromFile();
	return b1;
}

void FittedBackgroundModel::addCycle() 
{
	TimeSeriesData newSmoothingCycle = loadBackgroundData();
	
	//if the smoothging bkground data has more than 1 cycle
	if (newSmoothingCycle.getNumberOfCycles() > 1)
	{
		newSmoothingCycle = avgOfBackgroundDataOfNCycles(newSmoothingCycle);
	}

	//generate a time series of bkground data from model
	TimeSeriesData newDataCreatedFromModel 
		= createData(1, newSmoothingCycle.getTimeSeriesSize());
	double vSmoothingCoef = 1.0;
	std::cout << "Select a smoothing parameter V between 0-1\n";
	std::cin >> vSmoothingCoef;
	
	/*itarate over all the data points and generate the
	new smoothed cycle*/
	TimeSeriesData smoothedBkGData;
	for (int i = 0; i < newSmoothingCycle.getTimeSeriesSize(); i++)
	{
		double smoothedValue;
		std::pair<time, value> curModelValue 
			= newDataCreatedFromModel.getNthPairOfTimeseries(i);
		std::pair<time, value> vSmoothingValue
			= newSmoothingCycle.getNthPairOfTimeseries(i);

		smoothedValue = (vSmoothingCoef)*vSmoothingValue.second +
			(vSmoothingCoef - 1.0) * curModelValue.second;
		smoothedBkGData.addTimeseriesPair(curModelValue.first, smoothedValue);

	}
	
	//update the background model with new smoothed cycle
	makeModelFromAvgBackgroundData(smoothedBkGData);

}

//TimeSeriesData FittedBackgroundModel::
//		generateTimeSeriesFromModel( int nCycles, int mSamples)
//{
//	TimeSeriesData out = createData(nCycles, mSamples);
//	return out;
//}
