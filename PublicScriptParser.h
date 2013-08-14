//
//  PublicScriptParser.h
//
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#ifndef __Script__ScriptParser__
#define __Script__ScriptParser__

#include <iostream>
#include <vector>
#include "ScriptableMath.h"
/*Import your custom classes*/



using namespace std;
namespace ScriptParser {
	//Variable space
	struct varSpace{
		vector<bool> bVars;//bool variables
		vector<int> iVars;//int variables
		vector<float> fVars;//float variables
		vector<Math::Vector *> vVars;//Vector Variables
		vector<void *> pVars;//Custom type variables
	};
	struct params{
		/*Define Your Custom Parameters*/
		
		
	};
	//Primary Method
	void parseScript(char *,varSpace*,params*,bool,int,int);
	
}

#endif /* defined(__Script__ScriptParser__) */
