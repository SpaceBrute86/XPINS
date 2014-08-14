//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

//Version 0.2.0
#ifndef __XPINS__Parser__
#define __XPINS__Parser__

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include "XPINSArray.h"

using namespace std;
using namespace XPINSInstructions;
class XPINSBindings;

namespace XPINSParser
{
	
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
//Memory Management Functions
	void SetGarbageCapcaity(size_t capacity);				//Set Garbage Capacity
	void EmptyGarbage(XPINSVarSpace& vars);					//Delete unused values to avoid leaks
	void EmptyAllGarbage();									//Deal withMemory Pressure



// Argument Parsing
	void* ParseArg(XPINSScriptSpace& script, Argument arg,  DataType& type, bool ignoreLastArrayIndex=false);						//Any type
	
	
	// Parsing Scripts
	void ParseScript(string,vector<XPINSBindings*>);
	void ParseScriptCluster(string,vector<XPINSBindings*>);
	
}
// Constant, Built In Function, and Expression Processing
namespace XPINSBuiltIn{
	
// Expression Parsing:
	bool ParseBoolExp(opCode op,bool assign,XPINSArray args);								//BOOL
	double ParseNumExp(opCode op,bool assign,XPINSArray args);								//NUM
	XPINSScriptableMath::Vector ParseVecExp(opCode op,bool assign,XPINSArray args);			//VEC
	XPINSScriptableMath::Quaternion ParseQuatExp(opCode op,bool assign,XPINSArray args);	//QUAT
	XPINSScriptableMath::Matrix ParseMatExp(opCode op,bool assign,XPINSArray args);			//MAT
	XPINSScriptableMath::Polynomial ParsePolyExp(opCode op,bool assign,XPINSArray args);	//POLY
	XPINSScriptableMath::VectorField ParseFieldExp(opCode op,bool assign,XPINSArray args);	//FIELD

// BIF (Built In Function) Parsing:
	bool ParseBoolBIF(int fNum,XPINSArray args);									//BOOL
	double ParseNumBIF(int fNum,XPINSArray args);								//NUM
	XPINSScriptableMath::Vector ParseVecBIF(int fNum, XPINSArray args);			//VEC
	XPINSScriptableMath::Quaternion ParseQuatBIF(int fNum, XPINSArray args);		//QUAT
	XPINSScriptableMath::Matrix ParseMatBIF(int fNum, XPINSArray args);			//MAT
	XPINSScriptableMath::Polynomial ParsePolyBIF(int fNum, XPINSArray args);		//POLY
	XPINSScriptableMath::VectorField ParseFieldBIF(int fNum, XPINSArray args);	//FIELD
	void ParseVoidBIF(int fNum, XPINSArray args);									//VOID
	
}

#endif /* defined(__Script__ScriptParser__) */
