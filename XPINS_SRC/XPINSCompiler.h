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
#include "XPINSInstruction.h"
using namespace std;
using namespace XPINSInstructions;
namespace XPINSCompiler{
	//Call this to compile an XPINS script
	InstructionSet compileScript(string);
}
#endif /* defined(__XPINS__XPINSCompiler__) */
