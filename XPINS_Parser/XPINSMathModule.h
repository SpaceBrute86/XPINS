//
//  XPINSMathModule.h
//  XPINS
//
//  Created by Robbie Markwick on 5/18/14.
//
//

#ifndef __XPINS__XPINSMathModule__
#define __XPINS__XPINSMathModule__

#include <iostream>
#include "XPINSScriptableMath.h"
using namespace XPINSScriptableMath;

// Math Library override
class XPINSMathModule
{
public:
	enum subModule
	{
		RANDOMVAR,EXPERIMENT,		//Basic Probability
		MARKOVL1,MARKOVL2,MARKOVL3,	//Markov Chain Probability
		VECL1,VECL2,				//Vectors
		MATL1,MATL2,MATL3,			//Matrices
		POLYL1,POLYL2,POLYL3,POLYL4		//Polynomials
	};
	virtual bool implemntsSubModule(subModule mod)									{return false;}
	// RANDOMVAR
	virtual double NormalRV(double mu, double sigma)								{return 0;}
	virtual	double ExponentialRV(double lambda)										{return 0;}
	virtual int PoissonRV(double lambda)											{return 0;}
	virtual bool BernoulliRV(double p)												{return false;}
	virtual double UniformRV(double low,double length)								{return 0;}
	// EXPERIMENT
	virtual int CoinFlip(double p,int n)											{return 0;}
	virtual int FairDiceRoll(int s,int n)											{return 0;}
	// MARKOVL1
	virtual double SimulateMarkovChain(Matrix chain, int n, int start)				{return 0;}
	virtual double TransitionProbability(Matrix chain, int n, int start, int stop)	{return 0;}
	// MARKOVL2
	virtual double SteadyStateProbability(Matrix chain, int state)					{return 0;}
	virtual bool Reachable(Matrix chain, int b, int a)								{return false;}
	// MARKOVL3
	virtual double AbsorbtionProbability(Matrix chain, int state,int start)			{return 0;}
	virtual double AbsorbtionTime(Matrix chain, int state,int start)				{return 0;}
	virtual int SimulateAbsorbtionTime(Matrix chain, int state,int start)			{return 0;}
	// VECL1
	virtual Vector Add(Vector,Vector)												{return Vector();}
	virtual Vector Scale(Vector, double)											{return Vector();}
	virtual double DotProduct(Vector,Vector)										{return 0;}
	virtual Vector CrossProduct(Vector,Vector)										{return Vector();}
	// VECL2
	virtual double AngleBetweenVectors(Vector,Vector)								{return 0;}
	virtual Vector ProjectionOntoVector(Vector,Vector)								{return Vector();}
	virtual Vector UnitVectorFromVector(Vector)										{return Vector();}
	// MATL1
	virtual Matrix Append(Matrix A,Matrix B)										{return Matrix();}
	virtual Matrix Transpose(Matrix matrix)											{return Matrix();}
	virtual Matrix Add(Matrix A,Matrix B)											{return Matrix();}
	virtual Matrix Scale(Matrix matrix,double scalar)								{return Matrix();}
	// MATL2
	virtual Matrix Multiply(Matrix A,Matrix B)										{return Matrix();}
	virtual Matrix Power(Matrix A,int p)											{return Matrix();}
	// MATL3
	virtual double Determinant(Matrix matrix)										{return 0;}
	virtual Matrix RowEchelon(Matrix matrix)										{return Matrix();}
	virtual Matrix ReducedRowEchelon(Matrix matrix)									{return Matrix();}
	virtual Matrix Invert(Matrix matrix)											{return Matrix();}
	// POLYL1
	virtual Polynomial Add(Polynomial a,Polynomial b)								{return Polynomial();}
	virtual Polynomial Scale(Polynomial,double)										{return Polynomial();}
	// POLYL2
	virtual Polynomial Multiply(Polynomial,Polynomial)								{return Polynomial();}
	virtual Polynomial Divide(Polynomial,Polynomial)								{return Polynomial();}
	virtual Polynomial Power(Polynomial a,unsigned int b)							{return Polynomial();}
	// POLYL3
	virtual Polynomial Composition(Polynomial,Polynomial,int var)					{return Polynomial();}
	virtual double Evaluate(Polynomial p,vector<double> vars)						{return 0;}
	virtual Polynomial PartialEvaluate(Polynomial p,double val,int var)				{return Polynomial();}
	//POLY L4
	virtual Polynomial Derivative(Polynomial p, int var)							{return Polynomial();}
	virtual Polynomial Integrate(Polynomial p, int var)								{return Polynomial();}
};


#endif /* defined(__XPINS__XPINSMathModule__) */
