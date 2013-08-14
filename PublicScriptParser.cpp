//
//  PublicScriptParser.cpp
//
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include "PublicScriptParser.h"
#include <math.h>
/*include your own headers*/


using namespace std;

/*Custom Scriptable Functions*/



//Helper functions

//read index of a variable
int readVarIndex(char* scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	while(scriptText[i]!=expectedEnd){
		index*=10;
		if(scriptText[i]=='1')index+=1;
		else if(scriptText[i]=='2')index+=2;
		else if(scriptText[i]=='3')index+=3;
		else if(scriptText[i]=='4')index+=4;
		else if(scriptText[i]=='5')index+=5;
		else if(scriptText[i]=='6')index+=6;
		else if(scriptText[i]=='7')index+=7;
		else if(scriptText[i]=='8')index+=8;
		else if(scriptText[i]=='9')index+=9;
		else if(scriptText[i]!='0')index/=10;
		i++;
	}
	*startIndex=i;
	return index;
}
//Read an int constant
int readInt(char* scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	bool isNeg=scriptText[i]=='-';
	while(scriptText[i]!=expectedEnd){
		index*=10;
		if(scriptText[i]=='1')index+=1;
		else if(scriptText[i]=='2')index+=2;
		else if(scriptText[i]=='3')index+=3;
		else if(scriptText[i]=='4')index+=4;
		else if(scriptText[i]=='5')index+=5;
		else if(scriptText[i]=='6')index+=6;
		else if(scriptText[i]=='7')index+=7;
		else if(scriptText[i]=='8')index+=8;
		else if(scriptText[i]=='9')index+=9;
		else if(scriptText[i]!='0')index/=10;
		i++;
	}
	if(isNeg)index*=-1;
	*startIndex=i;
	return index;
}
//read a float constant
float readFloat(char* scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	int fpartDig=0;
	bool fpart=false;
	bool isNeg=scriptText[i]=='-';
	while(scriptText[i]!=expectedEnd){
		if(fpart)fpartDig++;
		index*=10;
		if(scriptText[i]=='1')index+=1;
		else if(scriptText[i]=='2')index+=2;
		else if(scriptText[i]=='3')index+=3;
		else if(scriptText[i]=='4')index+=4;
		else if(scriptText[i]=='5')index+=5;
		else if(scriptText[i]=='6')index+=6;
		else if(scriptText[i]=='7')index+=7;
		else if(scriptText[i]=='8')index+=8;
		else if(scriptText[i]=='9')index+=9;
		else if(scriptText[i]=='.')fpart=true;
		else if(scriptText[i]!='0')index/=10;
		i++;
	}
	while (fpartDig) {
		index/=10;
		fpartDig--;
	}
	if(isNeg)index*=-1;
	*startIndex=i;
	return index;
}

