//
//  main.cpp
//  XPINSTest
//
//  Created by Robbie Markwick on 11/12/13.
//
//this program is designed to allow you to test your scripts
//The sample script displays a multiplication table.
//Note that XPINSBridge must first be implemented
//

#include <iostream>
#include <fstream>
#include "XPINSLocalBindings.h"
#include "XPINS.h"
#include <math.h>



enum TestScript{
    Playground,
	Performance,
    XPINSTEST,
    MATHTEST,
};

const TestScript state=XPINSTEST;

const string directory="/Users/robbiemarkwick/Documents/Code/XPINS/";

int main(int argc, const char * argv[])
{
	ifstream inFile;
	switch (state)
	{
		case Playground:
			inFile.open(directory+"XPINSTest/PlayGround.XPINS");
			break;
		case Performance:
			inFile.open(directory+"XPINSTest/Performance.XPINS");
			break;
		case XPINSTEST:
			inFile.open(directory+"XPINSTest/TestScript.XPINS");
			break;
		case MATHTEST:
			inFile.open(directory+"XPINSTest/MathTest.XPINS");
			break;
		default:
			break;
	}
	if (inFile.fail()) {
		cerr << "unable to open file for reading" << endl;
		exit(1);
	}
	string script="";
	char ch;
	while (inFile.get(ch)) {
		script+=ch;
	}
	inFile.close();
	cout<<"Running Script:\n"<<script<<"\n\n";
	vector<XPINSBindings*> bindings=vector<XPINSBindings*>(1);
	bindings[0]=new XPINSLocalBindings();
	XPINSParser::ParseScript(script, bindings);
	cout<<"\nSCRIPT FINISHED\n";

    return 0;
}

