#ifndef CMP_H
#define CMP_H

#include <QPointF>

struct PointCmp
{
	bool operator ()(const QPointF& p1, const QPointF& p2)
	{
		return p1.x() < p2.x();
	}
};

#endif
