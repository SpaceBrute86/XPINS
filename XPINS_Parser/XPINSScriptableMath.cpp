//Copyright (c) 2013 Robert Markwick
//See the file license.txt for Copying permission


#include "XPINSScriptableMath.h"
#include <math.h>
#include "XPINSMathModule.h"
#include <time.h>

using namespace XPINSScriptableMath;

bool seeded=false;

#pragma mark Probability Library

double XPINSScriptableMath::Probability::NormalRV(double mu, double sigma)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::RANDOMVAR))return mathMod->NormalRV(mu, sigma);
	if(!seeded)
	{
		srand(time(NULL));
		seeded=true;
	}
	double CDF=((double)rand())/(RAND_MAX);//Random between 0 and 1
	double devs=(CDF<0.5?-1:1)*sqrt(2*log(0.25/(CDF-pow(CDF, 2))));
	return mu+sigma*devs;
}

bool XPINSScriptableMath::Probability::BernoulliRV(double p)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::RANDOMVAR))return mathMod->BernoulliRV(p);
	if(!seeded)
	{
		srand(time(NULL));
		seeded=true;
	}
	double CDF=((double)rand())/(RAND_MAX);//Random between 0 and 1
	return CDF<p;
}
double XPINSScriptableMath::Probability::UniformRV(double low,double length)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::RANDOMVAR))return mathMod->UniformRV(low,length);
	if(!seeded)
	{
		srand(time(NULL));
		seeded=true;
	}
	double P=((double)rand())/(RAND_MAX);//Random between 0 and 1
	return P*length+low;
}

double XPINSScriptableMath::Probability::ExponentialRV(double lambda)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::RANDOMVAR))return mathMod->ExponentialRV(lambda);
	if(!seeded)
	{
		srand(time(NULL));
		seeded=true;
	}
	double CDF=((double)rand())/(RAND_MAX);//Random between 0 and 1
	double t=-log((1.0-CDF));
	return t/lambda;
}
int XPINSScriptableMath::Probability::PoissonRV(double lambda)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::RANDOMVAR))return mathMod->PoissonRV(lambda);
	int k=0;
	for (double time=0; time<1; ++k)
	{
		time+=ExponentialRV(lambda);
	}
	--k;
	return k;
}

int XPINSScriptableMath::Probability::CoinFlip(double p,int n)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::EXPERIMENT))return mathMod->CoinFlip(p, n);
	int sum=0;
	for (int i=0; i<n; ++i)
	{
		if (BernoulliRV(p))++sum;
	}
	return sum;
}
int  XPINSScriptableMath::Probability::FairDiceRoll(int sides,int n)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::EXPERIMENT))return mathMod->FairDiceRoll(sides, n);
	int sum=0;
	for (int i=0; i<n; ++i)
	{
		sum+=(int)UniformRV(1, sides);
	}
	return sum;
}
double XPINSScriptableMath::Probability::SimulateMarkovChain(Matrix chain, int steps, int start)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MARKOVL1))return mathMod->SimulateMarkovChain(chain,steps, start);
	Matrix markov=chain.Copy();
	int state=start;
	for (int i=0; i<steps; ++i)
	{
		double P=UniformRV(0.0, 1.0);
		for (int j=0; j<markov.GetRows(); ++j)
		{
			P-=markov.ValueAtPosition(j, state);
			if(P<=0)
			{
				state=j;
				break;
			}
		}
	}
	return state;
}
double XPINSScriptableMath::Probability::TransitionProbability(Matrix chain, int steps, int start, int stop)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MARKOVL1))return mathMod->TransitionProbability(chain,steps, start,stop);
	Matrix markov=chain.Copy();
	Matrix prob=markov.Copy();
	for (int i=1; i<steps; ++i)
	{
		prob*=markov;
	}
	return prob.ValueAtPosition(stop, start);
}
double XPINSScriptableMath::Probability::SteadyStateProbability(Matrix chain, int state)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MARKOVL2))return mathMod->SteadyStateProbability(chain, state);
	Matrix markov=chain.Copy();
	Matrix constants=Matrix(markov.GetRows(), 1);
	Matrix system=Matrix::Append(Matrix::IdentityMatrixOfSize(markov.GetRows())-markov, constants);
	for (int i=0; i<system.GetCols(); ++i)
	{
		system.SetValueAtPosition(1, 0, i);
	}
	Matrix solution=Matrix::ReducedRowEchelon(system);
	return solution.ValueAtPosition(state, solution.GetCols()-1);
}
double XPINSScriptableMath::Probability::AbsorbtionProbability(Matrix chain, int state,int start)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MARKOVL3))return mathMod->AbsorbtionProbability(chain, state,start);
	Matrix markov=Subchain(chain, state, start,false);
	return SteadyStateProbability(markov, state);
}
double XPINSScriptableMath::Probability::AbsorbtionTime(Matrix chain, int state,int start)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MARKOVL3))return mathMod->AbsorbtionTime(chain, state,start);
	Matrix markov=Subchain(chain, state, start,true);
	Matrix constants=Matrix(markov.GetRows(), 1,1.0);
	Matrix system=Matrix::Append(Matrix::IdentityMatrixOfSize(markov.GetRows())-markov, constants);
	Matrix solution=Matrix::ReducedRowEchelon(system);
	return solution.ValueAtPosition(state, solution.GetCols()-1);
}
int XPINSScriptableMath::Probability::SimulateAbsorbtionTime(Matrix chain, int state,int start)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MARKOVL3))return mathMod->SimulateAbsorbtionTime(chain, state,start);
	Matrix markov=Subchain(chain, state, start,true);
	int time=0;
	for (int current=start; current!=state; ++time)
	{
		double P=UniformRV(0.0, 1.0);
		for (int j=0; j<markov.GetRows(); ++j)
		{
			P-=markov.ValueAtPosition(j, current);
			if(P<=0)
			{
				current=j;
				break;
			}
		}
	}
	return time;
}
bool XPINSScriptableMath::Probability::Reachable(Matrix chain, int start, int stop)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MARKOVL2))return mathMod->Reachable(chain, start,stop);
	Matrix markov=chain.Copy();
	if(markov.ValueAtPosition(start, stop)!=0)  return true;
	Matrix prob=markov;
	for (int i=1; i<markov.GetCols(); ++i)
	{
		prob*=markov;
		if(prob.ValueAtPosition(start, stop)!=0)  return true;
	}
	return false;
}
Matrix XPINSScriptableMath::Probability::Subchain(Matrix chain,int& state,int& start,bool mustReach)
{
	Matrix markov=chain.Copy();
	Matrix tempMarkov=markov.Copy();
	int pathPoints=0;
	for (int i=0; i<markov.GetRows(); ++i) {
		if(!Reachable(markov, start, i)||(mustReach&&!Reachable(markov, i, state)))
		{
			for (int j=0; j<tempMarkov.GetRows(); ++j)
			{
				tempMarkov.SetValueAtPosition(0, i, j);
				tempMarkov.SetValueAtPosition(0, j, i);
			}
		}
		else ++pathPoints;
	}
	Matrix smallMarkov=Matrix(pathPoints,tempMarkov.GetCols());
	int skipped=0;
	for (int i=0; i<tempMarkov.GetRows(); ++i)
	{
		bool skip=true;
		for (int j=0; j<tempMarkov.GetCols(); ++j)
		{
			if(tempMarkov.ValueAtPosition(i, j)!=0)
			{
				skip=false;
				break;
			}
		}
		if(skip)
		{
			++skipped;
			if(i-skipped<start)--start;
			if(i-skipped<state)--state;
		}
		else
		{
			for (int j=0; j<tempMarkov.GetCols(); ++j)
			{
				smallMarkov.SetValueAtPosition(tempMarkov.ValueAtPosition(i, j), (i-skipped), j);
			}
		}
	}
	Matrix reducedMarkov=Matrix(pathPoints,pathPoints);
	skipped=0;
	for (int i=0; i<smallMarkov.GetCols(); ++i)
	{
		bool skip=true;
		for (int j=0; j<smallMarkov.GetRows(); ++j)
		{
			if(smallMarkov.ValueAtPosition(j, i)!=0)
			{
				skip=false;
				break;
			}
		}
		if(skip)  ++skipped;
		else
		{
			for (int j=0; j<smallMarkov.GetRows(); ++j)
			{
				smallMarkov.SetValueAtPosition(smallMarkov.ValueAtPosition(j, i), j,(i-skipped));
			}
		}
	}
	for (int i=0; i<reducedMarkov.GetCols(); ++i) //Normalize Probabilities
	{
		int sum=0;
		for (int j=0; j<reducedMarkov.GetRows(); ++j)
		{
			sum+=reducedMarkov.ValueAtPosition(j, i);
		}
		for (int j=0; j<reducedMarkov.GetRows(); ++j)
		{
			reducedMarkov.SetValueAtPosition(reducedMarkov.ValueAtPosition(j, i)/sum,j, i);
		}
	}
	return reducedMarkov;
}

