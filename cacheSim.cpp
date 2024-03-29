/* 046267 Computer Architecture - Winter 20/21 - HW #2 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "cacheSim.h"

using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

int main(int argc, char **argv) {

	if (argc < 19) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// Get input arguments

	// File
	// Assuming it is the first argument
	char* fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}

	/* initialize the Memory Data-Type: 2 level cache and main */
	Memory memory(L1Assoc,L1Size,L1Cyc,L2Assoc,L2Size,L2Cyc,WrAlloc,BSize,MemCyc);
	while (getline(file, line)) {

		/* count the acsses to the memory */
		++memory.acessNum;

		stringstream ss(line);
		string address_str;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address_str)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

		/*					unComment this line if the benchmark's trace is required:			*/
		//cout << memory.acessNum << ") operation: " << operation << endl;

		string cutAddress = address_str.substr(2); // Removing the "0x" part of the address

		/*					unComment these 2 lines if the benchmark trace is required:			*/
		//cout << ", address (hex)" << cutAddress << endl;
		//cout << endl;

		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);
		unsigned long int address = num;
		
		/******************************** MEMORY_HANDLE_STARTS **************************************/
		/*******************************************************************************************/

		
		++memory.L1.acssesNum;
		memory.totalTime += memory.L1.cyclesNum;
		//--------------------------------------
		if( memory.L1.containsBlockOf(address) ){
			memory.L1_Hit(address,operation);
			continue;
		}
        else{ ++memory.L1.missNum;}
	

		++memory.L2.acssesNum;
       	memory.totalTime += memory.L2.cyclesNum;
		//--------------------------------------
		if(  memory.L2.containsBlockOf(address) ){
			memory.L2_Hit(address,operation);
			continue;
		}
		else{++memory.L2.missNum;}


		memory.totalTime += memory.cyclesNum;
		//--------------------------------------
		memory.L1_and_L2_Miss(address,operation);
		continue;

		/*******************************************************************************************/
		/******************************** MEMORY_HANDLE_ENDS **************************************/


		// DEBUG - remove this line
		cout << " (dec) " << num << endl;

	}

	double L1MissRate = (double)memory.L1.missNum/memory.L1.acssesNum;
	double L2MissRate = (double)memory.L2.missNum/memory.L2.acssesNum;
	double avgAccTime = (double)memory.totalTime/memory.acessNum;

	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
}
