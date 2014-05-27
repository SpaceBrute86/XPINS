//
//  XPNMath.m
//  XPINS
//
//  Created by Robbie Markwick on 5/4/14.
//
//

#import "XPNMath.h"
using namespace XPINSScriptableMath;

@implementation XPNVector
//Creating Vectors
+(XPNVector*)vectorWithVector:(Vector)vector{
	XPNVector* v=[XPNVector alloc];
	v->vector=vector;
	return v;
}
+(XPNVector*)vectorWithX:(double)x Y:(double)y Z:(double)z{
	return [XPNVector vectorWithVector: Vector (x, y,z)];
}
+(XPNVector*)polarVectorWithR:(double)r Theta:(double)t Z:(double)z{
	return [XPNVector vectorWithVector:Vector::PolarVector(r, t,z)];
}
+(XPNVector*)sphericalVectorWithRho:(double)r Theta:(double)t PHI:(double)p{
	return [XPNVector vectorWithVector:Vector::SphericalVector(r, t,p)];
}
+(XPNVector*) vectorWithMatrix:(XPNMatrix*)mat{
	return [XPNVector vectorWithVector:Matrix::VectorForMatrix(mat->matrix)];
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
	return [XPNVector vectorWithVector:(Vector::Add(self->vector, vec->vector))];
}
-(XPNVector*)scale:(double)scale{
	return [XPNVector vectorWithVector:(Vector::Scale(self->vector, scale))];
}
-(double)dotProductWithVector:(XPNVector*)vec{
	return Vector::DotProduct(self->vector, vec->vector);
}
-(XPNVector*)crossProductWithVector:(XPNVector*)vec{
	return [XPNVector vectorWithVector:(Vector::CrossProduct(self->vector, vec->vector))];
}
-(XPNVector *) multiplyWithMatrix:(XPNMatrix *)matrix{
	return [XPNVector vectorWithVector:Matrix::MVMultiply(matrix->matrix, vector)];
}
-(XPNVector*)projectoinOntoVector:(XPNVector*)vector{
	return [XPNVector vectorWithVector:(Vector::ProjectionOntoVector(self->vector, vector->vector))];
}
-(double)angleFromVector:(XPNVector*)vec{
	return Vector::AngleBetweenVectors(self->vector, vec->vector);
}
@end

@implementation XPNMatrix

//Creation
+(XPNMatrix*) matrixWithMatrix:(Matrix)mat{
	XPNMatrix* matrix=[[XPNMatrix alloc] init];
	matrix->matrix=mat;
	return matrix;
}
+(XPNMatrix*) matirxWithHeight:(int)rows Width:(int)cols{
	return [XPNMatrix matrixWithMatrix:Matrix(rows,cols)];
}
+(XPNMatrix*) identityMatrixOfSize:(int)size{
	return [XPNMatrix matrixWithMatrix:Matrix::IdentityMatrixOfSize(size)];
}
+(XPNMatrix*) rotationMatrixWithAngle:(double)angle aroundVector:(XPNVector*)vec{
	return [XPNMatrix matrixWithMatrix:Matrix::RotationMatrixWithAngleAroundVector(vec->vector, angle)];
}
+(XPNMatrix*) matrixWithVector:(XPNVector*)vec{
	return [XPNMatrix matrixWithMatrix:Matrix::MatrixForVector(vec->vector)];
}
-(id)copy{
	return [XPNMatrix matrixWithMatrix:self->matrix.Copy()];
}
-(id)copyWithZone:(NSZone *)zone{
	return [XPNMatrix matrixWithMatrix:self->matrix.Copy()];
}
//Getting/Setting Values
-(void)setValue:(double)val forRow:(int)row Column:(int)col{
	self->matrix.SetValueAtPosition(val, row, col);
}
-(double)getValueForRow:(int)row Column:(int)col{
	return self->matrix.ValueAtPosition(row, col);
}
//Matrix operations
-(void)appendMatrix:(XPNMatrix*)mat{
	self->matrix=Matrix::Append(self->matrix, mat->matrix);
}
-(XPNMatrix*)sumWithMatrix:(XPNMatrix*)mat{
	return [XPNMatrix matrixWithMatrix:Matrix::Add(self->matrix, mat->matrix)];
}
-(XPNMatrix*)scale:(double)scale{
	return [XPNMatrix matrixWithMatrix:Matrix::Scale(self->matrix,scale)];
}
-(XPNMatrix*)multiplyWithMatrix:(XPNMatrix*)mat{
	return [XPNMatrix matrixWithMatrix:Matrix::Multiply(self->matrix, mat->matrix)];
}
-(XPNMatrix*)invert{
	return [XPNMatrix matrixWithMatrix:Matrix::Invert(self->matrix)];
}
-(double)determinant{
	return Matrix::Determinant(self->matrix);
}
-(XPNMatrix*)transpose{
	return [XPNMatrix matrixWithMatrix:Matrix::Transpose(self->matrix)];
}
-(XPNMatrix*)rowEchelon{
	return [XPNMatrix matrixWithMatrix:Matrix::RowEchelon(matrix)];
}
-(XPNMatrix*)reducedRowEchelon{
	return [XPNMatrix matrixWithMatrix:Matrix::ReducedRowEchelon(matrix)];
}