#pragma mark Vector Library

//Initializing Vectors
XPINSScriptableMath::Vector::Vector()
{
	this->x=0;
	this->y=0;
	this->z=0;
}
XPINSScriptableMath::Vector::Vector(double x, double y,double z, coordSystem system)
{
	switch (system)
	{
		case Cartesian:
			this->x=x;
			this->y=y;
			this->z=z;
			break;
		case Polar:
			this->x=x*cos(y);
			this->y=x*sin(y);
			this->z=z;
			break;
		case Spherical:
			this->x=x*cos(y)*sin(z);
			this->y=x*sin(y)*sin(z);
			this->z=x*cos(z);
			break;
	}
	
}
//Vector Components Values
double Vector::Magnitude()
{
	return sqrtf(powf(x, 2)+powf(y, 2)+powf(z,2));
}
void Vector::Coords(double* a,double* b,double* c,coordSystem system)
{
	switch (system)
	{
		case Cartesian:
			if(a!=NULL)*a=x;
			if(b!=NULL)*b=y;
			if(c!=NULL)*c=z;
			break;
		case Polar:
			if(a!=NULL)*a=sqrt(pow(x,2)+pow(y, 2));
			if(b!=NULL)*b=atan2(y, x);
			if(c!=NULL)*c=z;
			break;
		case Spherical:
			if(a!=NULL)*a=sqrt(pow(x,2)+pow(y, 2)+pow(z, 2));
			if(b!=NULL)*b=atan2(y, x);
			if(c!=NULL)*c=atan2(sqrt(pow(x,2)+pow(y, 2)), z);
			break;
	}
}

//Vector Arithmetic
Vector Vector::Add(Vector vec1, Vector vec2)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::VECL1))return mathMod->Add(vec1, vec2);
	double x=vec1.x+vec2.x;
	double y=vec1.y+vec2.y;
	double z=vec1.z+vec2.z;
	return Vector(x,y,z, Cartesian);
}
Vector Vector::Scale(Vector vec, double scale)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::VECL1))return mathMod->Scale(vec, scale);
	return Vector(vec.x*scale,vec.y*scale,vec.z*scale, Cartesian);
}
double Vector::DotProduct(Vector a,Vector b)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::VECL1))return mathMod->DotProduct(a, b);
	double x=a.x*b.x;
	double y=a.y*b.y;
	double z=a.z*b.z;
	return x+y+z;
}
Vector Vector::CrossProduct(Vector a,Vector b)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::VECL1))return mathMod->CrossProduct(a, b);
	double x=(a.y*b.z) - (a.z*b.y);
	double y=(a.z*b.x) - (a.x*b.z);
	double z=(a.x*b.y) - (a.y*b.x);
	return Vector(x,y,z, Cartesian);
}
//Miscillaneous Vector Functions
double Vector::AngleBetweenVectors(Vector vec1, Vector vec2)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::VECL2))return mathMod->AngleBetweenVectors(vec1, vec2);
	double dot=DotProduct(vec1,vec2);
	double cos=dot/(vec1.Magnitude()*vec2.Magnitude());
	return acos(cos);
}
Vector Vector::ProjectionOntoVector(Vector vec1,Vector vec2)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::VECL2))return mathMod->ProjectionOntoVector(vec1, vec2);
	double dot=DotProduct(vec1, vec2);
	Vector result=Scale(vec2, dot/pow(vec2.Magnitude(),2));
	return result;
}
Vector Vector::UnitVectorFromVector(Vector vec)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::VECL2))return mathMod->UnitVectorFromVector(vec);
	return vec/vec.Magnitude();
}
//Related Scalar functions
double XPINSScriptableMath::Vector::AddPolar(double x,double y)
{
	double res=x+y;
	while (res<0) res+=2*M_PI;
	while (res>=2*M_PI) res-=2*M_PI;
	return res;
}
double XPINSScriptableMath::Vector::Dist(double x, double y,double z)
{
	return sqrtf(x*x+y*y+z*z);
}

