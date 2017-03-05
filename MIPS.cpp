#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but
//for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.


class RF
{
    public:
        bitset<32> ReadData1, ReadData2;
     	RF()
    	{
          Registers.resize(32);
          Registers[0] = bitset<32> (0);
        }

        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {

            if ( WrtEnable == bitset<1>(1) ){
                //convert binary addresses to decimal
                int decWrtReg = WrtReg.to_ulong();
                Registers[decWrtReg] = WrtData;
        	}
            else{
                //convert binary addresses to decimal
                int decRdReg1 = RdReg1.to_ulong();
                int decRdReg2 = RdReg2.to_ulong();

                //fetch the values from Registers and pass them to ReadData
                ReadData1 = Registers[decRdReg1];
                ReadData2 = Registers[decRdReg2];
            }
         }

	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {
                        rfout << Registers[j]<<endl;
                      }

                  }
                  else cout<<"Unable to open file";
                  rfout.close();

               }
	private:
            vector<bitset<32> >Registers;

};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {  //convert ALU Op Code to int for the ease of use
                int opCode = ALUOP.to_ulong();

                if(opCode == ADDU){
                    //addu
                    unsigned long sumVal = oprand1.to_ulong() + oprand2.to_ulong();

                    ALUresult = bitset<32>(sumVal);
                }
                else if(opCode == SUBU){
                    //subu
                    unsigned long diffVal = oprand1.to_ulong() - oprand2.to_ulong();
                    ALUresult = bitset<32>(diffVal);
                }
                else if(opCode == AND){
                    //and
                    ALUresult = oprand1 & oprand2;
                }
                else if(opCode == OR){
                    //or
                    ALUresult = oprand1 | oprand2;
                }
                else if(opCode == NOR){
                    //nor
                    ALUresult = oprand1 | oprand2;
                    ALUresult = ~ALUresult;
                }

                 return ALUresult;
               }
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem()
          {       IMem.resize(MemSize);
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {
                        IMem[i] = bitset<8>(line);
                        i++;
                     }
                  }
                  else cout<<"Unable to open file";
                  imem.close();

                  }

          bitset<32> ReadMemory (bitset<32> ReadAddress)
              {
               //Read the byte at the ReadAddress and the following 3 byte
              	int addrVal = ReadAddress.to_ulong();
              	string inst1 = IMem[addrVal].to_string();
              	string inst2 = IMem[addrVal + 1].to_string();
              	string inst3 = IMem[addrVal + 2].to_string();
              	string inst4 = IMem[addrVal + 3].to_string();

              	//concatenate them into one "32-bit" string
              	string inst = inst1 + inst2 + inst3 + inst4;

              	//convert string instruction to bitset
              	bitset<32> Instruction(inst);
              	return Instruction;
              }

      private:
           vector<bitset<8> > IMem;

};

class DataMem
{
      public:
          bitset<32> readdata;
          DataMem()
          {
             DMem.resize(MemSize);
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();

          }
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
          {
            bitset<32> finalresult;
            int addressVal =  (int)(Address.to_ulong());

            // if writemem and readmen are both 1, then we should exexute write first.
            if ( writemem == bitset<1>(1) ){ //When write it enabled
                string dataToWrite = WriteData.to_string();
                string str0 = dataToWrite.substr(0, 8);
                string str1 = dataToWrite.substr(8, 8);
                string str2 = dataToWrite.substr(16, 8);
                string str3 = dataToWrite.substr(24, 8);

                DMem[addressVal] = bitset<8>(str0);
                DMem[addressVal + 1] = bitset<8>(str1);
                DMem[addressVal + 2] = bitset<8>(str2);
                DMem[addressVal + 3] = bitset<8>(str3);

            }

            if ( readmem == bitset<1>(1) ){ //When write it enabled
                string result0 = DMem[addressVal].to_string();
                string result1 = DMem[addressVal + 1].to_string();
                string result2 = DMem[addressVal + 2].to_string();
                string result3 = DMem[addressVal + 3].to_string();
                string result = result0 + result1 + result2 + result3;

                finalresult = bitset<32>(result);

                readdata = finalresult;
            }

            return readdata;
          }

          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {
                        dmemout << DMem[j]<<endl;
                       }

                  }
                  else cout<<"Unable to open file";
                  dmemout.close();

               }

      private:
           vector<bitset<8> > DMem;

};



