//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#ifndef __Script__ScriptParser__
#define __Script__ScriptParser__

#include <iostream>
#include <vector>
#include "ScriptableMath.h"
/*include your custom classes*/



using namespace std;
namespace XPINSParser {
	//Variable space
	struct varSpace{
		vector<bool> bVars;//bool variables
		vector<int> iVars;//int variables
		vector<float> fVars;//float variables
		vector<ScriptableMath::Vector *> vVars;//Vector Variables
		vector<void *> pVars;//Custom type variables
	};
	struct params{
		/*Define Your Custom Parameters*/
		
		
	};
	//Primary Method
	//PARAM: the script text
	//PARAM: a varSpace object used to store script variables
	//PARAM: custom parameters passed to the script
	//PARAM: false unless calling recursively to use a WHILE loop
	//PARAM: Start index if reading while loop
	//PARAM: Stop index if reading while loop
	void parseScript(char *,varSpace*,params*,bool,int,int);
	
}

#endif /* defined(__Script__ScriptParser__) */