#pragma mark Quaternion Library

//Initializing Quaternions
XPINSScriptableMath::Quaternion::Quaternion()
{
	this->r=0;
	this->v=Vector();
}
XPINSScriptableMath::Quaternion::Quaternion (double r, Vector v)
{
	this->r=r;
	this->v=v;
}
//Quaternion Components
double Quaternion::Magnitude()
{
	return sqrtf(pow(v.x,2)+pow(v.y,2)+pow(v.z,2)+pow(r, 2));
}
void Quaternion::Components(double* a,double* b,double* c,double* d)
{
	if(a!=NULL)*a=v.x;
	if(b!=NULL)*b=v.y;
	if(c!=NULL)*c=v.z;
	if(d!=NULL)*d=r;
}
//Quaternion Arithmetic
Quaternion Quaternion::Add(Quaternion quat1, Quaternion quat2)
{
	double r=quat1.r+quat2.r;
	Vector v=quat1.v+quat2.v;
	return Quaternion(r,v);
}
Quaternion Quaternion::Scale(Quaternion quat, double scale)
{
	double r=quat.r*scale;
	Vector v=quat.v*scale;
	return Quaternion(r, v);
}
Quaternion Quaternion::Multiply(Quaternion quat1,Quaternion quat2)
{
	double r=quat1.r*quat2.r-Vector::DotProduct(quat1.v, quat2.v);
	Vector v=quat1.v*quat2.r+quat2.v*quat1.r+Vector::CrossProduct(quat1.v, quat2.v);
	return Quaternion(r,v);
}
//Other Quaternion Functions
Quaternion Quaternion::ConjugateQuaternion(Quaternion quat)
{
	double r=quat.r;
	Vector v=quat.v*-1;
	return Quaternion(r,v);
}
Quaternion Quaternion::UnitQuaternion(Quaternion quat)
{
	return quat/quat.Magnitude();
}
Quaternion Quaternion::InvertQuaternion(Quaternion quat)
{
	double magSquared=pow(quat.v.x,2)+pow(quat.v.y,2)+pow(quat.v.z,2)+pow(quat.r, 2);
	return ConjugateQuaternion(quat)/magSquared;
}
Vector Quaternion::RotateVector(Quaternion quat,Vector vec)
{
	Quaternion vq=Quaternion(0,vec);
	Quaternion inv=InvertQuaternion(quat);
	Quaternion product=quat*vq*inv;
	return product.v;
}

#pragma mark Matrix Library

//Creating, Copying, and Deleting Matrices
Matrix::Matrix ()
{
	values=new double[1];
	values[0]=0.0;
	rows=1;
	cols=1;
}

Matrix::Matrix (size_t r, size_t c)
{
	values=new double[r*c];
	for (int i=0; i<r*c; ++i)  values[i]=0.0;
	rows=r;
	cols=c;
}
Matrix::Matrix (size_t r, size_t c, double val)
{
	values=new double[r*c];
	for (int i=0; i<r*c; ++i)  values[i]=val;
	rows=r;
	cols=c;
}
Matrix::~Matrix()
{
	//delete[] values;
}
void Matrix::Clear()
{
	delete[] values;
}
Matrix Matrix::Copy()
{
	Matrix *m=new Matrix(rows,cols);
	for (int i=0; i<rows; ++i)
	{
		for (int j=0; j<cols; ++j)
		{
			m->values[i*cols+j]=values[i*cols+j];
		}
	}
	return *m;
}
Matrix Matrix::IdentityMatrixOfSize(size_t size)
{
	Matrix matrix=Matrix(size,size);
	for(size_t i=0;i<size;++i)
	{
		matrix.values[i*size+i]=1;
	}
	return matrix;
}
Matrix Matrix::RotationMatrixWithAngleAroundVector(Vector vec,double angle){
	Matrix matrix=Matrix(3,3);
	//get Unit Vector comonents
	Vector unitVec=Vector::UnitVectorFromVector(vec);
	double x=0,y=0,z=1;
	unitVec.Coords(&x, &y, &z, Vector::Cartesian);
	//Compute values
	double cosine=cos(angle);
	double sine=sin(angle);
	//Source for Formulas: http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations
	matrix.values[0]=cosine+x*x*(1-cosine);
	matrix.values[1]=z*sine+x*y*(1-cosine);
	matrix.values[2]=-y*sine+x*z*(1-cosine);
	matrix.values[3]=-z*sine+x*y*(1-cosine);
	matrix.values[4]=cosine+y*y*(1-cosine);
	matrix.values[5]=x*sine+z*y*(1-cosine);
	matrix.values[6]=y*sine+x*z*(1-cosine);
	matrix.values[7]=-x*sine+z*y*(1-cosine);
	matrix.values[8]=cosine+z*z*(1-cosine);
	return matrix;
}
/*
 Roll, Pitch, and Yaw
 Roll around X, 
 then Pitch around Y,
 then Yaw around Z
 */
