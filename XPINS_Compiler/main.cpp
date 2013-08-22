//
//  main.cpp
//  XPINS_Compiler
//
//  Created by Robbie Markwick on 8/21/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
using namespace std;
string removeComments(string);
string renameFunctions(string);

string strRepresentationOfInt(int);
bool stringsMatch(int,string,string);


int main(int argc, const char * argv[])
{
	string fileName;
	cout<<"Please enter the full path for the script you would like to compile"<<endl;
	cin>>fileName;
	ifstream inFile;
	inFile.open(fileName);
	if (inFile.fail()) {
		cerr << "unable to open file for reading" << endl;
		exit(1);
	}
	string s,scriptText="";
	/*
	while (inFile >> s) {
		scriptText += s;
		scriptText +="\n";
	}*/
	char ch;
	while (inFile.get(ch)) {
		scriptText+=ch;
	}
	cout<<"About To Compile Script:\n"<<scriptText;
	scriptText=removeComments(scriptText);
	cout<<"\n\n\n\nComments and ';'s Removed:\n"<<scriptText;
	scriptText=renameFunctions(scriptText);
	cout<<"\n\n\n\Functions Renamed:\n"<<scriptText;
    return 0;
}
//COMPILE STEPS
string removeComments(string input){
	string output="";
	char ch;
	for(int i=0;i<input.length();i++){
		ch=input[i];
		if(ch=='/'){// all '/'s will be removed, any meaningful ones denote a comment
			i++;
			ch=input[i];
			if(ch=='*'){//multi line comments
				while (input[i]!='*'||input[i+1]!='/')i++;
				i++;
			}
			else if(ch=='/'){// single line comments
				while (input[i+1]!='\n'||i+1==input.length())i++;
			}
		}
		else if(ch!=';') output+=ch;//No semicolons :(
	}
	return output;
}

string renameFunctions(string input){
	string output="";
	char ch;
	int i=0;
	while(input[i]!='@'){
		i++;
	}
	if(input[i+1]!='F'||input[i+2]!='U'||input[i+3]!='N'||input[i+4]!='C'){
		cerr<<"Invalid Script: Missing @FUNC.\nEXITING";
		exit(1);
	}
	i+=5;
	string intermediate1="";
	string intermediate2="";
	//Initialize first intermediate
	int j=0;
	while (input[j]!='@'||input[j+1]=='C') {
		j++;
	}
	for(;j<input.length();j++){
		ch=input[j];
		intermediate1+=ch;
	}
	//Do the actual work
	int x=1;
	while(input[i]!='@'||input[i+1]!='E'||input[i+2]!='N'||input[i+3]!='D'){
		if(i==input.length()){
			cerr<<"Invalid Script: Missing @END.\nEXITING";
			exit(1);
		}
		while (input[i]!=' ') i++;
		i++;
		//read function name
		string functionName;
		for(j=i;j<input.length()&&input[j]!='(';j++){
			functionName+=input[j];
		}
		//Replace function name
		intermediate2="";
		while(j<intermediate1.length()){
			while(intermediate1[j]!='#'){//Find start of function
				intermediate2+=intermediate1[j];
				j++;
				
			}
			intermediate2+=intermediate1[j];
			j++;
			if(stringsMatch(j, intermediate1, functionName)){//Found Match
				intermediate2+='F';
				intermediate2+=strRepresentationOfInt(x);
				while(intermediate1[j]!='('){//Find '('
					j++;
				}
			}
		}
		//Get ready for next loop
		intermediate1=intermediate2;
		x++;
		while (input[i]!='\n')i++;
	}

	return output;
}

//UTILTY FUNCTIONS
string strRepresentationOfInt(int x){
	if(x/10==0){
		switch (x) {
			case 1:return "1";
			case 2:return "2";
			case 3:return "3";
			case 4:return "4";
			case 5:return "5";
			case 6:return "6";
			case 7:return "7";
			case 8:return "8";
			case 9:return "9";
			case 0:return "0";
		}
	}
	else return strRepresentationOfInt(x/10)+strRepresentationOfInt(x%10);
	return "";
}
bool stringsMatch(int start,string first, string sec){
	for(int i=start;i<start+sec.length();i++){
		if(i==first.length()) return false;
		if(first[i]!=sec[i-start])return false;
	}
	return true;
}






















