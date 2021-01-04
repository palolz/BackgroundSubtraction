

#include <iostream>
#include <array>
#include <fstream>
#include "TimeSeriesData.h"
#include "AbstractBackgroundModel.h"
#include "interActingModel.h"


int main()
{   
    int a = 0;
       
    std::cout << "***Please make sure the background data***\n";
    std::cout << "***and the signal has same delta_t***\n";
    std::cout << "Now loading the Raw-Background-Data\n";
    
    FittedBackgroundModel fittedModelOfBkgrndData;
    TimeSeriesData bkgroungdata;
    TimeSeriesData mockBackgroundData;
    MockFittedBackgroundModel mockBKGData;
    InterActingModel signal;

    TimeSeriesData rawBackGroundData = fittedModelOfBkgrndData.loadBackgroundData();
    fittedModelOfBkgrndData.makeModelFromAvgBackgroundData(rawBackGroundData);
    
    std::cout << "A model has been created from the\n";
    std::cout << "Backgrounf data.\n\n";

    while (a == 2||a==0)
    {
        std::cout << "Enter the corrsponding number of your choice:\n\n";
        std::cout << "1. Load Interacting data and subtract it\n";
        std::cout << "   from the background data.\n\n";
        std::cout << "2. Update/refine the loaded background data with \n";
        std::cout << "   new cycle of background data.\n\n";
        std::cout << "3. Use the MockBackGroundModel to Subtract from \n";
        std::cout << "   interacting signal.\n\n";
        std::cout << "ENTER ANY OTHER NUMBER TO EXIT.\n";
        std::cin >> a;


        switch (a)
        {
        case 1:
            bkgroungdata = fittedModelOfBkgrndData.createData(50, 200);
            signal.loadInteractingDataAndSubFromBkgrougData(bkgroungdata);
            break;
        case 2:
            //load new cycle, gen new data from model(1 cycle), avg accroding to smoothing
            //co-eff "v"
            std::cout << "Now Loading the smoothing cycle of background data\n";
            fittedModelOfBkgrndData.addCycle();

            break;
        case 3:
            //gen new data from mock background model.
            //Load Interacting data and subtract it
            //from the background data.
            std::cout << "NOW LOADING MOCK BACKGROUND DATA\n";
            mockBackgroundData = mockBKGData.createData(1,200);
            signal.loadInteractingDataAndSubFromBkgrougData(mockBackgroundData);

            break;
        default:
            return 0;
        }
    }
            
     std::cin.get();
}