Matrix Matrix::RotationMatrixWithEulerAngles(double roll,double pitch, double yaw)
{
	Matrix matrix=Matrix(3,3);
	//Sines and Cosines
	double sr=sin(roll);
	double cr=cos(roll);
	double sp=sin(pitch);
	double cp=cos(pitch);
	double sy=sin(yaw);
	double cy=cos(yaw);
	//Compute values
	matrix.values[0]=cp*cy;
	matrix.values[1]=-cr*sy-sp*sr*cy;
	matrix.values[2]=sr*sy-sp*cr*cy;
	matrix.values[3]=cp*sy;
	matrix.values[4]=cr*cy-sp*sr*sy;
	matrix.values[5]=-sp*cr*sy-sr*cy;
	matrix.values[6]=sp;
	matrix.values[7]=cp*sr;
	matrix.values[8]=cp*cr;
	return matrix;
}
Matrix Matrix::RotationMatrixWithQuaternion(Quaternion quat){
	Matrix matrix=Matrix(3,3);
	//get Unit Quaternion components
	Quaternion unitQuat=Quaternion::UnitQuaternion(quat);
	double a,b,c,d;
	unitQuat.Components(&b, &c, &d, &a);
	//Compute values
	//Source for Formulas: http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
	matrix.values[0]=a*a+b*b-c*c-d*d;
	matrix.values[1]=2*(b*c-a*d);
	matrix.values[2]=2*(b*d+a*c);
	matrix.values[3]=2*(b*c+a*d);
	matrix.values[4]=a*a-b*b+c*c-d*d;
	matrix.values[5]=2*(c*d-a*b);
	matrix.values[6]=2*(b*d-a*c);
	matrix.values[7]=2*(c*d+a*b);
	matrix.values[8]=a*a-b*b-c*c+d*d;
	return matrix;
}
//Accessing Values
size_t Matrix::GetRows()
{
	return rows;
}
size_t Matrix::GetCols()
{
	return cols;
}
double Matrix::ValueAtPosition(size_t i,size_t j)
{
	return values[i*cols+j];
}
void Matrix::SetValueAtPosition(double val,size_t i,size_t j)
{
	values[i*cols+j]=val;
}
// Matrix/Vector Conversion
Vector Matrix::VectorForMatrix(Matrix m)
{
	if(m.rows==0)return Vector(0,0,0, Vector::Cartesian);
	return Vector(m.values[0],m.rows<1?0:m.values[1],m.rows<2?0:m.values[2], Vector::Cartesian);
}
Matrix Matrix::MatrixForVector(Vector v)
{
	Matrix m=Matrix(3,1);
	m.values[0]=v.x;
	m.values[1]=v.y;
	m.values[2]=v.z;
	return m;
}
//Matrix Operations
Matrix Matrix::Append(Matrix ma,Matrix mb)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL1))return mathMod->Append(ma, mb);
	Matrix a=ma.Copy(),b=mb.Copy();
	if (a.rows!=b.rows) return Matrix(a.rows,a.cols+b.cols); //Error workaround
	//Calculation
	Matrix m=Matrix(a.rows,a.cols+b.cols);
	for (size_t i=0; i<a.rows; ++i)
	{
		for (size_t j=0; j<a.cols+b.cols; ++j)
		{
			m.values[i*m.cols+j]=j<a.cols?a.values[i*a.cols+j]:b.values[i*b.cols+j-a.cols];
		}
	}
	return m.Copy();
}
Matrix Matrix::Add(Matrix ma,Matrix mb)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL1))return mathMod->Add(ma, mb);
	Matrix a=ma.Copy(),b=mb.Copy();
	if (a.rows!=b.rows||a.cols!=b.cols)  return Matrix(a.rows,a.cols); //Error workaround
	//Calculation
	for (size_t i=0; i<a.rows*a.cols; ++i)
	{
		a.values[i]+=b.values[i];
	}
	return a;
}

