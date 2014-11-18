#include "interpolation.h"
#include "cmp.h"
#include "functionparser.h"
#include "derivation.h"
#include "divideddiffs.h"
#include <algorithm>
#include <cmath>
#include <QPointF>

//hjdfhjdhf

Interpolation::Interpolation(const std::vector<QPointF>& _data) : data(_data)
{
	std::sort(data.begin(), data.end(), PointCmp());
	ab = Interval(data.front().x(), data.back().x());
	makeDividedDiffs();
}

Interpolation::Interpolation(const Func &_f, const Interval &_ab) : f(_f), ab(_ab) { }

Interpolation::Interpolation(const Func& _f, const Interval& _ab, const std::vector<double>& x)
	: f(_f), ab(_ab)
{
	for (std::vector<double>::const_iterator i = x.begin(); i != x.end(); ++i) {
		data.push_back(QPointF(*i, f(*i)));
	}
	std::sort(data.begin(), data.end(), PointCmp());
	makeDividedDiffs();
}

void Interpolation::makeLagrange(std::vector<std::string> &up,
											std::vector<std::string> &down)
{
	if (!lagrangeUp.empty() && !lagrangeDown.empty()) {
		up = lagrangeUp; down = lagrangeDown;
		return;
	}

	lagrangeUp.clear();
	lagrangeDown.clear();

	for (CVI k = data.begin(); k != data.end(); ++k) {
		std::string u = FunctionParser::strFromDouble(k->y());
		u += "*";
		std::string d = "";
		for (CVI j = data.begin(); j != data.end(); ++j) {
			if (j != k) {
				u += "(x";
				u += (j->x() < 0 ? "+" : "-");
				u += FunctionParser::strFromDouble(fabs(j->x()));
				u += ")";
				u += "*";
				d += "(";
				d += FunctionParser::strFromDouble(k->x());
				d += (j->x() < 0 ? "+" : "-");
				d += FunctionParser::strFromDouble(fabs(j->x()));
				d += ")";
				d += "*";
			}
		}
		u.erase(u.end() - 1);
		d.erase(d.end() - 1);
		lagrangeUp.push_back(u);
		lagrangeDown.push_back(d);
	}

	up = lagrangeUp;
	down = lagrangeDown;
}

void Interpolation::makeLagrangeGrData(QVector<QPointF> &grdata) const
{
	grdata.clear();
	if (data.empty()) {
		return;
	}
	
	double first = data.front().x();
	double last = data.back().x();
	double dx = (last - first) / 300;
	if (dx == 0) {
		grdata.push_back(QPointF(first, calcLagrange(first)));
		return;
	}

	while (first <= last - dx + 0.00001) {
		grdata.push_back(QPointF(first, calcLagrange(first)));
		first += dx;
	}
	grdata.push_back(QPointF(last, calcLagrange(last)));
}

double Interpolation::calcLagrange(double x) const
{
	double val = 0;
	for (CVI k = data.begin(); k != data.end(); ++k) {
		double mult = 1;
		double div = 1;
		for (CVI j = data.begin(); j != data.end(); ++j) {
			if (j != k) {
				mult *= (x - j->x());
				div *= (k->x() - j->x());
			}
		}
		val += k->y() * (mult / div);
	}

	return val;
}

double Interpolation::calcLagrangeError() const
{
	if (data.empty()) {
		return 0;
	}
	Derivator* derivator = new Derivator(f);
	unsigned int n = data.size();
	Func g = derivator->derive(n);
	double m = g.maxModulo(Interval(data.front().x(), data.back().x()));
	delete derivator;
	unsigned int fac = factorial(n);
	double max = 0;
	double dx = (ab.end() - ab.begin()) / 300;

	for (double x = ab.begin(); x <= ab.end() + 0.00001; x += dx) {
		double mult = 1;
		for (CVI i = data.begin(); i != data.end(); ++i) {
			mult *= x - i->x();
		}
		if (fabs(mult) > max) {
			max = fabs(mult);
		}
	}
	
	return m * max / fac;
}

void Interpolation::makeNewton(std::string& nton)
{
	if (!newton.empty()) {
		nton = newton;
		return;
	}

	typedef std::vector< std::vector<double> >::const_iterator CDVI;
	int k = 0;
	std::string mult = "";
	for (CDVI i = diffs.begin(); i != diffs.end(); ++i) {
		if (k != 0) {
			mult += "*(x";
			mult += (data[k - 1].x() > 0 ? "-" : "+");
			mult += FunctionParser::strFromDouble(data[k - 1].x());
			mult += ")";
		}
		
		newton += (i->front() > 0 ? "+" : "-") ;
		newton += FunctionParser::strFromDouble(fabs(i->front()));
		newton += mult;

		++k;
	}
	nton = newton;
}

void Interpolation::makeNewtonGrData(QVector<QPointF>& grdata) const
{
	grdata.clear();
	if (data.empty()) {
		return;
	}
	
	double first = data.front().x();
	double last = data.back().x();
	double dx = (last - first) / 300;

	if (dx == 0) {
		grdata.push_back(QPointF(first, calcNewton(first)));
		return;
	}

	while (first <= last - dx) {
		grdata.push_back(QPointF(first, calcNewton(first)));
		first += dx;
	}

	grdata.push_back(QPointF(last, calcNewton(last)));
}

