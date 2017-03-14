/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <math.h>  /* log2 */


using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss

struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };

class CacheBlock{
    public:
        string tag;
        string data;
        bool valid; //valid bit
        bool dirty; //dirty bit

    void writeBlock(string str1, string str2, bool v, bool d){
        tag = str1;
        data = str2;
        valid = v;
        dirty = d;
    }

    CacheBlock(){
        valid = false;
        dirty = false;
    }

    string getTag(){
        return tag;
    }
};

int main(int argc, char* argv[]){

    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L2blocksize;
      cache_params>>cacheconfig.L2setsize;
      cache_params>>cacheconfig.L2size;
      }

   // Implement by you:
   // initialize the hirearch cache system with those configs
   // probably you may define a Cache class for L1 and L2, or any data structure you like

    int L1AcceState = 0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState = 0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;

    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";

    traces.open(argv[2]);
    tracesout.open(outname.c_str());

    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;

    string baddr; //the address from the memory trace store in binary;
    //define Cache here

    int L1IndexSize = (cacheconfig.L1size*1024) / (cacheconfig.L1setsize * cacheconfig.L1blocksize);
    int L2IndexSize = (cacheconfig.L2size*1024) / (cacheconfig.L2setsize * cacheconfig.L2blocksize);

    CacheBlock l1[L1IndexSize][cacheconfig.L1setsize]; //L1 Cache
    CacheBlock l2[L2IndexSize][cacheconfig.L2setsize]; //L2 Cache

    int L1OffsetLength = log2(cacheconfig.L1blocksize);
    int L2OffsetLength = log2(cacheconfig.L2blocksize);

    int L1IndexLength = log2(L1IndexSize);
    int L2IndexLength = log2(L2IndexSize);

    int L1TagLength = 32 - L1OffsetLength - L1IndexLength;
    int L2TagLength = 32 - L2OffsetLength - L2IndexLength;

    int counter1[L1IndexSize]; //counter for every cache set of L1
    int counter2[L2IndexSize]; //counter for every cache set of L2
    
    //initalize counter array
    for(int i = 0; i < L1IndexSize; i++){
        counter1[i] = 0;
    }
    for(int i = 0; i < L2IndexSize; i++){
        counter2[i] = 0;
    }

    string tag_temp_1, index_temp_1, offset_temp_1;
    string tag_temp_2, index_temp_2, offset_temp_2;

    bitset<32> index_temp_bit_1;
    bitset<32> index_temp_bit_2;

    int index_temp_int_1, index_temp_int_2;

    string ex_tag, ex_index, ex_offset, ex_addr; // for temperoray storage of evited tag, index, and offset

    int checknum = 0;
    if(traces.is_open() && tracesout.is_open()){
        while(getline (traces,line)){   // read mem access file and access Cache
            L1AcceState = 0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
            L2AcceState = 0;
            istringstream iss(line);
            if(!(iss >> accesstype >> xaddr)){break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);

            //convert bitset address to binary string address
            baddr = accessaddr.to_string();

           // access the L1 and L2 Cache according to the trace;
            if(accesstype.compare("R")==0){
                 //Implement by you:
                 // read access to the L1 Cache,
                 //  and then L2 (if required),
                 //  update the L1 and L2 access state variable;

                //read L1
                tag_temp_1 = baddr.substr(0, L1TagLength);
                index_temp_1 = baddr.substr(L1TagLength, L1IndexLength);
                offset_temp_1 = baddr.substr( (L1TagLength + L1IndexLength), L1OffsetLength);

                index_temp_bit_1 = bitset<32>(index_temp_1);
                index_temp_int_1 = index_temp_bit_1.to_ulong();

                for(int i=0; i<cacheconfig.L1setsize; i++){
                    if( tag_temp_1 == l1[index_temp_int_1][i].getTag() ){
                        L1AcceState = RH;
                        break;
                    }
                    else{
                        L1AcceState = RM;
                    }
                }

                //if L1 Read miss, then proceed to L2
                if(L1AcceState == RM){
                    tag_temp_2 = baddr.substr(0, L2TagLength);
                    index_temp_2 = baddr.substr(L2TagLength, L2IndexLength);
                    offset_temp_2 = baddr.substr( (L2TagLength + L2IndexLength), L2OffsetLength);

                    index_temp_bit_2 = bitset<32>(index_temp_2);
                    index_temp_int_2 = index_temp_bit_2.to_ulong();

                    for(int j=0; j<cacheconfig.L2setsize; j++){
                        if( (tag_temp_2 == l2[index_temp_int_2][j].tag) && l2[index_temp_int_2][j].valid ){
                            L2AcceState = RH;
                            break;
                        }
                        else{
                            L2AcceState = RM;
                        }
                    }

                    if(L2AcceState == RH){
                            if(l1[index_temp_int_1][counter1[index_temp_int_1]].valid){
                                if(l1[index_temp_int_1][counter1[index_temp_int_1]].dirty){ // if L1's block is dirty and valid
                                    // pass data to L1's block
                                    l1[index_temp_int_1][counter1[index_temp_int_1]].dirty = false; // change L1 block's dirty bit to 0
                                    l1[index_temp_int_1][counter1[index_temp_int_1]].valid = true; // change L1 block's valid bit to 1

                                    // put the L1 block's original value to L2 block
                                    ex_tag = l1[index_temp_int_1][counter1[index_temp_int_1]].tag;
                                    ex_index = index_temp_1;
                                    ex_offset = offset_temp_1;
                                    ex_addr = ex_tag + ex_index + ex_offset;

                                    //replace L1 block's tag with the first part of the access address
                                    l1[index_temp_int_1][counter1[index_temp_int_1]].tag = baddr.substr(0, L1TagLength);

                                    // use ex_addr to write L2 block
                                    // update temp values
                                    tag_temp_2 = ex_addr.substr(0, L2TagLength);

                                    index_temp_2 = ex_addr.substr(L2TagLength, L2IndexLength);

                                    offset_temp_2 = ex_addr.substr( (L2TagLength + L2IndexLength), L2OffsetLength);

                                    //convert index to bitset and int format
                                    index_temp_bit_2 = bitset<32>(index_temp_2);
                                    index_temp_int_2 = index_temp_bit_2.to_ulong();

                                    for(int i = 0; i<cacheconfig.L2setsize; i++){
                                        if(l2[index_temp_int_2][i].tag == tag_temp_2){
                                            l2[index_temp_int_2][i].dirty = true; // if tag matches, change dirty to 1, if not, go to memory
                                        }
                                    }
                                }
                                else{ //if L1's block is valid but not dirty, then use the address from L2's block to replace L1 block's tag and data w/o eviction
                                    // put the L2 block's original value to L1 block
                                    //because this block was not dirty, we just need to replace the tag with L2's tag
                                    l1[index_temp_int_1][counter1[index_temp_int_1]].tag = baddr.substr(0, L1TagLength);
                                }

                                //update counter
                                if(counter1[index_temp_int_1] < (cacheconfig.L1setsize - 1) ){
                                    counter1[index_temp_int_1]++;
                                }
                                else{
                                    counter1[index_temp_int_1] = 0;
                                }
                            }
                            else{
                                //if not valid, then just fill L1 block up with L2's data and tag
                                l1[index_temp_int_1][counter1[index_temp_int_1]].tag = baddr.substr(0, L1TagLength);
                            }
                    }


                    if(L1AcceState == RM && L2AcceState == RM){ //If read miss on both L1 and L2
                        l2[index_temp_int_2][counter2[index_temp_int_2]].dirty = false;
                        l2[index_temp_int_2][counter2[index_temp_int_2]].valid = true;

                        l2[index_temp_int_2][counter2[index_temp_int_2]].tag = baddr.substr(0, L2TagLength);

                        //update counter
                        if(counter2[index_temp_int_2] < (cacheconfig.L2setsize - 1) ){
                            counter2[index_temp_int_2]++;
                        }
                        else{
                            counter2[index_temp_int_2] = 0;
                        }

                        l1[index_temp_int_1][counter1[index_temp_int_1]].dirty = false;
                        l1[index_temp_int_1][counter1[index_temp_int_1]].valid = true;

                        l1[index_temp_int_1][counter1[index_temp_int_1]].tag = baddr.substr(0, L1TagLength);
                        //update counter
                        if(counter1[index_temp_int_1] < (cacheconfig.L1setsize - 1) ){
                            counter1[index_temp_int_1]++;
                        }
                        else{
                            counter1[index_temp_int_1] = 0;
                        }
                    }
                }
             }
             else{
                   //Implement by you:
                  // write access to the L1 Cache,
                  //and then L2 (if required),
                  //update the L1 and L2 access state variable;
                tag_temp_1 = baddr.substr(0, L1TagLength);
                index_temp_1 = baddr.substr(L1TagLength, L1IndexLength);
                offset_temp_1 = baddr.substr( (L1TagLength + L1IndexLength), L1OffsetLength);

                index_temp_bit_1 = bitset<32>(index_temp_1);
                index_temp_int_1 = index_temp_bit_1.to_ulong();

                tag_temp_2 = baddr.substr(0, L2TagLength);
                index_temp_2 = baddr.substr(L2TagLength, L2IndexLength);
                offset_temp_2 = baddr.substr( (L2TagLength + L2IndexLength), L2OffsetLength);

                index_temp_bit_2 = bitset<32>(index_temp_2);
                index_temp_int_2 = index_temp_bit_2.to_ulong();

                //write L1
                for(int i=0; i<cacheconfig.L1setsize; i++){
                    if( tag_temp_1 == l1[index_temp_int_1][i].getTag() ){
                        L1AcceState = WH;
                        l1[index_temp_int_1][i].dirty = true;
                        //l1[index_temp_int_1][i].valid = true;
                        break;
                    }
                    else{
                        L1AcceState = WM;
                    }
                }

                if(L1AcceState == WM){ // if write miss in L1, then preceed to L2
                    for(int j=0; j<cacheconfig.L2setsize; j++){
                        if( tag_temp_2 == l2[index_temp_int_2][j].getTag() ){
                            L2AcceState = WH;
                            l2[index_temp_int_2][j].dirty = true;
                            //l2[index_temp_int_2][j].valid = true;
                            break;
                        }
                        else{
                            L2AcceState = WM;
                        }
                    }
                }
            }

            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";

    return 0;
}