Matrix Matrix::Scale(Matrix ma,double b)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL1))return mathMod->Scale(ma, b);
	Matrix a=ma.Copy();
	for (size_t i=0; i<a.rows*a.cols; ++i)
	{
		a.values[i]*=b;
	}
	return a;
}
Matrix Matrix::Multiply(Matrix ma,Matrix mb)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL2))return mathMod->Multiply(ma, mb);
	Matrix a=ma,b=mb;
	if (a.cols!=b.rows)return Matrix(a.rows,b.cols);//Error Workaround
	Matrix m=Matrix(a.rows,b.cols);
	for (size_t i=0; i<a.rows; ++i)
	{
		for (size_t p=0; p<a.cols; ++p)
		{
			for (size_t j=0; j<b.cols; ++j)
			{
				m.values[i*m.cols+j]+=a.values[i*a.cols+p]*b.values[p*b.cols+j];
			}
		}
	}
	return m;
}
Matrix Matrix::Power(Matrix A,int p)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL2))return mathMod->Power(A, p);
	Matrix a=A.Copy();
	if(p==0)return IdentityMatrixOfSize(a.rows);
	if(p==1)return a;
	int s=p/2;
	Matrix m=Power(A*A, s);
	if(p%2==1)m*=A;
	return m;
}
Vector Matrix::MVMultiply(Matrix m,Vector v)
{
	Matrix vectorMatrix=MatrixForVector(v);
	Matrix resultVectorMatrix=Multiply(m, vectorMatrix);
	return VectorForMatrix(resultVectorMatrix);
}
Matrix Matrix::Transpose(Matrix i)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL1))return mathMod->Transpose(i);
	Matrix in=i.Copy();
	Matrix out=Matrix(in.cols,in.rows);
	for (size_t i=0; i<in.rows; ++i)
	{
		for (size_t j=0; j<in.cols; ++j)
		{
			out.values[j*out.cols+i]=in.values[i*in.cols+j];
		}
	}
	return out;
}
double Matrix::Determinant(Matrix a)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL3))return mathMod->Determinant(a);
	Matrix ma=a.Copy();
	double det=1;
	for (int i=0; i<ma.rows; ++i)
	{
		if(ma.values[i*ma.cols+i]==0)//Row Swapping
		{
			int swapRow=i;
			for (;ma.values[swapRow*ma.cols+i]==0&&swapRow<ma.rows; ++swapRow);
			if (swapRow!=ma.rows)//Row Swap can work
			{
				for (int j=i; j<ma.cols; ++j)
				{
					double temp=ma.values[i*ma.cols+j];
					ma.values[i*ma.cols+j]=ma.values[swapRow*ma.cols+j];
					ma.values[swapRow*ma.cols+j]=temp;
					det*=-1;
				}
			}
			else return 0;
		}
		if(ma.values[i*ma.cols+i]!=0)
		{
			//Adjust other rows
			for (int j=i+1; j<ma.rows; ++j)
			{
				double m=ma.values[j*ma.cols+i]/ma.values[i*ma.cols+i];
				for (int k=i; k<ma.cols; ++k)
				{
					ma.values[j*ma.cols+k]-=m*ma.values[i*ma.cols+k];
				}
			}
		}
	}
	for (int i=0; i<ma.rows; ++i)
	{
		det*=ma.values[i*ma.cols+i];
	}
	return det;
}
Matrix Matrix::Invert(Matrix a)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL3))return mathMod->Invert(a);
	Matrix ma=a.Copy();
	if (ma.rows!=ma.cols) return Matrix(a.cols,a.rows);
	Matrix* mb=new Matrix(a.rows,a.cols);
	*mb=IdentityMatrixOfSize(a.cols);
	for (int i=0; i<ma.rows; ++i)
	{
		if(ma.values[i*ma.cols+i]==0)//Row Swapping
		{
			int swapRow=i;
			for (;ma.values[swapRow*ma.cols+i]==0&&swapRow<ma.rows; ++swapRow);
			if (swapRow!=ma.rows)//Row Swap can work
			{
				for (int j=i; j<ma.cols; ++j)
				{
					double temp=ma.values[i*ma.cols+j];
					ma.values[i*ma.cols+j]=ma.values[swapRow*ma.cols+j];
					ma.values[swapRow*ma.cols+j]=temp;
				}
				for (int j=0; j<mb->cols; ++j)
				{
					double temp=mb->values[i*mb->cols+j];
					mb->values[i*mb->cols+j]=mb->values[swapRow*mb->cols+j];
					mb->values[swapRow*mb->cols+j]=temp;
				}
			}
			else return Matrix(ma.rows,ma.cols);
		}
		if(ma.values[i*ma.cols+i]!=0)
		{
			//Adjust other rows
			for (int j=0; j<ma.rows; ++j)
			{
				if(j==i)++j;
				if(j>=ma.rows)break;
				double m=ma.values[j*ma.cols+i]/ma.values[i*ma.cols+i];
				for (int k=i; k<ma.cols; ++k)
				{
					ma.values[j*ma.cols+k]-=m*ma.values[i*ma.cols+k];
				}
				for (int k=0; k<ma.cols; ++k)
				{
					mb->values[j*mb->cols+k]-=m*mb->values[i*mb->cols+k];
				}
			}
			//divide current row
			double k=ma.values[i*ma.cols+i];
			for (int j=i; j<ma.cols; ++j)
			{
				ma.values[i*ma.cols+j]/=k;
			}
			for (int j=0; j<mb->cols; ++j)
			{
				mb->values[i*mb->cols+j]/=k;
			}
		}
	}
	return *mb;
}
Matrix Matrix::RowEchelon(Matrix a)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL3))return mathMod->RowEchelon(a);
	Matrix ma=a.Copy();
	for (int i=0; i<ma.rows; ++i)
	{
		if(ma.values[i*ma.cols+i]==0)//Row Swapping
		{
			int swapRow=i;
			for (;ma.values[swapRow*ma.cols+i]==0&&swapRow<ma.rows; ++swapRow);
			if (swapRow!=ma.rows) //Row Swap can work
			{
				for (int j=i; j<ma.cols; ++j)
				{
					double temp=ma.values[i*ma.cols+j];
					ma.values[i*ma.cols+j]=ma.values[swapRow*ma.cols+j];
					ma.values[swapRow*ma.cols+j]=temp;
				}
			}
		}
		if(ma.values[i*ma.cols+i]!=0)
		{
			//Adjust other rows
			for (int j=i+1; j<ma.rows; ++j)
			{
				double m=ma.values[j*ma.cols+i]/ma.values[i*ma.cols+i];
				for (int k=i; k<ma.cols; ++k)
				{
					ma.values[j*ma.cols+k]-=m*ma.values[i*ma.cols+k];
				}
			}
		}
	}
	return ma;
}
Matrix Matrix::ReducedRowEchelon(Matrix a)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::MATL3))return mathMod->ReducedRowEchelon(a);
	Matrix ma=a.Copy();
	for (int i=0; i<ma.rows; ++i)
	{
		if(ma.values[i*ma.cols+i]==0)//Row Swapping
		{
			int swapRow=i;
			for (;ma.values[swapRow*ma.cols+i]==0&&swapRow<ma.rows; ++swapRow);
			if (swapRow!=ma.rows)//Row Swap can work
			{
				for (int j=i; j<ma.cols; ++j)
				{
					double temp=ma.values[i*ma.cols+j];
					ma.values[i*ma.cols+j]=ma.values[swapRow*ma.cols+j];
					ma.values[swapRow*ma.cols+j]=temp;
				}
			}
		}
		if(ma.values[i*ma.cols+i]!=0)
		{
			//Adjust other rows
			for (int j=0; j<ma.rows; ++j)
			{
				if(j==i)++j;
				double m=ma.values[j*ma.cols+i]/ma.values[i*ma.cols+i];
				for (int k=i; k<ma.cols; ++k)
				{
					ma.values[j*ma.cols+k]-=m*ma.values[i*ma.cols+k];
				}
			}
			//divide current row
			double k=ma.values[i*ma.cols+i];
			for (int j=i; j<ma.cols; ++j)
			{
				ma.values[i*ma.cols+j]/=k;
			}
		}
	}
	return ma;
}

#pragma mark Polynomial Library

//Polynomial creation and Copying
Polynomial::Polynomial ()
{
	values=*new vector<Monomial>(1);
}
Polynomial::Polynomial (vector<Monomial> v)
{
	values=v;
}
Polynomial::Polynomial(double num)
{
	Monomial m=Monomial::monomial(num, 0, 0, 0, 0, 0, 0,0);
	values=vector<Monomial>(1,m);
}
void Polynomial::Clear()
{
	values.resize(0);
}
Polynomial::Monomial Polynomial::Monomial::monomial(double coeff,double xp, double yp, double zp, double tp,double up, double vp, double wp)
{
	Monomial m=Monomial();
	m.coeff=coeff;
	m.exponents.resize(7);
	m.exponents[0]=xp;
	m.exponents[1]=yp;
	m.exponents[2]=zp;
	m.exponents[3]=tp;
	m.exponents[4]=up;
	m.exponents[5]=vp;
	m.exponents[6]=wp;
	return m;
}
Polynomial Polynomial::Copy()
{
	Polynomial* result=new Polynomial();
	result->values.resize(values.size());
	for (int i=0; i<values.size(); ++i)
	{
		result->values[i].coeff=values[i].coeff;
		result->values[i].exponents.resize(values[i].exponents.size());
		for (int j=0; j<values[i].exponents.size(); ++j)
		{
			result->values[i].exponents[j]=values[i].exponents[j];
		}
	}
	return *result;
}
Polynomial Polynomial::Add(Polynomial p1, Polynomial p2)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL1))return mathMod->Add(p1, p2);
	Polynomial result=Polynomial();
	result.values.resize(p1.values.size()+p2.values.size());
	for (int i=0; i<p1.values.size(); ++i)
	{
		result.values[i].coeff=p1.values[i].coeff;
		result.values[i].exponents=p1.values[i].exponents;
	}
	for (int i=0; i<p2.values.size(); ++i)
	{
		result.values[i+p1.values.size()].coeff=p2.values[i].coeff;
		result.values[i+p1.values.size()].exponents=p2.values[i].exponents;
	}
	result.Clean();
	return result;
}

