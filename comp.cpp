#include <cstdio>
#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
/*void split1(const string& str, Container& cont)
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
              istream_iterator<string>(),
              back_push_backer(cont));
}*/
void BAD()
{
	
}
using namespace std;
vector<string> infixToPostfix(vector<string> infix)
{ // + * , pow( ( )
	vector<string> postfix;
	stack<string> st;
	for(int i=0;i<(int)infix.size();i++)
	{
		if(infix[i] == ",")
		{
			while(st.top()!="pow(")
			{
				postfix.push_back(st.top()+"");
				st.pop();
				if(st.empty) BAD();
			}
		}
		if(infix[i] == "*")
		{
			if(st.empty() || st.top()=="+" || st.top()=="*" || st.top()=="(" || st.top()=="pow(")
				st.push("*");
		}
		if(infix[i] == "+")
		{
			// empty = BAD
			while(st.top()=="*")
			{
				postfix.push_back("*");
				st.pop();
			}
			if(st.empty() || st.top()=="+" || st.top()=="(" || st.top()=="pow(")
				st.push("+");
		}
		if(infix[i] == "(" || infix[i] == "pow(")
		{
			st.push(infix[i]);
		}
		if(infix[i] == ")")
		{
			//BAD
			while(st.top()!="(" && st.top()!="pow(")
			{
				postfix.push_back(st.top()+"");
				st.pop();
			}
			if(st.top()=="pow("){
				st.push("^");
			}
		}
	}

	return postfix;
	
}


vector<string> tokenizeExpression(string expr)
{
	vector<string> tokens;
	string temp = "";
	for(int i=0;i<(int)expr.length;i++)
	{
		if(expr[i] == '+' || expr[i] == '*' || expr[i] == '(' || expr[i] == ')' || expr[i] == ',')
		{
			if(temp == "pow" && expr[i] == '(')
			{
				tokens.push_back(temp+"(");
				temp = "";
			}
			else if(temp.length() > 0)
			{
				tokens.push_back(temp);
				temp = "";
			}
			else
				tokens.push_back(expr[i]+"");
		}
		if(('a' <= expr[i] && 'z' >= expr[i]) || ('A' <= expr[i] && 'Z' >= expr[i]) || ('0' <= expr[i] && '9' >= expr[i]))
		{
			temp = temp + expr[i];
		}
	}
}
void printMov(ofstream& g,string dest,string source){

	g<<"mov "+dest+","+source;


}
void printAdd(ofstream& g,string dest,string source){
	g<<"add "+dest+","+source;

}
int main(int argc,char* argv[]){
	set<string> varset;
 	string file=(string(argv[1]));
	string newfilename=(file.substr(0,file.find_last_of('.'))+".asm");
	ofstream g(newfilename);
    ifstream f(argv[1]);
    string line;
    while (getline(f, line)){
		string found;
		unsigned long tem=line.find("=");
		if(tem==string::npos)
		{


			//TODO Tell assembly to print values
		}

		else
		{
			//Assuming single space around '='

			vector<string> postfix=infixToPostfix(tokenizeExpression(line.substr(tem+2)));
			stack<string> myst;
			for(int i=0;i<postfix.size();i++){

				if(postfix[i]=="+"){

					continue;
				}
				// operations

				myst.push(postfix[i]);
				if(!isdigit(postfix[i][0])){

					varset.insert(postfix[i]);

				}


			}

		}
		/*vector<char> words;
        split1(line,words);*/
		
        // cout << line << std::endl;
    }
}