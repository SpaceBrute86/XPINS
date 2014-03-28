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
#include <list>
#include "XPINSParser.h"

using namespace std;

class XPINSBindings
{
	//Bind Function
public:
	virtual void BindFunction(int,string,XPINSParser::XPINSVarSpace* data,int&,void*){}
	list<void*>toDelete;
};
#endif /* defined(__XPINS__XPINSBridge__) */