//Primary Function
void ScriptParser::parseScript(char* scriptText,varSpace *vars,params *parameters,bool isRECURSIVE,int start,int stop){
	/*Declare Your Custom Functions*/
	
	
	//SET UP VAR SPACE
	bool initialized_varSpace=false;
	if(vars==NULL){
		vars=new varSpace;
		vars->bVars=vector<bool>();
		vars->iVars=vector<int>();
		vars->fVars=vector<float>();
		vars->vVars=vector<Math::Vector*>();
		vars->pVars=vector<void*>();
		initialized_varSpace=true;
	}
	//RUN SCRIPT
	int i=0;//index of char in script
	if(isRECURSIVE)i=start;
	//Validate start of script
	if(!isRECURSIVE){
		while(scriptText[i]!='@')i++;
		if(scriptText[i+1]!='C'||
		   scriptText[i+2]!='O'||
		   scriptText[i+3]!='D'||
		   scriptText[i+4]!='E'){
			printf("\nERROR:INVALID SCRIPT:MISSING @CODE!\n");
			return;
		}
	}
	while(true){
		//get to next line
		while(scriptText[i]!='\n')i++;
		i++;
		//reaching the end of the Script
		if((scriptText[i]!='@'&&
			scriptText[i+1]!='E'&&
			scriptText[i+2]!='N'&&
			scriptText[i+3]!='D')||
		   (isRECURSIVE&&i==stop)){
			break;
		}
		//Declaring new vars
		if(scriptText[i]=='B'){
			vars->bVars.resize(vars->bVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='I'){
			vars->iVars.resize(vars->iVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='F'){
			vars->fVars.resize(vars->fVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='V'){
			vars->vVars.resize(vars->vVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='*'){
			vars->pVars.resize(vars->pVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		if(scriptText[i]=='$'){
			i++;
			if(scriptText[i]=='B'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$'&&scriptText[i]!='^')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='B') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPE DOESN'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->bVars[index]=vars->bVars[index2];
				}
				else if(scriptText[i]=='^'){
					if(scriptText[i+1]=='T')
						vars->bVars[index]=true;
					else if(scriptText[i+1]=='F')
						vars->bVars[index]=false;
					else{
						printf("\nERROR:INVALID SCRIPT:INVALID BOOL CONSTANT!\n");
						return;
					}
				}
				else if(scriptText[i]!='#'){
					if (scriptText[i+1]!='F'&&(scriptText[i+1]!='X'||scriptText[i+2]!='_')) {
						printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
						return;
					}
					i+=2;
					/*Bridge Your Custom BOOL Functions*/
					
					
					//BUILT IN FUNCTIONS
					//X_AND
					else if(scriptText[i+1]=='A'&&scriptText[i+2]=='N'&&scriptText[i+3]=='D'){
						i+=4;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='B') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='B') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=vars->bVars[index2]&&vars->bVars[index3];
					}
					//X_OR
					else if(scriptText[i+1]=='O'&&scriptText[i+2]=='R'){
						i+=3;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='B') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='B') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=vars->bVars[index2]||vars->bVars[index3];
					}
					//X_NOT
					else if(scriptText[i+1]=='N'&&scriptText[i+2]=='O'&&scriptText[i+3]=='T'){
						i+=4;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='B') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=!vars->bVars[index2];
					}
					//X_ILESS
					else if(scriptText[i+1]=='I'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
						i+=6;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=vars->iVars[index2]<vars->iVars[index3];
					}
					//X_FLESS
					else if(scriptText[i+1]=='F'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
						i+=6;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=vars->fVars[index2]<vars->fVars[index3];
					}
					//X_IMORE
					else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
						i+=6;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=vars->iVars[index2]>vars->iVars[index3];
					}
					//X_FMORE
					else if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
						i+=6;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=vars->fVars[index2]>vars->fVars[index3];
					}
					//X_IEQUAL
					else if(scriptText[i+1]=='I'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
						i+=7;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=vars->iVars[index2]==vars->iVars[index3];
					}
					//X_FEQUAL
					else if(scriptText[i+1]=='F'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
						i+=7;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:ICORRECT PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->bVars[index]=vars->fVars[index2]==vars->fVars[index3];
					}
					else{
						printf("\nERROR:INVALID SCRIPT:UNDEFINED FUNCTION!\n");
						return;
					}
				}
			}
			else if(scriptText[i]=='I'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$'&&scriptText[i]!='^')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='I') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPES DON'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->iVars[index]=vars->iVars[index2];
				}
				else if(scriptText[i]=='^'){
					i++;
					int n=readInt(scriptText,&i,'\n');
					vars->iVars[index]=n;
				}
				else if(scriptText[i]!='#'){
					if (scriptText[i+1]!='F'&&(scriptText[i+1]!='X'||scriptText[i+2]!='_')) {
						printf("\nERROR:INVALID SCRIPT:UNDECLARED FUNCTION!\n");
						return;
					}
					i+=2;
					/*Bridge Your Custom INT Functions*/
					
					
					//BUILT IN FUNCTIONS
					//X_IADD
					else if(scriptText[i+1]=='I'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->iVars[index]=vars->iVars[index2]+vars->iVars[index3];
					}
					//X_ISUB
					else if(scriptText[i+1]=='I'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->iVars[index]=vars->iVars[index2]-vars->iVars[index3];
					}
					//X_IMULT
					else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
						i+=6;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->iVars[index]=vars->iVars[index2]*vars->iVars[index3];
					}
					//X_IDIV
					else if(scriptText[i+1]=='I'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->iVars[index]=vars->iVars[index2]/vars->iVars[index3];
					}
					//X_RAND
					else if(scriptText[i+1]=='P'&&scriptText[i+2]=='O'&&scriptText[i+3]=='W'){
						i+=4;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='I') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ',');
						vars->iVars[index]=arc4random()%(vars->iVars[index3]-vars->iVars[index2])+vars->fVars[index2];
					}
					else{
						printf("\nERROR:INVALID SCRIPT:NONEXISTENT FUNCTION!\n");
						return;
					}
				}
			}
			else if(scriptText[i]=='F'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$'&&scriptText[i]!='^')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='F') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPES DON'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->fVars[index]=vars->fVars[index2];
				}
				else if(scriptText[i]=='^'){
					i++;
					float n=readFloat(scriptText,&i,'\n');
					vars->fVars[index]=n;
				}
				else if(scriptText[i]!='#'){
					if (scriptText[i+1]!='F'&&(scriptText[i+1]!='X'||scriptText[i+2]!='_')) {
						printf("\nERROR:INVALID SCRIPT:NONEXISTENT FUNCTION!\n");
						return;
					}
					i+=2;
					/*Bridge Your Custom FLOAT Functions*/

					
					//BUILT IN FUNCTIONS
					//X_FADD
					else if(scriptText[i+1]=='F'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=vars->fVars[index2]+vars->fVars[index3];
					}
					//X_FSUB
					else if(scriptText[i+1]=='F'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=vars->fVars[index2]-vars->fVars[index3];
					}
					//X_FMULT
					else if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
						i+=6;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=vars->fVars[index2]*vars->fVars[index3];
					}
					//X_FDIV
					else if(scriptText[i+1]=='F'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=vars->fVars[index2]/vars->fVars[index3];
					}
					//X_VMAG
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='M'&&scriptText[i+3]=='A'&&scriptText[i+4]=='G'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=vars->vVars[index2]->magnitude();
					}
					//X_VDIR
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='R'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=vars->vVars[index2]->direction();
					}
					//X_VX
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='X'){
						i+=3;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ')');
						float f=0;
						vars->vVars[index2]->RectCoords(&f, NULL);
						vars->fVars[index]=f;
					}
					//X_VY
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='Y'){
						i+=3;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ')');
						float f=0;
						vars->vVars[index2]->RectCoords(NULL, &f);
						vars->fVars[index]=f;
					}
					//X_VANG
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+4]=='G'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=Math::Vector::angleBetweenVectors(vars->vVars[index2], vars->vVars[index3]);
					}
					//X_VADDPOLAR
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'&&scriptText[i+5]=='P'&&scriptText[i+6]=='O'&&scriptText[i+7]=='L'&&scriptText[i+8]=='A'&&scriptText[i+9]=='R'){
						i+=10;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=Math::addPolar(vars->fVars[index2], vars->fVars[index3]);
					}
					//X_VDIST
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='S'&&scriptText[i+5]=='T'){
						i+=6;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->fVars[index]=Math::dist(vars->fVars[index2], vars->fVars[index3]);
					}
					//X_TSIN
					else if(scriptText[i+1]=='T'&&scriptText[i+2]=='S'&&scriptText[i+3]=='I'&&scriptText[i+4]=='N'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						vars->fVars[index]=sinf(vars->fVars[index2]);
					}
					//X_TCOS
					else if(scriptText[i+1]=='T'&&scriptText[i+2]=='C'&&scriptText[i+3]=='O'&&scriptText[i+4]=='S'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						vars->fVars[index]=cosf(vars->fVars[index2]);
					}
					//X_TTAN
					else if(scriptText[i+1]=='T'&&scriptText[i+2]=='T'&&scriptText[i+3]=='A'&&scriptText[i+4]=='N'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						vars->fVars[index]=tanf(vars->fVars[index2]);
					}
					//X_TATAN
					else if(scriptText[i+1]=='T'&&scriptText[i+2]=='A'&&scriptText[i+3]=='T'&&scriptText[i+4]=='A'&&scriptText[i+5]=='N'){
						i+=6;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ',');
						vars->fVars[index]=atan2f(index2, index3);
					}
					//X_POW
					else if(scriptText[i+1]=='P'&&scriptText[i+2]=='O'&&scriptText[i+3]=='W'){
						i+=4;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ',');
						vars->fVars[index]=powf(index2, index3);
					}
					else{
						printf("\nERROR:INVALID SCRIPT!\n");
						return;
					}
				}
			}
			else if(scriptText[i]=='V'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='V') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPES DON'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->vVars[index]=vars->vVars[index2]->copy();
				}
				else if(scriptText[i]!='#'){
					if (scriptText[i+1]!='F'&&(scriptText[i+1]!='X'||scriptText[i+2]!='_')) {
						printf("\nERROR:INVALID SCRIPT:NONEXISTENT FUNCTION!\n");
						return;
					}
					i+=2;
					/*Bridge Your Custom Vector Functions*/
					
					
					//BUILT IN FUNCTIONS
					//X_VREC
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->vVars[index]=new Math::Vector(vars->fVars[index2],vars->fVars[index3]);
					}
					//X_VPOL
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->vVars[index]=Math::Vector::PolarVector(vars->fVars[index2], vars->fVars[index3]);
					}
					//X_VADD
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->vVars[index]=Math::Vector::addVectors(vars->vVars[index2], vars->vVars[index3]);
					}
					//X_VSUB
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
						i+=5;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						Math::Vector *temp=Math::Vector::scaledVector(vars->vVars[index3], -1);
						vars->vVars[index]=Math::Vector::addVectors(vars->vVars[index2], temp);
						delete temp;
					}
					//X_VSCALE
					else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='C'&&scriptText[i+4]=='A'&&scriptText[i+5]=='L'&&scriptText[i+6]=='E'){
						i+=7;
						if (scriptText[i]!='('||scriptText[i+1]!='$'||scriptText[i+2]!='V') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index2=readVarIndex(scriptText, &i, ',');
						if (scriptText[i]!=','||scriptText[i+1]!='$'||scriptText[i+2]!='F') {
							printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
							return;
						}
						i+=3;
						int index3=readVarIndex(scriptText, &i, ')');
						vars->vVars[index]=Math::Vector::scaledVector(vars->vVars[index2], vars->fVars[index3]);
					}
					else{
						printf("\nERROR:INVALID SCRIPT:UNDECLARED FUNCTION!\n");
						return;
					}
				}
			}
			else if(scriptText[i]=='P'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$'&&scriptText[i]!='^')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='P') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPES DON'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->pVars[index]=vars->pVars[index2];
				}
				else if(scriptText[i]!='#'){
					if (scriptText[i+1]!='F') {
						printf("\nERROR:INVALID SCRIPT:UNDECLARED FUNCTION!\n");
						return;
					}
					i+=2;
					/*Bridge Your Custom Type Functions*/

					
					else{
						printf("\nWARNING:UNDECLARED PROCEDURE!\n");
					}
				}
			}
		}
		else if(scriptText[i]=='#'){
			if (scriptText[i+1]!='F') {
				printf("\nWARNING:UNDECLARED PROCEDURE!\n");
			}
			/*Bridge Your Custom Procedures*/
			
			
			else{
				printf("\nWARNING:UNDECLARED PROCEDURE!\n");
			}
		}
		else if(scriptText[i]=='@'){
			i++;
			//IF STATEMENT
			if(scriptText[i]=='I'&&scriptText[i+1]=='F'){
				while (scriptText[i]!='$') i++;
				if(scriptText[i+1]!='B'){
					printf("WARNING:VARIABLE NOT BOOL, SKIPPING IF BLOCK");
				}
				else{
					i+=2;
					int index=readVarIndex(scriptText, &i, ']');
					if(!vars->bVars[index]){
						while (scriptText[i]!='{')i++;
						int num=1;
						//skip if block
						while(num>0){
							if(scriptText[i]=='{')num++;
							else if(scriptText[i]=='}')num--;
							i++;
						}
						while (scriptText[i]!='\n')i++;
						//execute else if applicable
						if(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='L'&&scriptText[i+3]=='S'&&scriptText[i+4]=='E'){
							while (scriptText[i]!='{')i++;
							i++;
						}
					}
					else{
						while (scriptText[i]!='{')i++;
						i++;
					}
				}
			}
			//WHILE LOOP
			else if(scriptText[i]=='W'&&scriptText[i+1]=='H'&&scriptText[i+2]=='I'&&scriptText[i+3]=='L'&&scriptText[i+4]=='E'){
				while (scriptText[i]!='$') i++;
				if(scriptText[i+1]!='B'){
					printf("WARNING:VARIABLE NOT BOOL, SKIPPING WHILE LOOP");
				}
				else{
					i+=2;
					int index=readVarIndex(scriptText, &i, ']');
					while (scriptText[i]!='{')i++;
					int loopStart=i;
					int num=1;
					//skip if block
					while(num>0){
						if(scriptText[i]=='{')num++;
						else if(scriptText[i]=='}')num--;
						i++;
					}
					int loopStop=i-1;
					while (vars->bVars[index]) {
						ScriptParser::parseScript(scriptText, vars, parameters, true, loopStart, loopStop);
					}
				}
			}
			//ELSE (BYPASS because IF was executed)
			else if(scriptText[i]=='E'&&scriptText[i+1]=='L'&&scriptText[i+2]=='S'&&scriptText[i+3]=='E'){
				while (scriptText[i]!='{')i++;
				int num=1;
				//skip if block
				while(num>0){
					if(scriptText[i]=='{')num++;
					else if(scriptText[i]=='}')num--;
					i++;
				}
				
			}
		}
	}
	if(initialized_varSpace) delete vars;
}











