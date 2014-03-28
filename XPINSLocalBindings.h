//
//  XPINSLocalBindings.h
//  XPINS
//
//  Created by Robbie Markwick on 3/27/14.
//
//

#ifndef __XPINS__XPINSLocalBindings__
#define __XPINS__XPINSLocalBindings__

#include <iostream>
#include"XPINSBindings.h"

/*include necessary header files*/

class XPINSLocalBindings : public XPINSBindings
{
public:
	virtual void BindFunction(int,string,XPINSParser::XPINSVarSpace* data,int&,void*);
	/*Define your custom parameters here*/
	
private:
	/*Declare your custom functions here*/
	
};


#endif /* defined(__XPINS__XPINSLocalBindings__) */
