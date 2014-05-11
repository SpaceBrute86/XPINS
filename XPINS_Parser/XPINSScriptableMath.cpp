//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINSScriptableMath.h"
#include <math.h>
using namespace XPINSScriptableMath;


#pragma mark Probability Library

double XPINSScriptableMath::Probability::NormalRV(double mu, double sigma)
{
	double CDF=((double)arc4random())/(0x100000000);//Random between 0 and 1
	double devs=(CDF<0.5?-1:1)*sqrt(2*log(0.25/(CDF-pow(CDF, 2))));
	return mu+sigma*devs;
}

bool XPINSScriptableMath::Probability::BernoulliRV(double p)
{
	double CDF=((double)arc4random())/(0x100000000);//Random between 0 and 1
	return CDF<p;
}
double XPINSScriptableMath::Probability::UniformRV(double low,double length)
{
	double P=((double)arc4random())/(0x100000000);//Random between 0 and 1
	return P*length+low;
}

double XPINSScriptableMath::Probability::ExponentialRV(double lambda)
{
	double CDF=((double)arc4random())/(0x100000000);//Random between 0 and 1
	double t=-log((1.0-CDF));
	return t/lambda;
}
int XPINSScriptableMath::Probability::PoissonRV(double lambda)
{
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
	int sum=0;
	for (int i=0; i<n; ++i)
	{
		if (BernoulliRV(p))++sum;
	}
	return sum;
}
int  XPINSScriptableMath::Probability::FairDiceRoll(int sides,int n)
{
	int sum=0;
	for (int i=0; i<n; ++i)
	{
		sum+=(int)UniformRV(1, sides);
	}
	return sum;
}
double XPINSScriptableMath::Probability::SimulateMarkovChain(Matrix chain, int steps, int start)
{
	Matrix markov=chain.copy();
	int state=start;
	for (int i=0; i<steps; ++i)
	{
		double P=((double)arc4random())/(0x100000000);
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
	Matrix markov=chain.copy();
	Matrix prob=Matrix::IdentityMatrixOfSize(markov.GetRows());
	for (int i=0; i<steps; ++i)
	{
		prob*=markov;
	}
	return prob.ValueAtPosition(stop, start);
}
double XPINSScriptableMath::Probability::SteadyStateProbability(Matrix chain, int state)
{
	Matrix markov=chain.copy();
	Matrix constants=Matrix(markov.GetRows(), 1);
	Matrix system=Matrix::Append(Matrix::IdentityMatrixOfSize(markov.GetRows())-markov, constants);
	for (int i=0; i<system.GetCols(); ++i)
	{
		system.SetValueAtPosition(1, 0, i);
	}
	Matrix solution=Matrix::ReducedRowEchelon(system);
	return solution.ValueAtPosition(state, solution.GetCols()-1);
}
double XPINSScriptableMath::Probability::AbosorbtionProbability(Matrix chain, int state,int start)
{
	Matrix markov=Subchain(chain, state, start,false);
	return SteadyStateProbability(markov, state);
}
double XPINSScriptableMath::Probability::AbosorbtionTime(Matrix chain, int state,int start)
{
	Matrix markov=Subchain(chain, state, start,true);
	Matrix constants=Matrix(markov.GetRows(), 1,1.0);
	Matrix system=Matrix::Append(Matrix::IdentityMatrixOfSize(markov.GetRows())-markov, constants);
	Matrix solution=Matrix::ReducedRowEchelon(system);
	return solution.ValueAtPosition(state, solution.GetCols()-1);
}
int XPINSScriptableMath::Probability::SimulateAbosorbtionTime(Matrix chain, int state,int start)
{
	Matrix markov=Subchain(chain, state, start,true);
	int time=0;
	for (int current=start; current!=state; ++time)
	{
		double P=((double)arc4random())/(0x100000000);
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
	Matrix markov=chain.copy();
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
	Matrix markov=chain.copy();
	Matrix tempMarkov=markov.copy();
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
XPINSScriptableMath::Vector::Vector(double x,double y,double z)
{
	this->x=x;
	this->y=y;
	this->z=z;
}
Vector Vector::PolarVector(double dist, double dir,double z)
{
	double x=dist*cosf(dir);
	double y=dist*sinf(dir);
	return Vector(x,y,z);
}
Vector Vector::SphericalVector(double dist, double dir,double alt)
{
	double x=dist*cosf(dir)*sinf(alt);
	double y=dist*sinf(dir)*sinf(alt);
	double z=dist*cosf(alt);
	return Vector(x,y,z);
}
//Vector Components Values
double Vector::Magnitude()
{
	return sqrtf(powf(x, 2)+powf(y, 2)+powf(z,2));
}
void Vector::SphericalCoords(double *rho, double *theta,double *phi)
{
	if(rho)*rho=sqrtf(powf(x, 2)+powf(y, 2)+powf(z,2));
	if(theta)*theta=atan2f(y, x);
	if(phi)*phi=atan2f(sqrtf(x*x+y*y), z);
}
void Vector::PolarCoords(double *r, double *theta,double *z)
{
	if(r)*r=sqrtf(powf(x, 2)+powf(y, 2));
	if(theta)*theta=atan2f(y, x);
	if(z)*z=this->z;
}
void Vector::RectCoords(double *x, double *y,double *z)
{
	if(x)*x=this->x;
	if(y)*y=this->y;
	if(z)*z=this->z;
}

//Vector Arithmetic
Vector Vector::Add(Vector vec1, Vector vec2)
{
	double x=vec1.x+vec2.x;
	double y=vec1.y+vec2.y;
	double z=vec1.z+vec2.z;
	return Vector(x,y,z);
}
Vector Vector::Scale(Vector vec, double scale)
{
	return Vector(vec.x*scale,vec.y*scale,vec.z*scale);
}
double Vector::DotProduct(Vector a,Vector b)
{
	double x=a.x*b.x;
	double y=a.y*b.y;
	double z=a.z*b.z;
	return x+y+z;
}
Vector Vector::CrossProduct(Vector a,Vector b)
{
	double x=(a.y*b.z) - (a.z*b.y);
	double y=(a.z*b.x) - (a.x*b.z);
	double z=(a.x*b.y) - (a.y*b.x);
	return Vector(x,y,z);
}
//Miscillaneous Vector Functions
double Vector::AngleBetweenVectors(Vector vec1, Vector vec2)
{
	double dot=DotProduct(vec1,vec2);
	double cos=dot/(vec1.Magnitude()*vec2.Magnitude());
	return acosf(cos);
}
Vector Vector::ProjectionInDirection(Vector vec,double dir,double alt)
{
	Vector unit=Vector::SphericalVector(1, dir,alt);
	double dot=DotProduct(vec, unit);
	Vector result=Scale(unit, dot);
	return result;
}
Vector Vector::UnitVectorFromVector(Vector vec)
{
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
//	delete values;
}
Matrix Matrix::copy()
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
	unitVec.RectCoords(&x, &y, &z);
	//Compute values
	//Source for Formulas: http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations
	matrix.values[0]=cosf(angle)+x*x*(1-cosf(angle));
	matrix.values[1]=z*sinf(angle)+x*y*(1-cosf(angle));
	matrix.values[2]=-y*sinf(angle)+x*z*(1-cosf(angle));
	matrix.values[3]=-z*sinf(angle)+x*y*(1-cosf(angle));
	matrix.values[4]=cosf(angle)+y*y*(1-cosf(angle));
	matrix.values[5]=x*sinf(angle)+z*y*(1-cosf(angle));
	matrix.values[6]=y*sinf(angle)+x*z*(1-cosf(angle));
	matrix.values[7]=-x*sinf(angle)+z*y*(1-cosf(angle));
	matrix.values[8]=cosf(angle)+z*z*(1-cosf(angle));
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
	if(m.rows==0)return Vector(0,0,0);
	return Vector(m.values[0],m.rows<1?0:m.values[1],m.rows<2?0:m.values[2]);
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
	Matrix a=ma.copy(),b=mb.copy();
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
	return m.copy();
}

Matrix Matrix::Add(Matrix ma,Matrix mb)
{
	Matrix a=ma.copy(),b=mb.copy();
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
	Matrix a=ma.copy();
	for (size_t i=0; i<a.rows*a.cols; ++i)
	{
		a.values[i]*=b;
	}
	return a;
}
Matrix Matrix::Multiply(Matrix ma,Matrix mb)
{
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
Vector Matrix::MVMultiply(Matrix m,Vector v)
{
	Matrix vectorMatrix=MatrixForVector(v);
	Matrix resultVectorMatrix=Multiply(m, vectorMatrix);
	return VectorForMatrix(resultVectorMatrix);
}
Matrix Matrix::Transpose(Matrix i)
{
	Matrix in=i.copy();
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
	Matrix ma=a.copy();
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
	Matrix ma=a.copy();
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
	Matrix ma=a.copy();
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
	Matrix ma=a.copy();
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

//Polynomial creation and copying
Polynomial::Polynomial ()
{
	values=*new vector<Monomial>(1);
}
Polynomial::Polynomial (vector<Monomial> v)
{
	values=v;
}
Polynomial::Monomial Polynomial::Monomial::monomial(double coeff,double xp, double yp, double zp, double tp)
{
	Monomial m=Monomial();
	m.coeff=coeff;
	m.exponents.resize(4);
	m.exponents[0]=xp;
	m.exponents[1]=yp;
	m.exponents[2]=zp;
	m.exponents[3]=tp;
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
	if(s==0)  return Polynomial();
	Polynomial result=p1.Copy();
	for (int i=0; i<result.values.size(); ++i)
	{
		result.values[i].coeff*=s;
	}
	return result;
}
Polynomial Polynomial::Multiply(Polynomial a1, Polynomial a2){
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
	if(b==0){
		Polynomial p=Polynomial();
		p.values.resize(1);
		p.values[0].coeff=1;
		p.values[0].exponents.resize(0);
		return p;
	}
	Polynomial p1=a.Copy();
	Polynomial result=p1.Copy();
	for (int i=1; i<b; ++i) {
		result*=p1;
	}
	return result;
}
Polynomial Polynomial::Composition(Polynomial f,Polynomial g,int var)
{
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
	Polynomial result=poly.Copy();
	for (int i=0; i<result.values.size(); ++i)
	{
		++result.values[i].exponents[var];
		result.values[i].coeff/=result.values[i].exponents[var];
	}
	return result;
}

//Polynomial Cleanup
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