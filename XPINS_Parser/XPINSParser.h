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
	struct XPINSVarSpace{
		vector<bool> bVars;//bool variables
		vector<int> iVars;//int variables
		vector<float> fVars;//float variables
		vector<XPINSScriptableMath::Vector> vVars;//Vector Variables
		vector<XPINSScriptableMath::Matrix> mVars;//Vector Variables
		vector<void*> pVars;//Custom type variables
	};
	struct XPINSDataRef{
		XPINSParams* scriptParams;
		XPINSVarSpace* scriptVars;
	};
	//PARAMETER PARSING:
	bool ParseBoolArg(string,XPINSDataRef,int&,char);
	int ParseIntArg(string,XPINSDataRef,int&,char);
	float ParseFloatArg(string,XPINSDataRef,int&,char);
	XPINSScriptableMath::Vector ParseVecArg(string,XPINSDataRef,int&,char);
	XPINSScriptableMath::Matrix ParseMatArg(string,XPINSDataRef,int&,char);
	void* ParsePointerArg(string,XPINSDataRef,int&,char);

	//Primary Method
	void ParseScript(string,XPINSParams*);
	
}
//Built In Function/Expression Processing
namespace XPINSBuiltIn{
	//Expression PARSING:
	bool ParseBoolExp(string,XPINSParser::XPINSDataRef,int&);
	int ParseIntExp(string,XPINSParser::XPINSDataRef,int&);
	float ParseFloatExp(string,XPINSParser::XPINSDataRef,int&);
	XPINSScriptableMath::Vector ParseVecExp(string,XPINSParser::XPINSDataRef,int&);
	XPINSScriptableMath::Matrix ParseMatExp(string,XPINSParser::XPINSDataRef,int&);
	
	//BIF (Built In Function) PARSING:
	bool ParseBoolBIF(int fNum, string,XPINSParser::XPINSDataRef,int&);
	int ParseIntBIF(int fNum, string,XPINSParser::XPINSDataRef,int&);
	float ParseFloatBIF(int fNum, string,XPINSParser::XPINSDataRef,int&);
	XPINSScriptableMath::Vector ParseVecBIF(int fNum, string,XPINSParser::XPINSDataRef,int&);
	XPINSScriptableMath::Matrix ParseMatBIF(int fNum, string,XPINSParser::XPINSDataRef,int&);
	void ParseVoidBIF(int fNum, string,XPINSParser::XPINSDataRef,int&);
}

#endif /* defined(__Script__ScriptParser__) */
