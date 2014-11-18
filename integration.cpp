#include <cmath>
#include <algorithm>
#include <QString>
#include <iostream>
#include <fstream>
#include "integration.h"
#include "derivation.h"
#include "functionparser.h"

std::map<unsigned int, std::vector<double> > Integration::legandre;

void Integration::init()
{
	legandre[1].push_back(0);
	legandre[2].push_back(-1/sqrt(3.0));
	legandre[2].push_back(1/sqrt(3.0));
	legandre[3].push_back(-sqrt(3.0/5));
	legandre[3].push_back(0);
	legandre[3].push_back(sqrt(3.0/5));
	legandre[4].push_back(-sqrt((15+2*sqrt(30.0)) / 35));
	legandre[4].push_back(-sqrt((15-2*sqrt(30.0)) / 35));
	legandre[4].push_back(sqrt((15-2*sqrt(30.0)) / 35));
	legandre[4].push_back(sqrt((15+2*sqrt(30.0)) / 35));
}

double Integration::integrateWithSimpson() const
{
	double I = 0;
	double dx = (ab.end() - ab.begin()) / 4;
	int k = 2;
	double begin = ab.begin();
	double end = begin + 2*dx;
	while (k > 0) {
		I += ((end - begin) / 6)*(f(begin) + 4*f((begin + end) / 2) + f(end));
		begin = end;
		end += 2*dx;
		if ((ab.end() - begin) <= 0.00001) {
			end = ab.end();
		}
		--k;
	}

	if (I - 0 <= 0.0000001) {
		I = 0;
	}

	return I;
}

double Integration::integrateWithTrapezoid() const
{
	double I = 0;
	double dx = (ab.end() - ab.begin()) / 4;
	double begin = ab.begin();
	double end = begin + dx;
	while ((ab.end() - end) >= 0.00001) {
		I += (end - begin)*(f(end) + f(begin)) / 2;
		begin = end;
		end += dx;
	}
	I += (ab.end() - begin)*(f(ab.end()) + f(begin)) / 2;

	if (I - 0 <= 0.0000001) {
		I = 0;
	}

	return I;
}

double Integration::integrateWithRectangle() const
{
	double I = 0;
	double dx = (ab.end() - ab.begin()) / 4;
	double begin = ab.begin();
	double end = begin + dx;
	
	while ((ab.end() - end) >= 0.00001) {
		I += (end - begin)*f(begin);
		begin = end;
		end += dx;
	}
	I += (ab.end() - begin)*f(begin);

	if (I - 0 <= 0.0000001) {
		I = 0;
	}

	return I;
}

double Integration::errorSimpson() const
{
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(4);
	double max = g.maxModulo(ab);
	double h = (ab.end() - ab.begin()) / 4;
	delete derivator;

	return (max * pow(h, 4) * (ab.end() - ab.begin())) / 180;
}

double Integration::errorTrapezoid() const
{
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(2);
	double max = g.maxModulo(ab);
	double h = (ab.end() - ab.begin()) / 4;
	delete derivator;

	return (max * h * h * (ab.end() - ab.begin())) / 12;
}

double Integration::errorRectangle() const
{
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive();
	double max = g.maxModulo(ab);
	double h = (ab.end() - ab.begin()) / 4;
	delete derivator;

	return (max * h * (ab.end() - ab.begin())) / 2;
}

double Integration::stepSimpson(double eps) const
{
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(4);
	double max = g.maxModulo(ab);
	delete derivator;

	return pow(180*eps / (max*(ab.end() - ab.begin())), 0.25);
}

double Integration::stepTrapezoid(double eps) const
{
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive(2);
	double max = g.maxModulo(ab);
	delete derivator;

	return sqrt(12*eps / (max*(ab.end() - ab.begin())));
}

double Integration::stepRectangle(double eps) const
{
	Derivator* derivator = new Derivator(f);
	Func g = derivator->derive();
	double max = g.maxModulo(ab);
	delete derivator;

	return (2*eps / (max*(ab.end() - ab.begin())));
}

