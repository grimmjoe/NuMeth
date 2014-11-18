#ifndef DIVIDEDDIFFS_H
#define DIVIDEDDIFFS_H

#include <vector>
#include <QPointF>

class DividedDiffs
{
public:
	typedef std::vector<QPointF>::const_iterator CDI;
	DividedDiffs(const std::vector<QPointF>& d) : data(d) { }
	DividedDiffs(const DividedDiffs& dd) : data(dd.data), diffs(dd.diffs) { }
	void setData(const std::vector<QPointF>& d)
	{
		data = d;
		diffs.clear();
	}
	void calcDividedDiffs();
	void getDividedDiffs(std::vector< std::vector<double> >& d) const;
private:
	std::vector<QPointF> data;
	std::vector<std::vector<double> > diffs;
};

#endif
