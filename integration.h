#ifndef INTEGRATION_H
#define INTEGRATION_H

#include "function.h"
#include <string>
#include <map>
#include <vector>

class Integration
{
public:
	Integration(const Func& _f, const Interval& in) : f(_f), ab(in) { }
	double integrateWithSimpson() const;
	double integrateWithTrapezoid() const;
	double integrateWithRectangle() const;
	double errorSimpson() const;
	double errorTrapezoid() const;
	double errorRectangle() const;
	double stepSimpson(double eps) const;
	double stepTrapezoid(double eps) const;
	double stepRectangle(double eps) const;
	std::pair<double, std::string> gauss(unsigned int n);
	static std::map<unsigned int, std::vector<double> > legandre;
	static void init();
        Func getFunc() const { return f; }
        Interval getInterval() const { return ab; }
        bool isEmpty() const
        {
            return (f.isEmpty() && ab.isEmpty());
        }
private:
	Func f;
	Interval ab;
	void getGaussPoints(unsigned int n, std::vector<double>& p) const;
	void makeMatrix(const std::vector<double>& p,
								  std::vector< std::vector<double> >& m) const;
	void calcCoeffs(std::vector< std::vector<double> >& gm,
							 std::vector<double>& c);
	void makeGaussMatrix(std::vector< std::vector<double> >& gm);
	std::pair<double, std::string> calcGauss(const std::vector<double>& points,
		const std::vector<double>& c);
};

#endif
