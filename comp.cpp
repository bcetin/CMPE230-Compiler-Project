#include <cstdio>
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

vector<string> infixToPostfix(vector<string>& infix)
{ // + * , pow( ( )
	vector<string> postfix;
	stack<string> st;
	for(int i=0;i<(int)infix.length;i++)
	{
		if(infix[i] == ",")
		{
			while(st.top()!="pow(")
			{
				postfix.push_back(st.top());
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
			while(st.top("("))
			{
				postfix.push_back(st.top());
				st.pop();
			}
		}
	}
	
	
}


vector<string> tokenizeExpression(string& expr)
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
			else if(temp.length > 0)
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
using namespace std;
int main(int argc,char* argv){
    istringstream f(argv[1]);
    string line;
    while (getline(f, line)){
		string found=line.find("=");
		if(found!=string::npos)
		{
			//TODO Tell assembly to print values
		}
		else
		{
			//Assuming single space around '='
			tokenizeExpression(line.substr(found+2));
		}
		/*vector<char> words;
        split1(line,words);*/
		
        // cout << line << std::endl;
    }
}