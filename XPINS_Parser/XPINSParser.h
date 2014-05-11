//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

//Version 0.2.0
#ifndef __XPINS__Parser__
#define __XPINS__Parser__

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include "XPINSScriptableMath.h"


using namespace std;
class XPINSBindings;

namespace XPINSParser
{
// Array Data Type
	struct XPINSArray{
		vector<void*> values;	//Value References
		string types;			//Type representaitions
	};
// Variable space
	struct XPINSVarSpace{
	// Variables
		bool* bVars;							//bool variables
		double* nVars;							//number variables
		XPINSScriptableMath::Vector* vVars;		//Vector Variables
		XPINSScriptableMath::Matrix* mVars;		//Matrix Variables
		XPINSScriptableMath::Polynomial* pVars;	//Polynomial Variables
		string* sVars;							//String Variables
		void** oVars;							//Custom type variables
		XPINSArray* aVars;						//Array variables
	// Deconstructor
		~XPINSVarSpace();	//Clears All Variables
	};
// Script and related Data
	class XPINSScriptSpace{
	public:
	// Data
		string instructions;				//Script Text
		int index;							//Current Index in Script Text
		XPINSVarSpace* data;				//Script Variables
		vector<XPINSBindings*> bindings;	//Bindings objects
		list<void*>toDelete;				//Objects to delte at end of Script
	// Functions
		XPINSScriptSpace(string script,vector<XPINSBindings*> bindings);	//Create Script Space
		char currentChar();													//Character at script indx
		bool matchesString(string testSTring);								//Check String against script
	};

// Argument Parsing
	bool* ParseBoolArg(XPINSScriptSpace& script,char expectedEnd);								//Boolean
	double* ParseNumArg(XPINSScriptSpace& script,char expectedEnd);								//Number
	XPINSScriptableMath::Vector* ParseVecArg(XPINSScriptSpace& script,char expectedEnd);		//Vector
	XPINSScriptableMath::Matrix* ParseMatArg(XPINSScriptSpace& script,char expectedEnd);		//Matrix
	XPINSScriptableMath::Polynomial* ParsePolyArg(XPINSScriptSpace& script,char expectedEnd);	//Polynomial
	string* ParseStrArg(XPINSScriptSpace& script,char expectedEnd);								//String
	void** ParsePointerArg(XPINSScriptSpace& script,char expectedEnd, char* type=NULL);			//Pointer
	XPINSArray* ParseArrayArg(XPINSScriptSpace& script,char expectedEnd);						//Array
	
// Parsing Scripts
	void ParseScript(string,vector<XPINSBindings*>);
	
}
// Built In Function and Expression Processing
namespace XPINSBuiltIn{
// Expression PARSING:
	bool ParseBoolExp(XPINSParser::XPINSScriptSpace&);								//Boolean Expression
	double ParseNumExp(XPINSParser::XPINSScriptSpace&);								//Numerical Expression
	XPINSScriptableMath::Vector ParseVecExp(XPINSParser::XPINSScriptSpace&);		//Vector Expression
	XPINSScriptableMath::Matrix ParseMatExp(XPINSParser::XPINSScriptSpace&);		//Matrix Expression
	XPINSScriptableMath::Polynomial ParsePolyExp(XPINSParser::XPINSScriptSpace&);	//Polynomial Expression
	void ParseVoidExp(XPINSParser::XPINSScriptSpace&);								//Void Expression

// BIF (Built In Function) PARSING:
	bool ParseBoolBIF(int fNum,XPINSParser::XPINSScriptSpace&);								//Boolean BIF
	double ParseNumBIF(int fNum, XPINSParser::XPINSScriptSpace&);							//Number BIF
	XPINSScriptableMath::Vector ParseVecBIF(int fNum, XPINSParser::XPINSScriptSpace&);		//Vector BIF
	XPINSScriptableMath::Matrix ParseMatBIF(int fNum, XPINSParser::XPINSScriptSpace&);		//Matrix BIF
	XPINSScriptableMath::Polynomial ParsePolyBIF(int fNum, XPINSParser::XPINSScriptSpace&);	//Polynomial BIF
	void ParseVoidBIF(int fNum, XPINSParser::XPINSScriptSpace&);							//Void BIF
}

#endif /* defined(__Script__ScriptParser__) */
