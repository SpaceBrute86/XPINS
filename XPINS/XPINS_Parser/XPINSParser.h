//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

//Version 0.2.0
#ifndef __XPINS__Parser__
#define __XPINS__Parser__

#include <iostream>
#include <vector>
#include <string>
#include "XPINSScriptableMath.h"


using namespace std;
class XPINSCustomStruct {
public:
	bool shouldDelete;
	void* obj;
};
struct XPINSParams;
namespace XPINSParser{
	//Variable space
	struct varSpace{
		vector<bool> bVars;//bool variables
		vector<int> iVars;//int variables
		vector<float> fVars;//float variables
		vector<XPINSScriptableMath::Vector *> vVars;//Vector Variables
		vector<XPINSCustomStruct> pVars;//Custom type variables
	};
	//PARAMETER PARSING:
	//All of these functions have same params, just different return types
	//PARAM: the script text
	//PARAM: a varSpace object used to store script variables
	//PARAM: current index
	//PARAM: expected end
	bool parseBoolArg(string,XPINSParams*,varSpace*,int*,char);
	int parseIntArg(string,XPINSParams*,varSpace*,int*,char);
	float parseFloatArg(string,XPINSParams*,varSpace*,int*,char);
	XPINSScriptableMath::Vector* parseVecArg(string,XPINSParams*,varSpace*,int*,char);
	XPINSCustomStruct parsePointerArg(string,XPINSParams*,varSpace*,int*,char);

	//Primary Method
	//PARAM: the script text
	//PARAM: a varSpace object used to store script variables
	//PARAM: custom parameters passed to the script
	//PARAM: false unless calling recursively to use a WHILE loop
	//PARAM: Start index if reading while loop
	//PARAM: Stop index if reading while loop
	void parseScript(string,XPINSParams*,varSpace*,bool,int,int);
}

#endif /* defined(__Script__ScriptParser__) */