Polynomial Polynomial::Scale(Polynomial p1, double s)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL1))return mathMod->Scale(p1, s);
	if(s==0)  return Polynomial();
	Polynomial result=p1.Copy();
	for (int i=0; i<result.values.size(); ++i)
	{
		result.values[i].coeff*=s;
	}
	return result;
}
Polynomial Polynomial::Multiply(Polynomial a1, Polynomial a2){
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL2))return mathMod->Multiply(a1, a2);
	Polynomial* result=new Polynomial();
	Polynomial p1=a1.Copy();
	Polynomial p2=a2.Copy();
	result->values.resize(p1.values.size()*p2.values.size());
	for (int i=0; i<p1.values.size(); ++i)
	{
		for (int j=0; j<p2.values.size(); ++j)
		{
			Monomial m=Monomial();
			m.coeff=p1.values[i].coeff*p2.values[j].coeff;
			m.exponents.resize(p1.values[i].exponents.size()>p2.values[j].exponents.size()?p1.values[i].exponents.size():p2.values[j].exponents.size());
			for (int k=0; k<p1.values[i].exponents.size()||k<p2.values[j].exponents.size(); ++k) {
				int a=(k<p1.values[i].exponents.size())?p1.values[i].exponents[k]:0;
				int b=(k<p2.values[j].exponents.size())?p2.values[j].exponents[k]:0;
				m.exponents[k]=a+b;
			}
			result->values[i*p2.values.size()+j]=m;
		}
	}
	result->Clean();
	return *result;
}
Polynomial Polynomial::Power(Polynomial a,unsigned int b){
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL2))return mathMod->Power(a, b);
	Polynomial p1=a.Copy();
	if(b==0)
	{
		Polynomial p=Polynomial();
		p.values.resize(1);
		p.values[0].coeff=1;
		p.values[0].exponents.resize(0);
		return p;
	}
	if(b==1)return p1;
	int s=b/2;
	Polynomial result=Power(p1*p1, s);
	if(b%2==1)result*=p1;
	return result;
}
Polynomial Polynomial::Composition(Polynomial f,Polynomial g,int var)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL3))return mathMod->Composition(f, g,var);
	Polynomial result=Polynomial();
	for (int i=0; i<f.values.size(); ++i)//For each monomial in F
	{
		Monomial m=f.values[i];
		Polynomial res;
		if(f.values[i].exponents[var]>=1)//Raise G to correct power
		{
			res=g.Copy();
			for (int j=1; j<f.values[i].exponents[var]; ++j)
			{
				res*=g;
			}
		}
		else//Power is 0, replace G with 1
		{
			res=Polynomial();
			res.values.resize(1);
			res.values[0].coeff=1;
			res.values[0].exponents.resize(0);
		}
		m.exponents[var]=0;
		for (int j=0; j<res.values.size(); ++j)//Multiply result by coefficient and other variables
		{
			res.values[j].coeff*=m.coeff;
			if(res.values[j].exponents.size()<m.exponents.size())
				res.values[j].exponents.resize(m.exponents.size());
			for (int k=0; k<res.values[j].exponents.size()&&k<m.exponents.size(); ++k)
			{
				res.values[j].exponents[k]+=m.exponents[k];
			}
		}
		result+=res;
	}
	result.Clean();
	return result;
}

double Polynomial::Evaluate(vector<double> vars)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL3))return mathMod->Evaluate(*this, vars);
	double result=0;
	//Evaluate Variable by Variable
	for (int i=0; i<values.size(); ++i)
	{
		Monomial m=values[i];
		double mRes=m.coeff;
		for (int j=0; j<vars.size()&&j<m.exponents.size(); ++j)
		{
			mRes*=pow(vars[j], m.exponents[j]);
		}
		result+=mRes;
	}
	return result;
}

Polynomial Polynomial::PartialEvaluate(double val, int var)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL3))return mathMod->PartialEvaluate(*this,val, var);
	Polynomial result=Polynomial();
	result.values.resize(values.size());
	//Evaluate Variable by Variable
	for (int i=0; i<values.size(); ++i)
	{
		Monomial m=values[i];
		Monomial res=Monomial();
		res.exponents=vector<unsigned>(m.exponents.size());
		for (int j=0; j<m.exponents.size(); ++j)
		{
			res.exponents[j]=j==var?0:m.exponents[j];
		}
		res.coeff=m.coeff*pow(val, m.exponents[var]);
		result.values[i]=res;
	}
	result.Clean();
	return result;
}

Polynomial Polynomial::Derivative(Polynomial poly, int var)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL4))return mathMod->Derivative(poly,var);
	Polynomial result=poly.Copy();
	for (int i=0; i<result.values.size(); ++i)
	{
		result.values[i].coeff*=result.values[i].exponents[var];
		--result.values[i].exponents[var];
	}
	result.Clean();
	return result;
}
Polynomial Polynomial::Integrate(Polynomial poly, int var)
{
	if(mathMod!=NULL&&mathMod->implemntsSubModule(XPINSMathModule::POLYL4))return mathMod->Integrate(poly,var);
	Polynomial result=poly.Copy();
	for (int i=0; i<result.values.size(); ++i)
	{
		++result.values[i].exponents[var];
		result.values[i].coeff/=result.values[i].exponents[var];
	}
	return result;
}

