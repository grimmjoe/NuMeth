#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QList>

class QAction;
class QLabel;
class Interpolation;
class Integration;
class Plotter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
protected:
    void closeEvent(QCloseEvent* cEvent);
private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void interpolateLagrange();
    void interpolateNewton();
    void linearConstStep();
    void linearVarStep();
    void quadrateConstStep();
    void quadrateVarStep();
    void cubeConstStep();
    void cubeVarStep();
    //void dividedDiffs();
    void constSequence();
    void tchebishevSequence();
    void lagrangeError();
    void integrateSimpson();
    void integrateTrapezoidal();
    void integrateRectangle();
    void integrateGauss();
    /*void simpsonStep();
    void trapezoidalStep();
    void rectangleStep();
    void simpsonError();
    void trapezoidError();
    void rectangleError();*/
    void funcCalc();
    void lagrangeCalc();
    void newtonCalc();
    void about();
    void openRecentFile();
private:
    void createActions();
    void createSessionActions();
    void createInterpolationActions();
    void createIntegrationActions();
    void createCalcActions();
    void createMenus();
    void createToolBars();
    std::pair<QVariant, bool> getEpsilon();
    //void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString& fileName);
    bool saveFile(const QString& fileName);
    bool readFile(const QString& fileName);
    bool writeFile(const QString& fileName);
    void setCurrentFile(const QString& fileName);
    void updateRecentFileActions();
    void clear();
    QString strippedName(const QString& fullName);
    void enableActions(int id);
    void disablePlainInterpolation();
    void disableFuncInterpolation();
	void disableAllActions();
    void enableInterpolation();
    void enableIntegration();
    void setSessionType();
    void getInterpolationData(std::ifstream& input, std::vector<QPointF>& data);
    void getInterpolationData(std::ifstream& input, std::vector<double>& data);

    Interpolation* interpolation;
    Integration* integration;
    Plotter* plotter;
    QStringList recentFiles;
    QString curFile;

    enum { MaxRecentFiles = 5, MagicNumber = 0x7F51C815 };
    enum { Interpol_Session, InterpolFunc_Session, InterpolPlain_Session,
       InterpolFuncPlain_Session, Integ_Session };
    QAction* recentFileActions[MaxRecentFiles];
    QAction* separatorAction;
    QMenu* sessionMenu;
    QMenu* interpolationMenu;
    QMenu* interpolateSubMenu;
    QMenu* calcSubMenu;
    QMenu* constStepSubMenu;
    QMenu* varStepSubMenu;
    QMenu* polynomSeqSubMenu;
    QMenu* integrationMenu;
    QMenu* integrateSubMenu;
    QMenu* integStepSubMenu;
    QMenu* integErrorsSubMenu;
    QMenu* helpMenu;
    QToolBar* sessionToolBar;
    QToolBar* interpolationToolBar;
    QToolBar* integrationToolBar;
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* saveAsAction;
    QAction* closeAction;
    QAction* exitAction;
    QAction* interpolateLagrangeAction;
    QAction* interpolateNewtonAction;
    QAction* linearConstStepAction;
    QAction* linearVarStepAction;
    QAction* quadrateConstStepAction;
    QAction* quadrateVarStepAction;
    QAction* cubeConstStepAction;
    QAction* cubeVarStepAction;
    QAction* dividedDiffsAction;
    QAction* constSequenceAction;
    QAction* tchebishevSequenceAction;
    QAction* integrateSimpsonAction;
    QAction* integrateTrapezoidalAction;
    QAction* integrateRectangleAction;
    QAction* integrateGaussAction;
    QAction* simpsonStepAction;
    QAction* trapezoidalStepAction;
    QAction* rectangleStepAction;
    QAction* lagrangeErrorAction;
    QAction* simpsonErrorAction;
    QAction* trapezoidErrorAction;
    QAction* rectangleErrorAction;
    QAction* funcCalcAction;
    QAction* lagrangeCalcAction;
    QAction* newtonCalcAction;
    QAction* aboutAction;
    QAction* aboutQtAction;

    QList<QAction*> interpolationActions;
    QList<QAction*> integrationActions;
};

#endif // MAINWINDOW_H
