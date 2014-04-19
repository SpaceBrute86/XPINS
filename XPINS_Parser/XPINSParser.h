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
	class XPINSScriptSpace{
	public:
		string instructions;
		int index;
		XPINSVarSpace* data;
		XPINSBindings* bindings;
		XPINSScriptSpace(string script,XPINSBindings*);
		char currentChar(){
			return instructions[index];
		}
		bool matchesString(string);
	};
	//Argument PARSING:
	bool ParseBoolArg(XPINSScriptSpace&,char,int*index=NULL);
	int ParseIntArg(XPINSScriptSpace&,char,int*index=NULL);
	float ParseFloatArg(XPINSScriptSpace&,char,int*index=NULL);
	XPINSScriptableMath::Vector ParseVecArg(XPINSScriptSpace&,char,int*index=NULL);
	XPINSScriptableMath::Matrix ParseMatArg(XPINSScriptSpace&,char,int*index=NULL);
	string ParseStrArg(XPINSScriptSpace&,char,int*index=NULL);
	void* ParsePointerArg(XPINSScriptSpace&,char,int*index=NULL);
	
	//Parsing Scripts
	void ParseScript(string,XPINSBindings*);
	
}
//Built In Function/Expression Processing
namespace XPINSBuiltIn{
	//Expression PARSING:
	bool ParseBoolExp(XPINSParser::XPINSScriptSpace&);
	int ParseIntExp(XPINSParser::XPINSScriptSpace&);
	float ParseFloatExp(XPINSParser::XPINSScriptSpace&);
	XPINSScriptableMath::Vector ParseVecExp(XPINSParser::XPINSScriptSpace&);
	XPINSScriptableMath::Matrix ParseMatExp(XPINSParser::XPINSScriptSpace&);
	void ParseVoidExp(XPINSParser::XPINSScriptSpace&);

	//BIF (Built In Function) PARSING:
	bool ParseBoolBIF(int fNum,XPINSParser::XPINSScriptSpace&);
	int ParseIntBIF(int fNum, XPINSParser::XPINSScriptSpace&);
	float ParseFloatBIF(int fNum, XPINSParser::XPINSScriptSpace&);
	XPINSScriptableMath::Vector ParseVecBIF(int fNum, XPINSParser::XPINSScriptSpace&);
	XPINSScriptableMath::Matrix ParseMatBIF(int fNum, XPINSParser::XPINSScriptSpace&);
	void ParseVoidBIF(int fNum, XPINSParser::XPINSScriptSpace&);
}

#endif /* defined(__Script__ScriptParser__) */
