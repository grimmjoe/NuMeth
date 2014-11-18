#include "divideddiffs.h"
#include "divnode.h"

void DividedDiffs::calcDividedDiffs()
{
	typedef std::vector<std::vector<DivNode> >::iterator VI;
	typedef std::vector<std::vector<DivNode> >::const_iterator CVI;
	typedef std::vector<DivNode>::iterator DI;
	typedef std::vector<DivNode>::const_iterator CDNI;
	std::vector<std::vector<DivNode> > divDiff;
	if (data.empty()) {
		return;
	}
	std::vector<DivNode> v;
	for (CDI i = data.begin(); i != data.end(); ++i) {
		v.push_back(DivNode(i->y(), i->x()));
	}
	divDiff.push_back(v);
	int n = data.size();
	--n;
	int k = 0;
	while (n > 0) {
		v.clear();
		for (DI i = divDiff[k].begin(); i != divDiff[k].end() - 1; ++i) {
			double val = ((i+1)->value - i->value) / ((i+1)->lastX - i->firstX);
			v.push_back(DivNode(val, i->firstX, (i+1)->lastX));
		}
		divDiff.push_back(v);
		++k; --n;
		v.clear();
	}

	std::vector<double> vv;
	for (CVI i = divDiff.begin(); i != divDiff.end(); ++i) {
		vv.clear();
		for (CDNI j = i->begin(); j != i->end(); ++j) {
			vv.push_back(j->value);
		}
		diffs.push_back(vv);
		vv.clear();
	}
}

void DividedDiffs::getDividedDiffs(std::vector<std::vector<double> > &d) const
{
	d = diffs;
}