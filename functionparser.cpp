#include "functionparser.h"
#include <stack>
#include <algorithm>
#include <cctype>
#include <cmath>

std::map<std::string, int> FunctionParser::prec;
std::list<std::string> FunctionParser::funcNames;

void FunctionParser::initializePrecs()
{
	prec["+"] = 1;
	prec["-"] = 1;
	prec["*"] = 2;
	prec["/"] = 2;
	prec["^"] = 3;
}

void FunctionParser::initializeFuncNames()
{
	funcNames.push_back("SQRT");
	funcNames.push_back("SIN");
	funcNames.push_back("COS");
	funcNames.push_back("TAN");
	funcNames.push_back("ARCTAN");
	funcNames.push_back("ARCSIN");
	funcNames.push_back("ARCCOS");
	funcNames.push_back("LOG10");
	funcNames.push_back("LN");
	funcNames.push_back("EXP");
}

double FunctionParser::doubleFromStr(const std::string& s)
{
	if (s.empty()) {
		return 0;
	}
	
	return atof(s.c_str());
}

bool FunctionParser::isNumber(const std::string &s)
{
	if (!s.empty()){
		if (isdigit(s[0]) || (s[0] == '.')) {
			return true;
		}
		else if (s[0] == '-') {
			if (s.size() == 1) {
				return false;
			}
			else if (isdigit(s[1]) || (s[1] == '.')) {
				return true;
			}
		}
	}

	return false;
}

void FunctionParser::parse(std::list<std::string> &parsed, std::string& arg)
{
	parsed.clear();
	std::stack<std::string> s;
	getRidOfSpaces();
	SI i = f.begin();
	bool sign = 0;
	bool first = true;
	if (i != f.end() && *i == '-') {
		sign = 1;
		++i;
	}
	for (; i != f.end();) {
		if (isdigit(*i)) {
			std::string n = getNumber(i);
			if (n == "####") {
				parsed.clear();
				parsed.push_back("The number must contain only one .!");
				return;
			}
			if (sign) {
				sign = 0;
				n.insert(n.begin(), '-');
			}
			parsed.push_back(n);
		}
		else if (isOperator(*i)) {
			std::string op;
			op.insert(op.begin(), *i);
			if (!s.empty()) {
				while (!s.empty() && precendence(s.top(), op) >= 0 && s.top() != "(") {
					parsed.push_back(s.top());
					s.pop();
				}
			}
			s.push(op);
			++i;
		}
		else if (*i == '(') {
			std::string bracket = "(";
			s.push(bracket);
			++i;
		}
		else if (*i == ')') {
			while (!s.empty() && s.top() != "(") {
				parsed.push_back(s.top());
				s.pop();
			}
			if (s.empty()) {
				parsed.clear();
				parsed.push_back("#There's an ) without (!");;
				return;
			}
			s.pop();
			if (!s.empty() && isFunction(s.top())) {
				parsed.push_back(s.top());
				s.pop();
			}
			++i;
		}
		else if (isalpha(*i)) {
			std::string name = getName(i);
			if (!isFunction(name)) {
				parsed.push_back(name);
				if (first) {
					arg = name;
					first = false;
				}
				else {
					if (name != arg) {
						parsed.clear();
						parsed.push_back("#The function can take only one argument!");
					}
				}
			}
			else {
				s.push(name);
			}
		}
		else {
			parsed.clear();
			parsed.push_back("#Unknown token!");
			return;
		}
	}

	while (!s.empty()) {
		parsed.push_back(s.top());
		s.pop();
	}
}

void FunctionParser::getRidOfSpaces()
{
	for (SI i = f.begin(); i != f.end();) {
		if (isspace(*i)) {
			f.erase(i);
			continue;
		}
		++i;
	}
}

std::string FunctionParser::getNumber(SI& pos) const
{
	std::string n;
	for (; pos != f.end() && (isdigit(*pos) || *pos == '.'); ++pos) {
		n += *pos;
	}
	int c = std::count(n.begin(), n.end(), '.');
	if (c > 1) {
		return "####";
	}
	return n;
}

bool FunctionParser::isOperator(const std::string& s)
{
	if (s == "+" || s == "-" || s == "*" || s == "/" || s == "^") {
		return true;
	}

	return false;
}



bool FunctionParser::isOperator(char c)
{
	std::string s;
	s.insert(s.begin(), c);
	return isOperator(s);
}

int FunctionParser::precendence(const std::string &op1, const std::string &op2) const
{
	return (prec[op1] - prec[op2]);
}

bool FunctionParser::isFunction(const std::string &s)
{
	std::list<std::string>::iterator p = std::find(funcNames.begin(), funcNames.end(), s);
	if (p == funcNames.end()) {
		return false;
	}

	return true;
}

std::string FunctionParser::strFromDouble(double d)
{
	if (d == 0) {
		return "0";
	}
	std::string str;
	if (d < 0) {
		str += "-";
		d = fabs(d);
	}
	double eps = 4;
	int k = 0;
	while (d >= 1) {
		d /= 10;
		k++;
	}
	if (k < 1) {
		str += "0";
	}
	while(k >= 1) {
		d *= 10;
		str += static_cast<int>(d) % 10 + '0';
		--k;
	}
	str += '.';
	do {
		d *= 10;
		str += static_cast<int>(d) % 10 + '0';
		--eps;
	} while (eps >= 1);
	for(std::string::reverse_iterator i = str.rbegin(); i != str.rend()
												&& *i == '0' && *i != '.';) {
		std::string::reverse_iterator p = i;
		++i;
		str.erase(p.base());
	}

	return str;
}

std::string FunctionParser::getName(FunctionParser::SI &pos) const
{
	std::string name;
	for (; pos != f.end() && isalnum(*pos);) {
		name += toupper(*pos);
		++pos;
	}

	return name;
}
