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
	struct XPINSArray{
		vector<void*> values;
		string types;
	};
	//Variable space
	struct XPINSVarSpace{
		bool* bVars;//bool variables
		double* nVars;//number variables
		XPINSScriptableMath::Vector* vVars;//Vector Variables
		XPINSScriptableMath::Matrix* mVars;//Matrix Variables
		string* sVars;//String Variables
		void** pVars;//Custom type variables
		XPINSArray* aVars;//Array variables
		~XPINSVarSpace(){
			free(bVars);
			free(nVars);
			free(vVars);
			free(mVars);
			free(sVars);
			free(pVars);
			free(aVars);
		}
	};
	class XPINSScriptSpace{
	public:
		string instructions;
		int index;
		XPINSVarSpace* data;
		vector<XPINSBindings*> bindings;
		XPINSScriptSpace(string script,vector<XPINSBindings*>);
		char currentChar(){
			return instructions[index];
		}
		bool matchesString(string);
	};
	//Argument PARSING:
	bool* ParseBoolArg(XPINSScriptSpace&,char);
	double* ParseNumArg(XPINSScriptSpace&,char);
	XPINSScriptableMath::Vector* ParseVecArg(XPINSScriptSpace&,char);
	XPINSScriptableMath::Matrix* ParseMatArg(XPINSScriptSpace&,char);
	string* ParseStrArg(XPINSScriptSpace&,char);
	void** ParsePointerArg(XPINSScriptSpace&,char, char* type=NULL);
	XPINSArray* ParseArrayArg(XPINSScriptSpace&,char);

	
	//Parsing Scripts
	void ParseScript(string,vector<XPINSBindings*>);
	
}
//Built In Function/Expression Processing
namespace XPINSBuiltIn{
	//Expression PARSING:
	bool ParseBoolExp(XPINSParser::XPINSScriptSpace&);
	double ParseNumExp(XPINSParser::XPINSScriptSpace&);
	XPINSScriptableMath::Vector ParseVecExp(XPINSParser::XPINSScriptSpace&);
	XPINSScriptableMath::Matrix ParseMatExp(XPINSParser::XPINSScriptSpace&);
	void ParseVoidExp(XPINSParser::XPINSScriptSpace&);

	//BIF (Built In Function) PARSING:
	bool ParseBoolBIF(int fNum,XPINSParser::XPINSScriptSpace&);
	double ParseNumBIF(int fNum, XPINSParser::XPINSScriptSpace&);
	XPINSScriptableMath::Vector ParseVecBIF(int fNum, XPINSParser::XPINSScriptSpace&);
	XPINSScriptableMath::Matrix ParseMatBIF(int fNum, XPINSParser::XPINSScriptSpace&);
	void ParseVoidBIF(int fNum, XPINSParser::XPINSScriptSpace&);
}

#endif /* defined(__Script__ScriptParser__) */
