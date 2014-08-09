//
//  XPNMath.h
//  XPINS
//
//  Created by Robbie Markwick on 5/4/14.
//
//

#import <Foundation/Foundation.h>
#import "XPINSScriptableMath.h"
@class XPNMatrix;
@interface XPNVector : NSObject{
@public
	XPINSScriptableMath::Vector vector;
}
//Creating Vectors
+(XPNVector*)vectorWithVector:(XPINSScriptableMath::Vector)vector;
+(XPNVector*)vectorWithX:(double)x Y:(double)y Z:(double)z;
+(XPNVector*)polarVectorWithR:(double)r Theta:(double)t Z:(double)z;
+(XPNVector*)sphericalVectorWithRho:(double)r Theta:(double)t PHI:(double)p;
+(XPNVector*) vectorWithMatrix:(XPNMatrix*)mat;

//Coordinates
-(double)getX;
-(double)getY;
-(double)getZ;
-(double)getR;
-(double)getMagnitude;
-(double)getTheta;
-(double)getPHI;
//Manipulations
-(XPNVector*)sumWithVector:(XPNVector*)vector;
-(XPNVector*)scale:(double)scale;
-(double)dotProductWithVector:(XPNVector*)vector;
-(XPNVector*)crossProductWithVector:(XPNVector*)vector;
-(XPNVector*)multiplyWithMatrix:(XPNMatrix*)matrix;
-(XPNVector*)projectoinOntoVector:(XPNVector*)vector;
-(double)angleFromVector:(XPNVector*)vector;
@end

@interface XPNMatrix : NSObject <NSCopying>{
@public
	XPINSScriptableMath::Matrix matrix;
}
//Creation
+(XPNMatrix*) matrixWithMatrix:(XPINSScriptableMath::Matrix)mat;
+(XPNMatrix*) matirxWithHeight:(int)rows Width:(int)cols;
+(XPNMatrix*) identityMatrixOfSize:(int)size;
+(XPNMatrix*) rotationMatrixWithAngle:(double)angle aroundVector:(XPNVector*)vec;
+(XPNMatrix*) matrixWithVector:(XPNVector*)vec;
//Getting/Setting Values
-(void)setValue:(double)val forRow:(int)row Column:(int)col;
-(double)getValueForRow:(int)row Column:(int)col;
//Matrix operations
-(void)appendMatrix:(XPNMatrix*)mat;
-(XPNMatrix*)sumWithMatrix:(XPNMatrix*)mat;
-(XPNMatrix*)scale:(double)scale;
-(XPNMatrix*)multiplyWithMatrix:(XPNMatrix*)mat;
-(XPNMatrix*)invert;
-(double)determinant;
-(XPNMatrix*)transpose;
-(XPNMatrix*)rowEchelon;
-(XPNMatrix*)reducedRowEchelon;
@end

@interface XPNPolynomial : NSObject <NSCopying>{
@public
	XPINSScriptableMath::Polynomial polynomial;
}
//Creation
+(XPNPolynomial*) polynomialWithPolynomial:(XPINSScriptableMath::Polynomial)poly;
+(XPNPolynomial*) monomialWithCoefficient:(double)coeff xPower:(int)x yPower:(int)y zPower:(int)z tPower:(int)t;
//Arithmetic
-(XPNPolynomial*)sumWithPolynomial:(XPNPolynomial*)poly;
-(XPNPolynomial*)scale:(double)scale;
-(XPNPolynomial*)multiplyByPolynomial:(XPNPolynomial*)poly;
-(XPNPolynomial*)raiseToPower:(int)power;
-(XPNPolynomial*)compositionWithPolynomial:(XPNPolynomial*)poly forVar:(int) var;
//Evaluation
-(double)evaluationWithX:(double)x Y:(double)y Z:(double)z T:(double)t;
-(XPNPolynomial*)partialEvaluationWithValue:(double)val forVar:(int) var;
//Calculus
-(XPNPolynomial*)derivativeForVar:(int)var;
-(XPNPolynomial*)integralForVar:(int)var;

@end

