// m is the provided value in configure.txt
// 2-bit saturating counters indexed using m LSBs of each branch instruction
// The largest value of m is 32 (for 32 bit PCs)
// m value varying from 10 to 20

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <math.h>
#include <cstdlib>

using namespace std;

struct config{
    int mValue;
};

class GBHRegister{

};

int main(int argc, char* argv[]){
    config config;
    ifstream branchParams;
    branchParams.open(argv[1]);
    while(!branchParams.eof()){ // read config file
        branchParams >> config.mValue;
    }

    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    traces.open(argv[2]);
    tracesout.open(outname.c_str());

    string line, addrHexString, addrBinString, actualStatus, predictedStatus;
    int entryVal; 
    unsigned int addr;
    bitset<32> addrBit;
    int accessIndex, m, height;
    // build and initialize the 2-bit saturating counter using a 1-D array
    m = config.mValue;
    height = pow(2, m);
    // cout << "height: " <<height << "\n"; 
    int counterArray[height];
    // cout << "array size: " << sizeof(counterArray) << "\n"; 
    // the initial value in each entry should be "11" (Strong Taken)
    for(int i=0; i<height; i++){
        counterArray[i] = 11;
    }

    if(traces.is_open() && tracesout.is_open()){ //read branch access file
        while(getline (traces,line)){
            istringstream iss(line);
            if(!(iss >> addrHexString >> actualStatus)){break;}
            addrHexString = "0x" + addrHexString;
            stringstream saddr(addrHexString);
            saddr >> std::hex >> addr;
            addrBit = bitset<32> (addr);
            addrBinString = addrBit.to_string();

            //when access the address
            bitset<32> accessIndexBit(addrBinString.substr(32-m, m));
            accessIndex = accessIndexBit.to_ulong();
            // read the current value in the corresponding entry
            entryVal = counterArray[accessIndex];

            if(entryVal == 11){
                predictedStatus = "1";
                if(actualStatus == "1"){
                    counterArray[accessIndex] = 11;
                }else if(actualStatus == "0"){
                    counterArray[accessIndex] = 10;
                }

            }else if(entryVal == 10){
                predictedStatus = "1";
                if(actualStatus == "1"){
                    counterArray[accessIndex] = 11;
                }else if(actualStatus == "0"){
                    counterArray[accessIndex] = 0;
                }

            }else if(entryVal == 1){
                predictedStatus = "0";
                if(actualStatus == "1"){
                    counterArray[accessIndex] = 11;
                }else if(actualStatus == "0"){
                    counterArray[accessIndex] = 0;
                }

            }else if(entryVal == 0){
                predictedStatus = "0";
                if(actualStatus == "1"){
                    counterArray[accessIndex] = 1;
                }else if(actualStatus == "0"){
                    counterArray[accessIndex] = 0;
                }
            }

            tracesout << predictedStatus << endl;  // Output predicted results to the output file;

        }
        traces.close();
        tracesout.close();
    }else{
        cout<< "Unable to open trace or traceout file! ";
    }
    return 0;
}
