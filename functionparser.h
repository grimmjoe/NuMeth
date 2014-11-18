#ifndef FUNCTIONPARSER_H
#define FUNCTIONPARSER_H

#include "function.h"
#include <list>
#include <string>
#include <map>

class FunctionParser
{
public:
	typedef std::string::const_iterator CSI;
	typedef std::string::iterator SI;
	FunctionParser(const std::string& ff) : f(ff)
	{
		initializePrecs();
		initializeFuncNames();
	}
	void parse(std::list<std::string>& parsed, std::string& arg);
	static double doubleFromStr(const std::string& s);
	static bool isNumber(const std::string& s);
	static bool isFunction(const std::string& s);
	static bool isOperator(char c);
	static bool isOperator(const std::string& s);
	static std::string strFromDouble(double d);
private:
	std::string f;
	void getRidOfSpaces();
	std::string getNumber(SI& pos) const;
	int precendence(const std::string& op1, const std::string& op2) const;
	std::string getName(SI& pos) const;
	static std::map<std::string, int> prec;
	static std::list<std::string> funcNames;
	static void initializePrecs();
	static void initializeFuncNames();
	std::string toUpper(const std::string& s) const;
};

#endif
