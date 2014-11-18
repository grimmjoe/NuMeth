#ifndef DIVNODE_H
#define DIVNODE_H

struct DivNode
{
	double value;
	double firstX;
	double lastX;
	DivNode(double v, double x1, double x2) : value(v), firstX(x1), lastX(x2) { }
	DivNode(double v, double x) : value(v), firstX(x), lastX(x) { }
};

#endif