std::pair<double, std::string> Integration::gauss(unsigned int n)
{
	std::vector<double> points;
	getGaussPoints(n, points);
	std::vector< std::vector<double> > gaussMatrix;
	makeMatrix(points, gaussMatrix);
	std::vector<double> c;
	calcCoeffs(gaussMatrix, c);
	std::pair<double, std::string> g = calcGauss(points, c);
	
	return g;
}

void Integration::getGaussPoints(unsigned int n, std::vector<double>& p) const
{
	p.clear();
	p = legandre[n];
	double minus = (ab.end() - ab.begin()) / 2;
	double plus = (ab.end() + ab.begin()) / 2;
	for (std::vector<double>::iterator i = p.begin(); i != p.end(); ++i)
	{
		*i = minus * (*i) + plus;
	}
}

void Integration::makeMatrix(const std::vector<double>& p,
								  std::vector< std::vector<double> >& m) const
{
	m.clear();
	int n = p.size();
	double mid = (ab.end() + ab.begin()) / 2;
	for (int i = 0; i < n; ++i) {
		std::vector<double> temp;
		for (std::vector<double>::const_iterator j = p.begin(); j != p.end(); ++j) {
			temp.push_back(pow((*j - mid), i));
		}
		double integral = pow((ab.end() - mid), i + 1) / (i + 1)
			- pow((ab.begin() - mid), i + 1) / (i + 1);
		temp.push_back(integral);
		m.push_back(temp);
	}
}

void Integration::calcCoeffs(std::vector< std::vector<double> >& gm,
							 std::vector<double>& c)
{
	makeGaussMatrix(gm);
	unsigned int n = gm.size();
	unsigned int m = (n == 0 ? 0 : gm[0].size());
	for (int i = n - 1; i >= 0; --i) {
		double sum = 0;
		int k = c.size() - 1;
		for (unsigned int j = i + 1; j < m - 1; ++j) {
			sum += gm[i][j] * c[k];
			--k;
		}
		if (gm[i][i] == 0) {
			c.clear();
			return;
		}
		c.push_back((gm[i].back() - sum) / gm[i][i]);
	}
	std::reverse(c.begin(), c.end());
}

void Integration::makeGaussMatrix(std::vector< std::vector<double> >& gm)
{
	unsigned int begin = 0;
	unsigned int start = begin + 1;
	unsigned int n = gm.size();
	unsigned int m = (n == 0 ? 0 : gm[0].size());
	while (start < n) {
		for (unsigned int k = start; k < n; ++k) {
			if (gm[begin][begin] == 0) {
				if (gm[k][begin] == 0) {
					continue;
				}
				gm[begin].swap(gm[k]);
			}
			if (gm[k][begin] == 0) {
				continue;
			}
			double coeff = gm[k][begin] / gm[begin][begin];
			for (unsigned int i = begin; i < m; ++i) {
				gm[k][i] -= gm[begin][i] * coeff;
				if (fabs(gm[k][i]) <= 0.0000001) {
					gm[k][i] = 0;
				}
			}
		}
		++start;
		++begin;
	}
}

std::pair<double, std::string> Integration::calcGauss(const std::vector<double>& points,
													  const std::vector<double>& c)
{
	std::string gauss = "";
	double val = 0;
	typedef std::vector<double>::const_iterator CVI;
	for (CVI i = points.begin(), j = c.begin(); i != points.end() && j != c.end(); ++i, ++j) {
		double func = f(*i);
		val += *j * func;
		if (j != c.begin()) {
			gauss += (*j < 0 ? "-" : "+");
		}
		else {
			gauss += (*j < 0 ? "-" : "");
		}
                gauss += QString::number(*j).toStdString();
		gauss += "*f(";
                gauss += QString::number(*i).toStdString();
		gauss += ")";
	}

	return std::pair<double, std::string>(val, gauss);
}
