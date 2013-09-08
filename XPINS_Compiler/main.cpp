//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#include <iostream>
#include <fstream>
#include <string>
#include "XPINSCompiler.h"
using namespace std;
const int kMajor=0;
const int kMinor=1;

int main(int argc, const char * argv[])
{
	string fileName="";
	cout<<"Please enter the full path minus extension for the script you would like to compile"<<endl;
	cout<<"Make sure that the file extension is .XPINS"<<endl;
	cin>>fileName;
	string fileExt=".XPINS";//File extension for uncompiled XPINS: XPINS
	string outExt=".XPINSX";//File extension for compiled XPINS: XPINSX
	//read in file
	ifstream inFile;
	inFile.open(fileName+fileExt);
	if (inFile.fail()) {
		cerr << "unable to open file for reading" << endl;
		exit(1);
	}
	string scriptText="";
	char ch;
	while (inFile.get(ch)) {
		scriptText+=ch;
	}
	inFile.close();
	//compile script
	if(!XPINSCompiler::compileScript(&scriptText)){
		exit(-1);//script could not comiple
	}
	
	
	//Write Output to file

	ofstream outFile;
	outFile.open(fileName+outExt);
	if (outFile.fail()) {
		cerr << "unable to open file: "<<fileName<<outExt<<" for writing" << endl;
		exit(1);
	}
	cout<<endl<<endl<<endl<<"Writing To File:\n"<<fileName<<outExt;
	for (int i=0;i<scriptText.length();i++) {
		ch=scriptText[i];
		outFile<<ch;
	}
	outFile.close();
    return 0;
}
