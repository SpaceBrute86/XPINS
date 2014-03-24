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

struct XPINSParams;
namespace XPINSParser{
	//Variable space
	struct varSpace{
		vector<bool> bVars;//bool variables
		vector<int> iVars;//int variables
		vector<float> fVars;//float variables
		vector<XPINSScriptableMath::Vector> vVars;//Vector Variables
		vector<XPINSScriptableMath::Matrix> mVars;//Vector Variables
		vector<void*> pVars;//Custom type variables
	};
	//PARAMETER PARSING:
	//All of these functions have same params, just different return types
	//PARAM: the script text
	//PARAM: a varSpace object used to store script variables
	//PARAM: current index
	//PARAM: expected end
	bool ParseBoolArg(string,XPINSParams*,varSpace*,int*,char);
	int ParseIntArg(string,XPINSParams*,varSpace*,int*,char);
	float ParseFloatArg(string,XPINSParams*,varSpace*,int*,char);
	XPINSScriptableMath::Vector ParseVecArg(string,XPINSParams*,varSpace*,int*,char);
	XPINSScriptableMath::Matrix ParseMatArg(string,XPINSParams*,varSpace*,int*,char);
	void* ParsePointerArg(string,XPINSParams*,varSpace*,int*,char);

	//Primary Method
	//PARAM: the script text
	//PARAM: a varSpace object used to store script variables
	//PARAM: custom parameters passed to the script
	//PARAM: false unless calling recursively to use a WHILE loop
	//PARAM: Start index if reading while loop
	//PARAM: Stop index if reading while loop
	void ParseScript(string,XPINSParams*,varSpace*,bool,int,int);
	
	//Built In Function/Expression Processing
	namespace XPINSBuiltIn{
		//Expression PARSING:
		bool ParseBoolExp(string,XPINSParams*,varSpace*,int*);
		int ParseIntExp(string,XPINSParams*,varSpace*,int*);
		float ParseFloatExp(string,XPINSParams*,varSpace*,int*);
		XPINSScriptableMath::Vector ParseVecExp(string,XPINSParams*,varSpace*,int*);
		XPINSScriptableMath::Matrix ParseMatExp(string,XPINSParams*,varSpace*,int*);

		//BIF (Built In Function) PARSING:
		bool ParseBoolBIF(string,XPINSParams*,varSpace*,int*);
		int ParseIntBIF(string,XPINSParams*,varSpace*,int*);
		float ParseFloatBIF(string,XPINSParams*,varSpace*,int*);
		XPINSScriptableMath::Vector ParseVecBIF(string,XPINSParams*,varSpace*,int*);
		XPINSScriptableMath::Matrix ParseMatBIF(string,XPINSParams*,varSpace*,int*);
		void ParseVoidBIF(string,XPINSParams*,varSpace*,int*);
	}
}

#endif /* defined(__Script__ScriptParser__) */