@end


@implementation XPNPolynomial
-(instancetype)init{
	self=[super init];
	self->polynomial=Polynomial();
	return self;
}
//Creation
+(XPNPolynomial *)polynomialWithPolynomial:(XPINSScriptableMath::Polynomial)poly{
	XPNPolynomial* polynomial=[[XPNPolynomial alloc] init];
	polynomial->polynomial=poly;
	return polynomial;
}
+(XPNPolynomial*) monomialWithCoefficient:(double)coeff xPower:(int)x yPower:(int)y zPower:(int)z tPower:(int)t {
	vector<Polynomial::Monomial> mon=vector<Polynomial::Monomial>(1);
	mon[0].coeff=coeff;
	mon[0].exponents.resize(4);
	mon[0].exponents[0]=x;
	mon[0].exponents[1]=y;
	mon[0].exponents[2]=z;
	mon[0].exponents[3]=t;
	return [XPNPolynomial polynomialWithPolynomial:Polynomial(mon)];
}
-(id)copy{
	return [XPNPolynomial polynomialWithPolynomial:self->polynomial.Copy()];
}
-(id)copyWithZone:(NSZone *)zone{
	return [XPNPolynomial polynomialWithPolynomial:self->polynomial.Copy()];
}
//Arithmetic
-(XPNPolynomial*)sumWithPolynomial:(XPNPolynomial*)poly{
	return [XPNPolynomial polynomialWithPolynomial:Polynomial::Add(polynomial, poly->polynomial)];
}
-(XPNPolynomial*)scale:(double)scale{
	return [XPNPolynomial polynomialWithPolynomial:Polynomial::Scale(polynomial, scale)];
}
-(XPNPolynomial*)multiplyByPolynomial:(XPNPolynomial*)poly{
	return [XPNPolynomial polynomialWithPolynomial:Polynomial::Multiply(polynomial, poly->polynomial)];
}
-(XPNPolynomial*)raiseToPower:(int)pow{
	return [XPNPolynomial polynomialWithPolynomial:Polynomial::Power(polynomial, pow)];
}
-(XPNPolynomial*)compositionWithPolynomial:(XPNPolynomial*)poly forVar:(int) var{
	return [XPNPolynomial polynomialWithPolynomial:Polynomial::Composition(polynomial, poly->polynomial,var)];
}
//Evaluation
-(double)evaluationWithX:(double)x Y:(double)y Z:(double)z T:(double)t{
	vector<double> vars=vector<double>(4);
	vars[0]=x;
	vars[1]=y;
	vars[2]=z;
	vars[3]=t;
	return polynomial.Evaluate(vars);
}
-(XPNPolynomial*)partialEvaluationWithValue:(double)val forVar:(int) var{
	return [XPNPolynomial polynomialWithPolynomial:polynomial.PartialEvaluate(val,var)];
}
//Calculus
-(XPNPolynomial*)derivativeForVar:(int)var{
	return [XPNPolynomial polynomialWithPolynomial:Polynomial::Derivative(polynomial, var)];
}
-(XPNPolynomial*)integralForVar:(int)var{
	return [XPNPolynomial polynomialWithPolynomial:Polynomial::Integrate(polynomial, var)];
}

@end



