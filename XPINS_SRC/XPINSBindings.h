//
//  XPINSBridge.h
//  XPINS
//
//  Created by Robbie Markwick on 8/29/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#ifndef __XPINS__XPINSBindings__
#define __XPINS__XPINSBindings__

#include <iostream>
#include <string>
#include "XPINSParser.h"

using namespace std;

class XPINSBindings
{
public:
	virtual void* BindFunction(int,XPINSArray args)=0;	//Bind Function
};
#endif /* defined(__XPINS__XPINSBridge__) */