//Polynomial Cleanup
size_t Polynomial::Size()
{
	return values.size();
}
void Polynomial::Clean(){
	//Add matching monomials
	for (int i=0; i<values.size(); ++i)
	{
		if(values[i].coeff!=0)
			for (int j=i+1; j<values.size(); ++j)
			{
				if(values[j].coeff!=0)
				{
					bool match=true;
					int k=0;
					for (;k<values[i].exponents.size()&&k<values[j].exponents.size(); ++k)//Test for matching exponents
					{
						if(values[i].exponents[k]!=values[j].exponents[k])
						{
							match=false;
							break;
						}
					}
					if(match)for (;k<values[i].exponents.size()||k<values[j].exponents.size(); ++k)//Test zero exponents
					{
						if((values[i].exponents.size()<=k&&values[j].exponents[k]!=0)||
						   (values[j].exponents.size()<=k&&values[i].exponents[k]!=0))
						{
							match=false;
							break;
						}
					}
					//Conmbine terms (and make test coefficient zero )
					if(match)
					{
						values[i].coeff+=values[j].coeff;
						values[j].coeff=0;
					}
				}
			}
	}
	//Condense Polynomial
	int removed=0;
	for (int i=0; i<values.size()-removed; ++i)
	{
		Monomial m=values[i];
		if(m.coeff==0){//Remove Value;
			++removed;
			for (int j=i; j<values.size()-removed; ++j)
			{
				values[j]=values[j+1];
			}
			for (int j=int(values.size()-removed); j<values.size(); ++j)
			{
				values[j].coeff=0;
			}
			--i;
		}
	}
	values.resize(values.size()-removed);
}

#pragma mark Vector Field Library

// Vectors Creation
VectorField::VectorField ()
{
	P=Polynomial();
	Q=Polynomial();
	R=Polynomial();
}
VectorField::VectorField (Polynomial p,Polynomial q,Polynomial r)
{
	P=p.Copy();
	Q=q.Copy();
	R=r.Copy();
}
VectorField::VectorField(Vector v)
{
	P=Polynomial(v.x);
	Q=Polynomial(v.y);
	R=Polynomial(v.z);
}
VectorField VectorField::Copy()
{
	Polynomial p=P.Copy();
	Polynomial q=Q.Copy();
	Polynomial r=R.Copy();
	return VectorField(p,q,r);
}
void VectorField::Clear()
{
	P.Clear();
	P.Clear();
	R.Clear();
}
VectorField VectorField::GradientField(Polynomial poly)
{
	Polynomial p=Polynomial::Derivative(poly, 0);
	Polynomial q=Polynomial::Derivative(poly, 1);
	Polynomial r=Polynomial::Derivative(poly, 2);
	return VectorField(p,q,r);
}
// Vector Operations
VectorField VectorField::Add(VectorField a, VectorField b)
{
	Polynomial p=a.P+b.P;
	Polynomial q=a.Q+b.Q;
	Polynomial r=a.R+b.R;
	return VectorField(p,q,r);
}
VectorField VectorField::Scale(VectorField a, double b)
{
	Polynomial p=a.P*b;
	Polynomial q=a.Q*b;
	Polynomial r=a.R*b;
	return VectorField(p,q,r);
}
VectorField VectorField::PScale(VectorField a, Polynomial b)
{
	Polynomial p=a.P*b;
	Polynomial q=a.Q*b;
	Polynomial r=a.R*b;
	return VectorField(p,q,r);
}
Polynomial VectorField::Dot(VectorField a, VectorField b)
{
	return a.P*b.P+a.Q*b.Q+a.R*b.R;
}
VectorField VectorField::Cross(VectorField a, VectorField b)
{
	Polynomial p=a.Q*b.R-a.R*b.Q;
	Polynomial q=a.R*b.P-a.P*b.R;
	Polynomial r=a.P*b.Q-a.Q*b.P;
	return VectorField(p,q,r);
}
VectorField VectorField::MatMult(Matrix a, VectorField b)
{
	Polynomial p=b.P*a.values[0]+b.Q*a.values[1]+b.R*a.values[2];
	Polynomial q=b.P*a.values[3]+b.Q*a.values[4]+b.R*a.values[5];
	Polynomial r=b.P*a.values[6]+b.Q*a.values[7]+b.R*a.values[8];
	return VectorField(p,q,r);
}
// Composition and Evaluation
VectorField VectorField::Compose(VectorField a, Polynomial b, int var)
{
	Polynomial P=Polynomial::Composition(a.P, b, var);
	Polynomial Q=Polynomial::Composition(a.Q, b, var);
	Polynomial R=Polynomial::Composition(a.R, b, var);
	return VectorField(P,Q,R);
}
VectorField VectorField::PartialEvaluate(double val, int var)
{
	Polynomial p=P.PartialEvaluate(val, var);
	Polynomial q=Q.PartialEvaluate(val, var);
	Polynomial r=R.PartialEvaluate(val, var);
	return VectorField(p,q,r);
}
Vector VectorField::Evaluate(vector<double> vals)
{
	double x=P.Evaluate(vals);
	double y=Q.Evaluate(vals);
	double z=R.Evaluate(vals);
	return Vector(x,y,z, Vector::Cartesian);
}
// Calculus
VectorField VectorField::Derivative(VectorField field, int var)
{
	Polynomial P=Polynomial::Derivative(field.P, var);
	Polynomial Q=Polynomial::Derivative(field.Q, var);
	Polynomial R=Polynomial::Derivative(field.R, var);
	return VectorField(P,Q,R);
}
VectorField VectorField::Integrate(VectorField field, int var)
{
	Polynomial P=Polynomial::Integrate(field.P, var);
	Polynomial Q=Polynomial::Integrate(field.Q, var);
	Polynomial R=Polynomial::Integrate(field.R, var);
	return VectorField(P,Q,R);
}
//Vector Calculus
Polynomial VectorField::Divergence()
{
	Polynomial dPdx=Polynomial::Derivative(P, 0);
	Polynomial dQdy=Polynomial::Derivative(Q, 1);
	Polynomial dRdz=Polynomial::Derivative(R, 2);
	return dPdx+dQdy+dRdz;
}
VectorField VectorField::Curl()
{
	Polynomial x=Polynomial::Derivative(R, 1)-Polynomial::Derivative(Q, 2);
	Polynomial y=Polynomial::Derivative(P, 2)-Polynomial::Derivative(R, 0);
	Polynomial z=Polynomial::Derivative(Q, 0)-Polynomial::Derivative(P, 1);
	return VectorField(x,y,z);
}
//Line and Surface Integral Helpers (bounds and Normalization)
VectorField::bound::bound(Polynomial start, Polynomial stop)
{
	this->start=start;
	this->stop=stop;
}
Polynomial Magnitude(VectorField f)
{
	VectorField field=f.Copy();
	//Create Taylor Polynomial
	vector<Polynomial::Monomial> terms=vector<Polynomial::Monomial>(10);
	terms[0]=Polynomial::Monomial::monomial(0.18547058, 0, 0, 0, 0, 0, 0, 0);
	terms[1]=Polynomial::Monomial::monomial(1.66923523, 1, 0, 0, 0, 0, 0, 0);
	terms[2]=Polynomial::Monomial::monomial(-2.2256470, 2, 0, 0, 0, 0, 0, 0);
	terms[3]=Polynomial::Monomial::monomial(3.11590576, 3, 0, 0, 0, 0, 0, 0);
	terms[4]=Polynomial::Monomial::monomial(-3.3384705, 4, 0, 0, 0, 0, 0, 0);
	terms[5]=Polynomial::Monomial::monomial(2.59658813, 5, 0, 0, 0, 0, 0, 0);
	terms[6]=Polynomial::Monomial::monomial(-1.4163208, 6, 0, 0, 0, 0, 0, 0);
	terms[7]=Polynomial::Monomial::monomial(0.51361084, 7, 0, 0, 0, 0, 0, 0);
	terms[8]=Polynomial::Monomial::monomial(-0.1112824, 8, 0, 0, 0, 0, 0, 0);
	terms[9]=Polynomial::Monomial::monomial(0.01091003, 9, 0, 0, 0, 0, 0, 0);
	Polynomial sqrtTaylor=Polynomial(terms);
	//Extend Range
	int newRange=0x100;
	Polynomial sub=Polynomial(vector<Polynomial::Monomial>(1,Polynomial::Monomial::monomial(1.0/newRange,1,0,0,0,0,0,0)));
	sqrtTaylor=Polynomial::Composition(sqrtTaylor, sub, 0);
	sqrtTaylor*=sqrt(newRange);
	//Find Magnitude
	Polynomial xs=Polynomial::Power(field.P,2);
	Polynomial ys=Polynomial::Power(field.Q,2);
	Polynomial zs=Polynomial::Power(field.R,2);
	return Polynomial::Composition(sqrtTaylor, xs+ys+zs, 0);
}
//Line Integrals
Polynomial VectorField::LineIntegral(Polynomial f,VectorField curve,  bound bounds)
{
	VectorField dr=Derivative(curve, 4);
	Polynomial poly=f*Magnitude(dr);
	poly=poly.Composition(poly, curve.P, 0);
	poly=poly.Composition(poly, curve.Q, 1);
	poly=poly.Composition(poly, curve.R, 2);
	Polynomial integral=Polynomial::Integrate(poly, 4);
	return Polynomial::Composition(integral, bounds.stop, 4)-Polynomial::Composition(integral, bounds.start, 4);
}
Polynomial VectorField::LineIntegral(VectorField v,VectorField curve,bound bounds)
{
	VectorField dr=Derivative(curve, 4);
	Polynomial poly=Dot(v, dr);
	poly=poly.Composition(poly, curve.P, 0);
	poly=poly.Composition(poly, curve.Q, 1);
	poly=poly.Composition(poly, curve.R, 2);
	Polynomial integral=Polynomial::Integrate(poly, 4);
	return Polynomial::Composition(integral, bounds.stop, 4)-Polynomial::Composition(integral, bounds.start, 4);
}

