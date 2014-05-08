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
-(XPNVector*)projectionInDirectionOfVector:(XPNVector*)vector;

@end

/*
 //Accessing Components
 double ValueAtPosition(size_t,size_t);
 void SetValueAtPosition(double,size_t,size_t);
 //Special Matrices
 static Matrix IdentityMatrixOfSize(size_t);
 static Matrix DiagonalMatrixWithValues(std::vector<double>);
 static Matrix RotationMatrixWithAngleAroundVector(Vector,double);
 //Converting Matricies to/from Vectors
 static Vector VectorForMatrix(Matrix);
 static Matrix MatrixForVector(Vector);
 //Matrix Operations
 static Matrix Add(Matrix,Matrix);
 static Matrix Scale(Matrix,double);
 static Matrix Multiply(Matrix,Matrix);
 static Vector MultiplyMatrixVector(Matrix,Vector);
 static Matrix Invert(Matrix);
 static Matrix Transpose(Matrix);
 static double Determinant(Matrix);
 */
@interface XPNMatrix : NSObject{
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
-(XPNMatrix*)sumWithMatrix:(XPNMatrix*)mat;
-(XPNMatrix*)scale:(double)scale;
-(XPNMatrix*)multiplyWithMatrix:(XPNMatrix*)mat;
-(XPNMatrix*)invert;
-(double)determinant;
-(XPNMatrix*)transpose;
@end