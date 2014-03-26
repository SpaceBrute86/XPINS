//
//  XPINSBridge.h
//  XPINS
//
//  Created by Robbie Markwick on 8/29/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#ifndef __XPINS__XPINSBridge__
#define __XPINS__XPINSBridge__

#include <iostream>
#include <string>
#include "XPINSParser.h"

using namespace std;
/*include your custom classes*/
struct XPINSParams{
	/*Define Your Custom Parameters*/
	
};

namespace XPINSBridge
{
	//Bridge Function
	//PARAM: the Index of the function
	//PARAM: custom parameters passed to the script
	//PARAM: a varSpace object used to store script variables
	//PARAM: the script text
	//PARAM: the current index of the script (index of the '(')
	//PARAM: return pointer
	void BridgeFunction(int,string,XPINSParser::XPINSDataRef data,int&,void*);
}
#endif /* defined(__XPINS__XPINSBridge__) */
