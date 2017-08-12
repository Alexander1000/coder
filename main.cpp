#include <iostream>
#include <fstream>
#include <ctime>
#include "cl.cpp"
#include "coder.h"

using namespace std;
using namespace coder;

#define ACTION_ENCODE 1
#define ACTION_DECODE 2

void help ()
{
	cout << "Help message for coder:" << endl;
	cout << "-i <path> Input file" << endl;
	cout << "-o <path> Output file" << endl;
	cout << "-e Encode mode" << endl;
	cout << "-d Decode mode" << endl;
}

coder::Randomizer_2011 randomizer;

int main(int argc, char* argv[])
{
	CommandLine commandLine(argc, argv);

	if (commandLine.is("-h") || commandLine.is("--help")) {
		help();
		return 0;
	}

	Coder coder;

	if (commandLine.is("-i")) {
		char *szFile = commandLine.get("-i");
		coder.setInputFile(szFile);
	} else {
		char *szFile = new char[100];
		memset(szFile, 0, 100);
		cout << "Then input file name: ";
		cin >> szFile;
                coder.setInputFile(szFile);
	}

	if (commandLine.is("-o")) {
		char *szFile = commandLine.get("-o");
		coder.setOutputFile(szFile);
	} else {
		char *szFile = new char[100];
		memset(szFile, 0, 100);
		cout << "The output file name: ";
		cin >> szFile;
                coder.setOutputFile(szFile);
	}
	
	int action = 0;

	if (commandLine.is("-e")) {
		action = ACTION_ENCODE;
	} else if (commandLine.is("-d")) {
		action = ACTION_DECODE;
	}

	if (action == 0) {
            do {
		cout << "Select action:" << endl;
		cout << "1. Encode." << endl;
		cout << "2. Decode." << endl;
		cout << "> ";
		cin >> action;
            } while (action < ACTION_ENCODE || action > ACTION_DECODE);
	}

	// encode
	if (action == ACTION_ENCODE) {
            // Generate 1024-bit key
            UINT64 *key = new UINT64[16];
            memset(key, 0, sizeof(UINT64) * 16);
            cout << "Select action for key: " << endl;
            cout << "1. Generate file-key." << endl;
            cout << "2. Read from file." << endl;
            cin >> action;

            if (action == 1) {
		// generate key
		for (int i = 0; i < 16; i++) {
                    key[i] = ((UINT64) randomizer.randomize(0xFFFFFFFF) << 32) + randomizer.randomize(0xFFFFFFFF);
		}
			
                char* szFileKey = (char*) "key1.bin";
		FILE *hKey1 = fopen(szFileKey, "w+");
		fwrite(key, sizeof(UINT64), 16, hKey1);
		fclose(hKey1);
                coder.setKeyFileIn(szFileKey);
            } else {
                char *szFileKey = new char [100];
                memset(szFileKey, 0, 100);
                cout << "Enter key-file: " << endl;
                cin >> szFileKey;
                FILE *hKey1 = fopen(szFileKey, "r");
                fread(key, sizeof(UINT64), 16, hKey1);
                fclose(hKey1);
                free(szFileKey);
                coder.setKeyFileIn(szFileKey);
            }
                
            coder.setKeyFileOut((char*) "key2.bin");
            coder.encode();
	} else {
            // decode
            char *pName = new char [100];
            cout << "Enter key-file: ";
            cin >> pName;
            coder.setKeyFileIn(pName);
            
            coder.decode();
	}

	return 0;
}