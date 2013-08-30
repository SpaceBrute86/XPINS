//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#ifndef __XPINS__Parser__
#define __XPINS__Parser__

#include <iostream>
#include <vector>
#include "XPINSScriptableMath.h"
#include "XPINSBridge.h"


using namespace std;
namespace XPINSParser{
	//Variable space
	struct varSpace{
		vector<bool> bVars;//bool variables
		vector<int> iVars;//int variables
		vector<float> fVars;//float variables
		vector<ScriptableMath::Vector *> vVars;//Vector Variables
		vector<void *> pVars;//Custom type variables
	};
	
	//Primary Method
	//PARAM: the script text
	//PARAM: a varSpace object used to store script variables
	//PARAM: custom parameters passed to the script
	//PARAM: false unless calling recursively to use a WHILE loop
	//PARAM: Start index if reading while loop
	//PARAM: Stop index if reading while loop
	void parseScript(char *,varSpace*,XPINSBridge::params*,bool,int,int);
	//Read Function Parameter
	//PARAM: the script text
	//PARAM: the current index
	//PARAM: Variable Type
	//PARAM: expected end character (',' or ')')
	int readFuncParameter(char*,int*,char,char);
}

#endif /* defined(__Script__ScriptParser__) */
