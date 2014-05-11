//
//  XPNMath.m
//  XPINS
//
//  Created by Robbie Markwick on 5/4/14.
//
//

#import "XPNMath.h"

@implementation XPNVector
//Creating Vectors
+(XPNVector*)vectorWithVector:(XPINSScriptableMath::Vector)vector{
	XPNVector* v=[XPNVector alloc];
	v->vector=vector;
	return v;
}
+(XPNVector*)vectorWithX:(double)x Y:(double)y Z:(double)z{
	return [XPNVector vectorWithVector:XPINSScriptableMath:: Vector (x, y,z)];
}
+(XPNVector*)polarVectorWithR:(double)r Theta:(double)t Z:(double)z{
	return [XPNVector vectorWithVector:XPINSScriptableMath::Vector::PolarVector(r, t,z)];
}
+(XPNVector*)sphericalVectorWithRho:(double)r Theta:(double)t PHI:(double)p{
	return [XPNVector vectorWithVector:XPINSScriptableMath::Vector::SphericalVector(r, t,p)];
}
+(XPNVector*) matrixWithVector:(XPNMatrix*)mat{
	return [XPNVector vectorWithVector:XPINSScriptableMath::Matrix::VectorForMatrix(mat->matrix)];
}
//Coordinates
-(double)getX{
	double x;
	self->vector.RectCoords(&x, NULL, NULL);
	return x;
}
-(double)getY{
	double y;
	self->vector.RectCoords(NULL, &y, NULL);
	return y;
}
-(double)getZ{
	double z;
	self->vector.RectCoords( NULL,  NULL, &z);
	return z;
}
-(double)getR{
	double r;
	self->vector.PolarCoords(&r,  NULL,  NULL);
	return r;
}
-(double)getMagnitude{
	return self->vector.Magnitude();
}
-(double)getTheta{
	double t;
	self->vector.SphericalCoords(NULL, &t, NULL);
	return t;
}
-(double)getPHI{
	double p;
	self->vector.SphericalCoords(NULL, NULL, &p);
	return p;
}
//Manipulations
-(XPNVector*)sumWithVector:(XPNVector*)vec{
	return [XPNVector vectorWithVector:(XPINSScriptableMath::Vector::Add(self->vector, vec->vector))];
}
-(XPNVector*)scale:(double)scale{
	return [XPNVector vectorWithVector:(XPINSScriptableMath::Vector::Scale(self->vector, scale))];
}
-(double)dotProductWithVector:(XPNVector*)vec{
	return XPINSScriptableMath::Vector::DotProduct(self->vector, vec->vector);
}
-(XPNVector*)crossProductWithVector:(XPNVector*)vec{
	return [XPNVector vectorWithVector:(XPINSScriptableMath::Vector::CrossProduct(self->vector, vec->vector))];
}
-(XPNVector*)projectionInDirectionOfVector:(XPNVector*)vec{
	double a,b;
	vec->vector.SphericalCoords(NULL, &a, &b);
	return [XPNVector vectorWithVector:(XPINSScriptableMath::Vector::ProjectionInDirection(self->vector, a,b))];
}

@end

@implementation XPNMatrix

//Creation
+(XPNMatrix*) matrixWithMatrix:(XPINSScriptableMath::Matrix)mat{
	XPNMatrix* matrix=[[XPNMatrix alloc] init];
	matrix->matrix=mat;
	return matrix;
}
+(XPNMatrix*) matirxWithHeight:(int)rows Width:(int)cols{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix(rows,cols)];
}
+(XPNMatrix*) identityMatrixOfSize:(int)size{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix::IdentityMatrixOfSize(size)];
}
+(XPNMatrix*) rotationMatrixWithAngle:(double)angle aroundVector:(XPNVector*)vec{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix::RotationMatrixWithAngleAroundVector(vec->vector, angle)];
}
+(XPNMatrix*) matrixWithVector:(XPNVector*)vec{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix::MatrixForVector(vec->vector)];
}
//Getting/Setting Values
-(void)setValue:(double)val forRow:(int)row Column:(int)col{
	self->matrix.SetValueAtPosition(val, row, col);
}
-(double)getValueForRow:(int)row Column:(int)col{
	return self->matrix.ValueAtPosition(row, col);
}
//Matrix operations
-(XPNMatrix*)sumWithMatrix:(XPNMatrix*)mat{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix::Add(self->matrix, mat->matrix)];
}
-(XPNMatrix*)scale:(double)scale{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix::Scale(self->matrix,scale)];
}
-(XPNMatrix*)multiplyWithMatrix:(XPNMatrix*)mat{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix::Multiply(self->matrix, mat->matrix)];
}
-(XPNMatrix*)invert{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix::Invert(self->matrix)];
}
-(double)determinant{
	return XPINSScriptableMath::Matrix::Determinant(self->matrix);
}
-(XPNMatrix*)transpose{
	return [XPNMatrix matrixWithMatrix:XPINSScriptableMath::Matrix::Transpose(self->matrix)];
}

@end
