#ifndef PLOTTER_H
#define PLOTTER_H

#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QMap>

class PlotSettings;
class QToolButton;


class Plotter : public QWidget
{
	Q_OBJECT
public:
	Plotter(QWidget* parent = 0);
	void setPlotSettings(const PlotSettings& );
	void setCurveData(int , const QVector<QPointF>& );
	void clearCurve(int );
	QSize sizeHint() const;
	QSize minimumSizeHint() const;
public slots:
	void zoomIn();
	void zoomOut();
protected:
	void mousePressEvent(QMouseEvent* );
	void mouseMoveEvent(QMouseEvent* );
	void mouseReleaseEvent(QMouseEvent* );
	void keyPressEvent(QKeyEvent* );
	void wheelEvent(QWheelEvent* );
	void paintEvent(QPaintEvent* );
	void resizeEvent(QResizeEvent* );
private:
	void updateRubberBandRegion();
	void refreshPixmap();
	void drawGrid(QPainter* );
	void drawCurves(QPainter* );

	enum { Margin = 50 };
	QToolButton* zoomInButton;
	QToolButton* zoomOutButton;
	QMap<int, QVector<QPointF> > curveMap;
	QVector<PlotSettings> zoomStack;
	int curZoom;
	bool rubberBandIsShown;
	QRect rubberBandRect;
	QPixmap pixmap;
};

class PlotSettings
{
public:
	PlotSettings();

	double minX;
	double maxX;
	int numXTicks;
	double minY;
	double maxY;
	int numYTicks;
	
	double intervalX() const { return maxX - minX; }
	double intervalY() const { return maxY - minY; }
	void scroll(int , int );
	void adjust();
private:
	static void adjustAxis(double& , double& , int& );
};

#endif

