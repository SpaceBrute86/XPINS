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
		bool* bVars;								//Boolean variables
		double* nVars;								//Number variables
		XPINSScriptableMath::Vector* vVars;			//Vector Variables
		XPINSScriptableMath::Quaternion* qVars;		//Quaternion Variables
		XPINSScriptableMath::Matrix* mVars;			//Matrix Variables
		XPINSScriptableMath::Polynomial* pVars;		//Polynomial Variables
		XPINSScriptableMath::VectorField* fVars;	//Vector Field Variables
		string* sVars;								//String Variables
		void** oVars;								//Custom type variables
		XPINSArray* aVars;							//Array variables
	//Other
		XPINSArray Garbage;
		size_t GarbageCost;
		XPINSArray Trash;

	// Deconstructor
		~XPINSVarSpace();	//Clears All Variables
	};
// Script and related Data
	class XPINSScriptSpace{
	public:
	// Data
		string instructions;				//Script Text
		int index;							//Current Index in Script Text
		string clusterURL;					//URL of script Cluster URL
		XPINSVarSpace* data;				//Script Variables
		vector<XPINSBindings*> bindings;	//Bindings objects
		list<void*>toDelete;				//Objects to delte at end of Script
		XPINSArray* scriptParams;			//Parameters to be passed from script to script
		void* returnVal;					//Return Value from script
	// Functions
		XPINSScriptSpace(string script,vector<XPINSBindings*> bindings);		//Create Script Space
		XPINSScriptSpace(string cluster,string name,vector<XPINSBindings*> bindings);
		char currentChar();														//Character at script index
		bool matchesString(string testSTring);									//Check String against script
	};
//Miscllaneous Functions
	int readInt(XPINSScriptSpace& script,char expectedEnd);	//Read an Integer from the script
	void SetGarbageCapcaity(size_t capacity);				//Set Garbage Capacity
	void EmptyGarbage(XPINSVarSpace& vars);					//Delete unused values to avoid leaks
	void EmptyAllGarbage();									//Deal withMemory Pressure


// Argument Parsing
	void* ParseArg(XPINSScriptSpace& script, char expectedEnd, char& type);						//Any type
	bool* ParseBoolArg(XPINSScriptSpace& script,char expectedEnd);								//Boolean
	double* ParseNumArg(XPINSScriptSpace& script,char expectedEnd);								//Number
	XPINSScriptableMath::Vector* ParseVecArg(XPINSScriptSpace& script,char expectedEnd);		//Vector
	XPINSScriptableMath::Quaternion* ParseQuatArg(XPINSScriptSpace& script,char expectedEnd);	//Quaternion
	XPINSScriptableMath::Matrix* ParseMatArg(XPINSScriptSpace& script,char expectedEnd);		//Matrix
	XPINSScriptableMath::Polynomial* ParsePolyArg(XPINSScriptSpace& script,char expectedEnd);	//Polynomial
	XPINSScriptableMath::VectorField* ParseFieldArg(XPINSScriptSpace& script,char expectedEnd);	//VectorField

	string* ParseStrArg(XPINSScriptSpace& script,char expectedEnd);								//String
	void** ParsePointerArg(XPINSScriptSpace& script,char expectedEnd, char* type=NULL);			//Pointer
	XPINSArray* ParseArrayArg(XPINSScriptSpace& script,char expectedEnd);						//Array
	
// Parsing Scripts
	void ParseScript(string,vector<XPINSBindings*>);
	void ParseScriptCluster(string,vector<XPINSBindings*>);
	
}
// Constant, Built In Function, and Expression Processing
namespace XPINSBuiltIn{
	
// Constant Parsing
	bool ParseBoolConst(XPINSParser::XPINSScriptSpace& script);									//BOOL
	double ParseNumConst(XPINSParser::XPINSScriptSpace& script,char stop);						//NUM
	XPINSScriptableMath::Vector ParseVecConst(XPINSParser::XPINSScriptSpace& script);			//VEC
	XPINSScriptableMath::Quaternion ParseQuatConst(XPINSParser::XPINSScriptSpace& script);		//QUAT
	XPINSScriptableMath::Matrix ParseMatConst(XPINSParser::XPINSScriptSpace& script);			//MAT
	XPINSScriptableMath::Polynomial ParsePolyConst(XPINSParser::XPINSScriptSpace& script);		//POLY
	XPINSScriptableMath::VectorField ParseFieldConst(XPINSParser::XPINSScriptSpace& script);	//FIELD
	string ParseStrConst(XPINSParser::XPINSScriptSpace& script);								//STR
	XPINSParser::XPINSArray ParseArrConst(XPINSParser::XPINSScriptSpace& script);				//ARR
	
// Expression Parsing:
	bool ParseBoolExp(XPINSParser::XPINSScriptSpace&);								//BOOL
	double ParseNumExp(XPINSParser::XPINSScriptSpace&);								//NUM
	XPINSScriptableMath::Vector ParseVecExp(XPINSParser::XPINSScriptSpace&);		//VEC
	XPINSScriptableMath::Quaternion ParseQuatExp(XPINSParser::XPINSScriptSpace&);	//QUAT
	XPINSScriptableMath::Matrix ParseMatExp(XPINSParser::XPINSScriptSpace&);		//MAT
	XPINSScriptableMath::Polynomial ParsePolyExp(XPINSParser::XPINSScriptSpace&);	//POLY
	XPINSScriptableMath::VectorField ParseFieldExp(XPINSParser::XPINSScriptSpace&);	//FIELD
	void ParseVoidExp(XPINSParser::XPINSScriptSpace&);								//VOID

// BIF (Built In Function) Parsing:
	bool ParseBoolBIF(int fNum,XPINSParser::XPINSScriptSpace&);									//BOOL
	double ParseNumBIF(int fNum, XPINSParser::XPINSScriptSpace&);								//NUM
	XPINSScriptableMath::Vector ParseVecBIF(int fNum, XPINSParser::XPINSScriptSpace&);			//VEC
	XPINSScriptableMath::Quaternion ParseQuatBIF(int fNum, XPINSParser::XPINSScriptSpace&);		//QUAT
	XPINSScriptableMath::Matrix ParseMatBIF(int fNum, XPINSParser::XPINSScriptSpace&);			//MAT
	XPINSScriptableMath::Polynomial ParsePolyBIF(int fNum, XPINSParser::XPINSScriptSpace&);		//POLY
	XPINSScriptableMath::VectorField ParseFieldBIF(int fNum, XPINSParser::XPINSScriptSpace&);	//FIELD
	void ParseVoidBIF(int fNum, XPINSParser::XPINSScriptSpace&);								//VOID
	
}

#endif /* defined(__Script__ScriptParser__) */
