#include "function.h"
#include "functionparser.h"
#include <stack>
#include <cmath>
#include <iostream>

std::map<std::string, double(*)(double)> Func::funcNames;
std::vector<double(*)(double)> Func::funcs;

Func::Func(const std::string ff) : f(ff)
{
	initializeFuncs();
	initializeFuncNames();
	FunctionParser* parser = new FunctionParser(f);
	parser->parse(parsed, arg);
	delete parser;
}

Func::Func(const std::list<std::string>& p, const std::string& a) : f(""), parsed(p), arg(a)
{
	initializeFuncs();
	initializeFuncNames();
}

double Func::evalExpression(double x) const
{
	typedef std::list<std::string>::const_iterator CLI;
	double val = 0;
	std::stack<double> s;
	if (parsed.front()[0] != '#') {
		for (CLI i = parsed.begin(); i != parsed.end(); ++i) {
			if (*i == arg) {
				s.push(x);
			}
			else {
				if (FunctionParser::isNumber(*i)) {
					s.push(FunctionParser::doubleFromStr(*i));
				}
				else if(FunctionParser::isFunction(*i)) {
					if (!s.empty()) {
						double xx = s.top();
						s.pop();
						s.push(funcNames[*i](xx));
					}
					else {
						return 0;
					}
				}
				else if (FunctionParser::isOperator(*i)) {
					if (!s.empty()) {
						double x1 = s.top();
						double x2 = 0;
						s.pop();
						if (!s.empty()) {
							x2 = s.top();
							s.pop();
						}
						else {
							return 0;
						}
						s.push(evalOperator(*i, x2, x1));
					}
					else {
						return 0;
					}
				}
			}
		}
		val = s.top();
		s.pop();
	}

	if (fabs(val) <= 0.0000001) {
		val = 0;
	}

	return val;
}

void Func::initializeFuncs()
{
	funcs.push_back(&sqrt);
	funcs.push_back(&sin);
	funcs.push_back(&cos);
	funcs.push_back(&tan);
	funcs.push_back(&atan);
	funcs.push_back(&asin);
	funcs.push_back(&acos);
	funcs.push_back(&log10);
	funcs.push_back(&log);
	funcs.push_back(&exp);
}

void Func::initializeFuncNames()
{
	funcNames["SQRT"] = funcs[0];
	funcNames["SIN"] = funcs[1];
	funcNames["COS"] = funcs[2];
	funcNames["TAN"] = funcs[3];
	funcNames["ARCTAN"] = funcs[4];
	funcNames["ARCSIN"] = funcs[5];
	funcNames["ARCCOS"] = funcs[6];
	funcNames["LOG10"] = funcs[7];
	funcNames["LOG"] = funcs[8];
	funcNames["EXP"] = funcs[9];
}

double Func::evalOperator(const std::string& op, double x1, double x2) const
{
	if (op == "+") {
		return x1 + x2;
	}
	else if (op == "-") {
		return x1 - x2;
	}
	else if (op == "*") {
		return x1*x2;
	}
	else if (op == "/") {
		if (x2 != 0) {
			return x1 / x2;
		}
		else {
			return 0;
		}
	}
	else if (op == "^") {
		return pow(x1, x2);
	}
	else {
		return 0;
	}
}

double Func::minModulo(const Interval& in) const
{
	double a = in.begin();
	double b = in.end();
	double dx = (b - a) / 300;
	double m = fabs(evalExpression(a));
	a += dx;

	while (a <= b) {
		double val = fabs(evalExpression(a));
		if (val < m) {
			m = val;
		}
		a += dx;
	}

	return m;
}

double Func::maxModulo(const Interval& in) const
{
	double a = in.begin();
	double b = in.end();
	double dx = (b - a) / 300;
	double m = fabs(evalExpression(a));
	a += dx;

	while (a <= b) {
		double val = fabs(evalExpression(a));
		if (val > m) {
			m = val;
		}
		a += dx;
	}

	return m;
}

std::ostream& operator <<(std::ostream& out, const Func& f)
{
	out << f.func();

	return out;
}
