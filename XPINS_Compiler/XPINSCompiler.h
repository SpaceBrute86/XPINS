//
//  XPINSCompiler.h
//  XPINS
//
//  Created by Robbie Markwick on 9/7/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#ifndef __XPINS__XPINSCompiler__
#define __XPINS__XPINSCompiler__

#include <iostream>
#include <string>
using namespace std;
namespace XPINSCompiler{
	//Call this to compile an XPINS script
	bool compileScript(string&);

	//Compile Steps (you normally don't call these)
	bool checkVersion(string&);
	bool removeComments(string&);
	bool renameFunctions(string&);
	bool renameVars(string&);
	bool renameBuiltIns(string&);
	bool renameVars(string&);
	bool cleanUp(string&);
}
#endif /* defined(__XPINS__XPINSCompiler__) */
