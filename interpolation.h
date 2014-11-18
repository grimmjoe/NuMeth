#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <vector>
#include <string>
#include "function.h"
#include <QPointF>
#include <QVector>

class Interpolation
{
public:
	typedef std::vector<QPointF>::iterator VI;
	typedef std::vector<QPointF>::const_iterator CVI;
	Interpolation(const std::vector<QPointF>& d);
	Interpolation(const Func& ff, const Interval& abab, const std::vector<double>& x);
	Interpolation(const Func& ff, const Interval& abab);
	Interpolation(const Interpolation& in)
		: data(in.data), f(in.f), ab(in.ab), diffs(in.diffs), lagrangeUp(in.lagrangeUp),
		lagrangeDown(in.lagrangeDown), newton(in.newton) { }
	void makeLagrange(std::vector<std::string>& up, std::vector<std::string>& down);
        void makeLagrangeGrData(QVector<QPointF>& grdata) const;
	double calcLagrange(double x) const;
	double calcLagrangeError() const;
	void makeNewton(std::string& newton);
        void makeNewtonGrData(QVector<QPointF>& grdata) const;
	double calcNewton(double x) const;
	double linearConstStep(double eps) const;
	void linearVarStep(double eps, std::vector<double>& x) const;
	double quadrateConstStep(double eps) const;
	void quadrateVarStep(double eps, std::vector<double>& x) const;
	double cubeConstStep(double eps) const;
	void cubeVarStep(double eps, std::vector<double>& x) const;
	void constN(int n, QVector<QPointF>& grdata) const;
	void tchebishevN(int n, QVector<QPointF>& grdata) const;
	void makeFunctionGrData(QVector<QPointF>& grdata) const;
	void makeDividedDiffs();
	void getDividedDiffs(std::vector< std::vector<double> >& d);
	Func getFunc() const { return f; }
    void getData(std::vector<QPointF>& d) const { d = data; }
    Interval getInterval() const { return ab; }
private:
	std::vector<QPointF> data;
	Func f;
	Interval ab;
	std::vector< std::vector<double> > diffs;
	std::vector<std::string> lagrangeUp;
	std::vector<std::string> lagrangeDown;
	std::string newton;
};

unsigned int factorial(unsigned int n);

#endif