double Interpolation::calcNewton(double x) const
{
	double val = 0;
	typedef std::vector< std::vector<double> >::const_iterator CDVI;
	int k = 0;
	for (CDVI i = diffs.begin(); i != diffs.end(); ++i) {
		double mult = 1;
		for (int j = 0; j < k; ++j) {
			mult = mult * (x - data[j].x());
		}
		val += i->front() * mult;
		++k;
	}

	return val;
}

double Interpolation::linearConstStep(double eps) const
{
	Derivator* der = new Derivator(f);
	Func g = der->derive(2);
	double max = g.maxModulo(ab);
	
	return sqrt((8 * eps) / max);
}

void Interpolation::linearVarStep(double eps, std::vector<double> &x) const
{
	double xk = ab.begin();
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(2);
	
	do {
		double max = g.maxModulo(Interval(xk, ab.end()));
		x.push_back(xk);
		double xk1 = xk + sqrt(8 * eps / max);
		xk = xk1;
	} while (xk < ab.end());

	x.push_back(ab.end());
}

double Interpolation::quadrateConstStep(double eps) const
{
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(3);
	double max = g.maxModulo(ab);

	return pow((9*sqrt(3.)*eps) / max, 1.0/3);
}

void Interpolation::quadrateVarStep(double eps, std::vector<double>& x) const
{
	double xk = ab.begin();
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(3);
	
	do {
		double max = g.maxModulo(Interval(xk, ab.end()));
		x.push_back(xk);
		double xk2 = 2*(pow((9*eps*sqrt(3.0)) / max, 1.0/3)) + xk;
		if (xk2 < ab.end()) {
			x.push_back((xk2 + xk) / 2);
		}
		else {
			x.push_back((ab.end() + xk) / 2);
			x.push_back(ab.end());
		}
		xk = xk2;
	} while (xk < ab.end());
}

double Interpolation::cubeConstStep(double eps) const
{
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(4);
	double max = g.maxModulo(ab);
	
	return pow(24*eps / max, 1.0/4);
}

void Interpolation::cubeVarStep(double eps, std::vector<double> &x) const
{
	double xk = ab.begin();
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(4);
	
	do {
		double max = g.maxModulo(Interval(xk, ab.end()));
		x.push_back(xk);
		double xk3 = 3*(pow(eps*24 / max, 0.25)) + xk;
		if (xk3 < ab.end()) {
			double h = (xk3 - xk) / 3;
			x.push_back(xk + h);
			x.push_back(xk + 2*h);
		}
		else {
			double h = (ab.end() - xk) / 3;
			x.push_back(xk + h);
			x.push_back(xk + 2*h);
			x.push_back(ab.end());
		}
		xk = xk3;
	} while (xk < ab.end());
}

void Interpolation::constN(int n, QVector<QPointF> &grdata) const
{
	grdata.clear();
	if (n == 0) {
		return;
	}
	
	double first = ab.begin();
	double last = ab.end();
	double h = (last - first) / n;
	std::vector<QPointF> d;

	while (first <= last - h + 0.00001) {
		d.push_back(QPointF(first, f(first)));
		first += h;
	}

	d.push_back(QPointF(last, f(last)));

	Interpolation in(d);
	in.makeLagrangeGrData(grdata);
}

void Interpolation::tchebishevN(int n, QVector<QPointF> &grdata) const
{
	grdata.clear();
	if (n == 0) {
		return;
	}

	std::vector<double> tchebishev;
	const double pi = 4.0*atan(1.0);
	for (int k = 0; k <= n; ++k) {
		tchebishev.push_back(cos((2*pi*k + pi) / (2*(n + 1))));
	}

	std::vector<double> args;
	if (ab.begin() == -1 && ab.end() == 1) {
		args = tchebishev;
	}
	else {
		double minus = (ab.end() - ab.begin()) / 2;
		double plus = (ab.end() + ab.begin()) / 2;
		for (std::vector<double>::const_iterator i = tchebishev.begin(); i != tchebishev.end(); ++i) {
			args.push_back(*i*minus + plus);
		}
	}

	Interpolation in(f, ab, args);
	in.makeLagrangeGrData(grdata);
}

void Interpolation::makeFunctionGrData(QVector<QPointF> &grdata) const
{
	grdata.clear();
	double first = ab.begin();
	double last = ab.end();
	double dx = (last - first) / 300;

	while (first <= last - dx + 0.00001) {
		grdata.push_back(QPointF(first, f(first)));
		first += dx;
	}

	grdata.push_back(QPointF(last, f(last)));
}

void Interpolation::getDividedDiffs(std::vector<std::vector<double> > &d)
{
	d = diffs;
}

void Interpolation::makeDividedDiffs()
{
	DividedDiffs* diff = new DividedDiffs(data);
	diff->calcDividedDiffs();
	diff->getDividedDiffs(diffs);
	
	delete diff;
}

unsigned int factorial(unsigned int n)
{
	if (n == 1 || n == 0) {
		return 1;
	}

	return n * factorial(n - 1);
}

