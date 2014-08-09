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
#include "XPINSInstruction.h"

using namespace std;
using namespace XPINSInstructions;
class XPINSBindings;

namespace XPINSParser
{
// Array Data Type
	struct XPINSArray{
		vector<void*> values;	//Value References
		string types;			//Type representaitions
		vector<DataType> dataTypes;
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
		XPINSInstructions::InstructionSet instructions; //Script Instructions
		string clusterURL;					//URL of script Cluster URL
		XPINSVarSpace* data;				//Script Variables
		vector<XPINSBindings*> bindings;	//Bindings objects
		list<void*>toDelete;				//Objects to delte at end of Script
	//	XPINSArray* scriptParams;			//Parameters to be passed from script to script
	//	void* returnVal;					//Return Value from script
	// Functions
		XPINSScriptSpace(string script,vector<XPINSBindings*> bindings);		//Create Script Space
		XPINSScriptSpace(string cluster,string name,vector<XPINSBindings*> bindings);
	};
//Miscllaneous Functions
	void SetGarbageCapcaity(size_t capacity);				//Set Garbage Capacity
	void EmptyGarbage(XPINSVarSpace& vars);					//Delete unused values to avoid leaks
	void EmptyAllGarbage();									//Deal withMemory Pressure


// Argument Parsing
	void* ParseArg(XPINSScriptSpace& script, Argument arg,  DataType& type);						//Any type
	bool* ParseBoolArg(XPINSScriptSpace& script, Argument arg);								//Boolean
	double* ParseNumArg(XPINSScriptSpace& script, Argument arg);								//Number
	XPINSScriptableMath::Vector* ParseVecArg(XPINSScriptSpace& script, Argument arg);		//Vector
	XPINSScriptableMath::Quaternion* ParseQuatArg(XPINSScriptSpace& script, Argument arg);	//Quaternion
	XPINSScriptableMath::Matrix* ParseMatArg(XPINSScriptSpace& script, Argument arg);		//Matrix
	XPINSScriptableMath::Polynomial* ParsePolyArg(XPINSScriptSpace& script, Argument arg);	//Polynomial
	XPINSScriptableMath::VectorField* ParseFieldArg(XPINSScriptSpace& script, Argument arg);	//VectorField
	string* ParseStrArg(XPINSScriptSpace& script, Argument arg);								//String
	void** ParsePointerArg(XPINSScriptSpace& script, Argument arg, DataType* type=NULL);							//Pointer
	XPINSArray* ParseArrayArg(XPINSScriptSpace& script, Argument arg, bool ignoreSubscripts=false);						//Array
	
// Parsing Scripts
	void ParseScript(string,vector<XPINSBindings*>);
	void ParseScriptCluster(string,vector<XPINSBindings*>);
	
}
// Constant, Built In Function, and Expression Processing
namespace XPINSBuiltIn{
	
// Expression Parsing:
	bool ParseBoolExp(opCode op,bool assign,XPINSParser::XPINSScriptSpace&, vector<Argument> args);								//BOOL
	double ParseNumExp(opCode op,bool assign,XPINSParser::XPINSScriptSpace&, vector<Argument> args);								//NUM
	XPINSScriptableMath::Vector ParseVecExp(opCode op,bool assign,XPINSParser::XPINSScriptSpace&, vector<Argument> args);			//VEC
	XPINSScriptableMath::Quaternion ParseQuatExp(opCode op,bool assign,XPINSParser::XPINSScriptSpace&, vector<Argument> args);	//QUAT
	XPINSScriptableMath::Matrix ParseMatExp(opCode op,bool assign,XPINSParser::XPINSScriptSpace&, vector<Argument> args);			//MAT
	XPINSScriptableMath::Polynomial ParsePolyExp(opCode op,bool assign,XPINSParser::XPINSScriptSpace&, vector<Argument> args);	//POLY
	XPINSScriptableMath::VectorField ParseFieldExp(opCode op,bool assign,XPINSParser::XPINSScriptSpace&, vector<Argument> args);	//FIELD

// BIF (Built In Function) Parsing:
	bool ParseBoolBIF(int fNum,XPINSParser::XPINSScriptSpace&, vector<Argument> args);									//BOOL
	double ParseNumBIF(int fNum, XPINSParser::XPINSScriptSpace&, vector<Argument> args);								//NUM
	XPINSScriptableMath::Vector ParseVecBIF(int fNum, XPINSParser::XPINSScriptSpace&, vector<Argument> args);			//VEC
	XPINSScriptableMath::Quaternion ParseQuatBIF(int fNum, XPINSParser::XPINSScriptSpace&, vector<Argument> args);		//QUAT
	XPINSScriptableMath::Matrix ParseMatBIF(int fNum, XPINSParser::XPINSScriptSpace&, vector<Argument> args);			//MAT
	XPINSScriptableMath::Polynomial ParsePolyBIF(int fNum, XPINSParser::XPINSScriptSpace&, vector<Argument> args);		//POLY
	XPINSScriptableMath::VectorField ParseFieldBIF(int fNum, XPINSParser::XPINSScriptSpace&, vector<Argument> args);	//FIELD
	void ParseVoidBIF(int fNum, XPINSParser::XPINSScriptSpace&, vector<Argument> args);									//VOID
	
}

#endif /* defined(__Script__ScriptParser__) */