int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    bitset<32> PC(0); //initialize PC to zero
    bitset<32> instruction;

    while (1)
	{
        // Fetch one instruction from InsMem
        int PCVal = PC.to_ulong();
        instruction = myInsMem.ReadMemory(PCVal);
        string instStr = instruction.to_string(); //convert current instruction to string
		    // If current insturciton is "11111111111111111111111111111111", then break;
        if(instStr == "11111111111111111111111111111111"){ break; }
		    // decode(Read RF)
        bitset<5> readReg1(instStr.substr(6, 5)); //Rd Reg1 <== Intruc[25:21]
        bitset<5> readReg2(instStr.substr(11, 5)); //Rd Reg1 <== Intruc[20:16]
        bitset<5> writeReg; // wrt reg <== Instruc[15:11]
        bitset<3> ALUop;
        bitset<32> NextPC;
        bitset<1> writeEnable; //should be determined by IsStore | IsBranch | J-Type

        bitset<32> emptyData, preALUdown;
        string signExtendedVal;
        string toDecoder = instStr.substr(0, 6); // Intruc[31:26]

        // Read RF ***
        myRF.ReadWrite(readReg1, readReg2, writeReg, emptyData, bitset<1>(0));
        //***

        bool isStore, isLoad, isBranch, isJType, isIType, isEqual;

        // Decoder
        if(toDecoder == "100011"){ isLoad = true; } else{isLoad = false; }
        if(toDecoder == "101011"){ isStore = true; } else{isStore = false; }

        if( (toDecoder != "000000") && (toDecoder != "000010") ){ isIType = true; } else{isIType = false; }


        if( (toDecoder == "100011") || (toDecoder == "101011") ){
            ALUop = bitset<3>(string("001"));
        }
        else{
            ALUop = bitset<3>( instStr.substr(29, 3) );
        }

        if( toDecoder == "000010" ){
            isJType = true;
        }
        else {
            isJType = false; }

        if( toDecoder == "000100" ){
            isBranch = true;
        }
        else {
            isBranch = false; }

        if(isStore || isBranch || isJType){
            writeEnable = bitset<1>(0);
        }
        else{
            writeEnable = bitset<1>(1);
        }

        if(myRF.ReadData1 == myRF.ReadData2){ isEqual = true;  } else{isEqual = false;}

        //Adder 1 (after PC)
        int PCincVal = PCVal + 4; //Adder 1 Output

        bitset<32> PCinc(PCincVal); //convert Adder 1's output to 32-bit

        string PCincStr = PCinc.to_string();

        //Concat

        string concatStr = PCincStr.substr(0, 4) + instStr.substr(6, 26) + "00";

        bitset<32> concat(concatStr);
        //sign extendsion for Intruc[15:0]
        string preExtVal = instStr.substr(16, 16);

        string temp = preExtVal.substr(0, 1); //<== Intruc[15]

        for (int i = 0; i < 16; i++){
            signExtendedVal = temp + preExtVal;
        }


        // the Mux after Read Data 2
        if(isIType){
            preALUdown = bitset<32>(signExtendedVal);
        }
        else{
            preALUdown = myRF.ReadData2;
        }


        //Adder 2 (after sign extend)
        string secondAdderInputStr_2 = signExtendedVal.substr(2, 30) + "00";

        bitset<32> secondAdderInput_2(secondAdderInputStr_2);

        int secondAdderInputVal_2 = secondAdderInput_2.to_ulong();

        int secondAdderOutVal = PCincVal + secondAdderInputVal_2;

        bitset<32> secondAdderOut(secondAdderOutVal);

        // NextPC?    3:1 Mux for J-type
        if (isBranch && isEqual){
            NextPC = secondAdderOut;
        }
        else if(isJType){
            NextPC = concat;
        }
        else{
            NextPC = PCinc;
        }
        PC = NextPC;


        // the Mux before Wrt Reg
        if(isIType){
            writeReg = bitset<5>( (instStr.substr(11, 5)) ); // <== Intruc[20:16]
        }
        else{
            writeReg = bitset<5>( ( instStr.substr(16, 5)) );// <== Intruc[15:11]
        }


		// Execute
		// Call ALU!
        myALU.ALUOperation(ALUop, myRF.ReadData1, preALUdown);

        //determine whether need to read mem
        bitset<1> readMem;
        if(isLoad){ //which should be different from the ppt -10/21/2016 -1am
            readMem = bitset<1>(1);
        }
        else{
            readMem = bitset<1>(0);
        }

        //Data Memory
        // determine whether need to write mem
        bitset<1> writeMem;
        if(isStore){
            writeMem = bitset<1>(1);
        }
        else{
            writeMem = bitset<1>(0);
        }
		// Read/Write Mem
		bitset<32> readData; //the output
		readData = myDataMem.MemoryAccess(myALU.ALUresult, myRF.ReadData2, readMem, writeMem);

        //the mux after ALU and DataMem
        bitset<32> outputDataMemALU;
        if(isLoad){
            outputDataMemALU = readData;
        }
        else{
            outputDataMemALU = myALU.ALUresult;

        }

		// Write back to RF
        myRF.ReadWrite(readReg1, readReg2, writeReg, outputDataMemALU, writeEnable );

        myRF.OutputRF(); // dump RF;
    }
        myDataMem.OutputDataMem(); // dump data mem

        return 0;

}