Polynomial VectorField::SurfaceIntegral(Polynomial f,VectorField surface, bound uBound, bound vBound)
{
	//Find Normal Vector
	VectorField drdu=Derivative(surface, 4);
	VectorField drdv=Derivative(surface, 5);
	VectorField normal=Cross(drdu, drdv);
	//Get Scalar Field in terms of U and V
	Polynomial p=f.Copy();
	p=Polynomial::Composition(p, surface.P, 0);
	p=Polynomial::Composition(p, surface.Q, 1);
	p=Polynomial::Composition(p, surface.R, 2);
	Polynomial poly=p*Magnitude(normal);
	//Integrate
	Polynomial integral=Polynomial::Integrate(poly, 5);
	poly=Polynomial::Composition(integral, vBound.stop,5)-Polynomial::Composition(integral, vBound.start,5);
	integral=Polynomial::Integrate(poly, 4);
	poly=Polynomial::Composition(integral, uBound.stop,4)-Polynomial::Composition(integral, uBound.start,4);
	return poly;
}
Polynomial VectorField::SurfaceIntegral(VectorField v,VectorField surface, bound uBound, bound vBound)
{
	//Find Normal Vector
	VectorField drdu=Derivative(surface, 4);
	VectorField drdv=Derivative(surface, 5);
	VectorField normal=Cross(drdu, drdv);
	//Get Vector Field in terms of U and V
	VectorField field=v.Copy();
	field=Compose(field, surface.P, 0);
	field=Compose(field, surface.Q, 1);
	field=Compose(field, surface.R, 2);
	Polynomial poly=Dot(field, normal);
	//Integrate
	Polynomial integral=Polynomial::Integrate(poly, 5);
	poly=Polynomial::Composition(integral, vBound.stop,5)-Polynomial::Composition(integral, vBound.start,5);
	integral=Polynomial::Integrate(poly, 4);
	poly=Polynomial::Composition(integral, uBound.stop,4)-Polynomial::Composition(integral, uBound.start,4);
	return poly;
}
Polynomial VectorField::VolumeIntegral(Polynomial f, VectorField map, bound uBound,bound vBound,bound wBound)
{
	//Find Jacobian
	VectorField drdu=VectorField::Derivative(map, 4);
	VectorField drdv=VectorField::Derivative(map, 5);
	VectorField drdw=VectorField::Derivative(map, 6);
	Polynomial jacobian=Dot(drdu, Cross(drdv, drdw));
	//Get Scalar Field in terms of U and V
	Polynomial p=f.Copy();
	p=Polynomial::Composition(p, map.P, 0);
	p=Polynomial::Composition(p, map.Q, 1);
	p=Polynomial::Composition(p, map.R, 2);
	Polynomial poly=p*jacobian;
	//Integrate
	Polynomial integral=Polynomial::Integrate(poly, 6);
	poly=Polynomial::Composition(integral, wBound.stop,6)-Polynomial::Composition(integral, wBound.start,6);
	integral=Polynomial::Integrate(poly, 5);
	poly=Polynomial::Composition(integral, vBound.stop,5)-Polynomial::Composition(integral, vBound.start,5);
	integral=Polynomial::Integrate(poly, 4);
	poly=Polynomial::Composition(integral, uBound.stop,4)-Polynomial::Composition(integral, uBound.start,4);
	return poly;
}