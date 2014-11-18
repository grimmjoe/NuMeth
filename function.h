#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <map>
#include <vector>
#include <list>

class Interval;

class Func
{
public:
	friend class Derivator;
	typedef double (*function)(double);
	Func() : f()
	{
		initializeFuncs();
		initializeFuncNames();
	}
	Func(const std::string ff);
	Func(const Func& func) : f(func.f), parsed(func.parsed), arg(func.arg) { }
	double operator()(double x) const {
		return evalExpression(x);
	}
	std::string func() const { return f; }
	double minModulo(const Interval& in) const;
	double maxModulo(const Interval& in) const;
	static std::map<std::string, function> funcNames;
	static std::vector<function> funcs;
        bool isEmpty() const
        {
            if (f.empty() && parsed.empty()) {
                return true;
            }

            return false;
        }
	static void initializeFuncs();
	static void initializeFuncNames();
private:
	std::string f;
	std::list<std::string> parsed;
	std::string arg;
	double evalExpression(double) const;
	double evalOperator(const std::string& op, double x1, double x2) const;
	friend std::ostream& operator <<(std::ostream& out, const Func& f);
	Func(const std::list<std::string>& p, const std::string& arg);
};

class Interval
{
	double first;
	double last;
public:
	Interval() : first(0), last(0) { }
	Interval(double f, double l) : first(f), last(l) { }
	double begin() const { return first; }
	double end() const { return last; }
        bool isEmpty() const { return (first == 0 && last == 0); }
};

#endif
