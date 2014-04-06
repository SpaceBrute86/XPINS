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
class XPINSBindings;

namespace XPINSParser
{
	//Variable space
	struct XPINSVarSpace{
		vector<bool> bVars;//bool variables
		vector<int> iVars;//int variables
		vector<float> fVars;//float variables
		vector<XPINSScriptableMath::Vector> vVars;//Vector Variables
		vector<XPINSScriptableMath::Matrix> mVars;//Vector Variables
		vector<string> sVars;//Vector Variables
		vector<void*> pVars;//Custom type variables
	};
	//Argument PARSING:
	bool ParseBoolArg(string,XPINSVarSpace*,XPINSBindings*,int&,char);
	int ParseIntArg(string,XPINSVarSpace*,XPINSBindings*,int&,char);
	float ParseFloatArg(string,XPINSVarSpace*,XPINSBindings*,int&,char);
	XPINSScriptableMath::Vector ParseVecArg(string,XPINSVarSpace*,XPINSBindings*,int&,char);
	XPINSScriptableMath::Matrix ParseMatArg(string,XPINSVarSpace*,XPINSBindings*,int&,char);
	string ParseStrArg(string,XPINSVarSpace*,XPINSBindings*,int&,char);
	void* ParsePointerArg( string,XPINSVarSpace*,XPINSBindings*,int&,char);
	
	//Get Variables access
	void SetNumVar(float val, string,XPINSVarSpace*,int&,char);
	void SetVecVar(XPINSScriptableMath::Vector val, string,XPINSVarSpace*,int&,char);
	void SetMatVar(XPINSScriptableMath::Matrix val, string,XPINSVarSpace*,int&,char);
	
	//Parsing Scripts
	void ParseScript(string,XPINSBindings*);
	
}
//Built In Function/Expression Processing
namespace XPINSBuiltIn{
	//Expression PARSING:
	bool ParseBoolExp(string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	int ParseIntExp(string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	float ParseFloatExp(string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	XPINSScriptableMath::Vector ParseVecExp(string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	XPINSScriptableMath::Matrix ParseMatExp(string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	void ParseVoidExp(string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);

	//BIF (Built In Function) PARSING:
	bool ParseBoolBIF(int fNum, string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	int ParseIntBIF(int fNum, string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	float ParseFloatBIF(int fNum, string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	XPINSScriptableMath::Vector ParseVecBIF(int fNum, string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	XPINSScriptableMath::Matrix ParseMatBIF(int fNum, string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
	void ParseVoidBIF(int fNum, string,XPINSParser::XPINSVarSpace*,XPINSBindings*,int&);
}

#endif /* defined(__Script__ScriptParser__) */
