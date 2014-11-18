#include <QtGui>
#include <fstream>
#include "mainwindow.h"
#include "interpolation.h"
#include "integration.h"
#include "numethwizard.h"
#include "plotter.h"

bool pointXLess(const QPointF& p1, const QPointF& p2)
{
    return p1.x() < p2.x();
}

double getMinY(const QVector<QPointF>& vec)
{
    if (vec.isEmpty()) {
        return 0;
    }
    double minY = vec.front().y();
    for (QVector<QPointF>::ConstIterator i = vec.begin() + 1; i != vec.end(); ++i) {
        if (i->y() < minY) {
            minY = i->y();
        }
    }

    return minY;
}

double getMaxY(const QVector<QPointF>& vec)
{
    if (vec.isEmpty()) {
        return 0;
    }
    double maxY = vec.front().y();
    for (QVector<QPointF>::ConstIterator i = vec.begin() + 1; i != vec.end(); ++i) {
        if (i->y() > maxY) {
            maxY = i->y();
        }
    }

    return maxY;
}

MainWindow::MainWindow()
{
	Integration::init();
	createActions();
    createMenus();
    createToolBars();
    interpolation = 0;
    integration = 0;
    plotter = 0;
    disableAllActions();
	interpolationMenu->setEnabled(false);
	interpolationMenu->setEnabled(false);
	integrationMenu->setEnabled(false);
	integrationToolBar->setEnabled(false);
    readSettings();

    setWindowIcon(QIcon(":/images/nm.jpg"));
    setCurrentFile("");
	setWindowModified(false);
}

void MainWindow::createActions()
{
    createSessionActions();
    createInterpolationActions();
    createIntegrationActions();
    createCalcActions();

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show an information about this product"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About Qt"), this);
    aboutQtAction->setStatusTip(tr("Show information about Qt"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createSessionActions()
{
    newAction = new QAction(tr("&New"), this);
    newAction->setIcon(QIcon(":/images/new.png"));
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new session file"));;
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing session file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon(":/images/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the current session file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("&Save As"), this);
    saveAsAction->setStatusTip(tr("Save the current session file in another name"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    closeAction = new QAction(tr("&Close"), this);
    closeAction->setShortcut(QKeySequence::Close);
    closeAction->setStatusTip(tr("Close the current session"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setIcon(QIcon(":/images/exit.png"));
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
}

void MainWindow::createInterpolationActions()
{
    interpolateLagrangeAction = new QAction(tr("Lagrange"), this);
    interpolateLagrangeAction->setIcon(QIcon(":/images/lagrange.png"));
    interpolateLagrangeAction->setStatusTip(tr("Interpolate with Lagrange polynom"));
    connect(interpolateLagrangeAction, SIGNAL(triggered()),
            this, SLOT(interpolateLagrange()));
    interpolationActions.push_back(interpolateLagrangeAction);

    interpolateNewtonAction = new QAction(tr("Newton"), this);
    interpolateNewtonAction->setIcon(QIcon(":/images/newton.png"));
    interpolateNewtonAction->setStatusTip(tr("Interpolate with Newton polynom"));
    connect(interpolateNewtonAction, SIGNAL(triggered()),
            this, SLOT(interpolateNewton()));
    interpolationActions.push_back(interpolateNewtonAction);

    linearConstStepAction = new QAction(tr("Linear const"), this);
    linearConstStepAction->setIcon(QIcon(":/images/linearConst.png"));
    linearConstStepAction->setStatusTip(tr("Calculate const step for linear"
                                           " interpolation"));
    connect(linearConstStepAction, SIGNAL(triggered()),
            this, SLOT(linearConstStep()));
	linearConstStepAction->setEnabled(false);
    //interpolationActions.push_back(linearConstStepAction);

    linearVarStepAction = new QAction(tr("Linear var"), this);
    linearVarStepAction->setIcon(QIcon(":/images/linearVar.png"));
    linearVarStepAction->setStatusTip(tr("Calculate variable step for linear"
                                         " interpolation"));
    connect(linearVarStepAction, SIGNAL(triggered()),
            this, SLOT(linearVarStep()));
	linearVarStepAction->setEnabled(false);
    //interpolationActions.push_back(linearVarStepAction);

    quadrateConstStepAction = new QAction(tr("Quadrate const"), this);
    quadrateConstStepAction->setIcon(QIcon(":/images/quadrateConst.png"));
    quadrateConstStepAction->setStatusTip(tr("Calculate const step for "
                                             "quadrate interpolation"));
    connect(quadrateConstStepAction, SIGNAL(triggered()),
            this, SLOT(quadrateConstStep()));
	quadrateConstStepAction->setEnabled(false);
    //interpolationActions.push_back(quadrateConstStepAction);

    quadrateVarStepAction = new QAction(tr("Quadrate var"), this);
    quadrateVarStepAction->setIcon(QIcon(":/images/quadrateVar.png"));
    quadrateVarStepAction->setStatusTip(tr("Calculate variable step for "
                                           "quadrate interpolation"));
    connect(quadrateVarStepAction, SIGNAL(triggered()),
            this, SLOT(quadrateVarStep()));
	quadrateVarStepAction->setEnabled(false);
    //interpolationActions.push_back(quadrateVarStepAction);

    cubeConstStepAction = new QAction(tr("Cube const"), this);
    cubeConstStepAction->setIcon(QIcon(":/images/cubeConst.png"));
    cubeConstStepAction->setStatusTip(tr("Calculate const step for "
                                         "cube interpolation"));
    connect(cubeConstStepAction, SIGNAL(triggered()),
            this, SLOT(cubeConstStep()));
	cubeConstStepAction->setEnabled(false);
    //interpolationActions.push_back(cubeConstStepAction);

    cubeVarStepAction = new QAction(tr("Cube var"), this);
    cubeVarStepAction->setIcon(QIcon(":/images/cubeVar.png"));
    cubeVarStepAction->setStatusTip(tr("Calculate variable step for "
                                       "cube interpolation"));
    connect(cubeVarStepAction, SIGNAL(triggered()),
            this, SLOT(cubeVarStep()));
	cubeVarStepAction->setEnabled(false);
    //interpolationActions.push_back(cubeVarStepAction);

    dividedDiffsAction = new QAction(tr("Divided Differences"), this);
    dividedDiffsAction->setIcon(QIcon(":/images/divdiff.png"));
    dividedDiffsAction->setStatusTip(tr("Show divided differences"));
    connect(dividedDiffsAction, SIGNAL(triggered()),
            this, SLOT(dividedDiffs()));
	dividedDiffsAction->setEnabled(false);
    //interpolationActions.push_back(dividedDiffsAction);

    constSequenceAction = new QAction(tr("Equidistance nodes"), this);
    constSequenceAction->setIcon(QIcon(":/images/seqConst.png"));
    constSequenceAction->setStatusTip(tr("Show interpolation polynom sequence with "
                                         "equidistance nodes"));
    connect(constSequenceAction, SIGNAL(triggered()),
            this, SLOT(constSequence()));
    interpolationActions.push_back(constSequenceAction);

    tchebishevSequenceAction = new QAction(tr("Optimal nodes"), this);
    tchebishevSequenceAction->setIcon(QIcon(":/images/tchebishev.png"));
    tchebishevSequenceAction->setStatusTip(tr("Show interpolation polynom sequence "
                                              "with optimal nodes"));
    connect(tchebishevSequenceAction, SIGNAL(triggered()),
            this, SLOT(tchebishevSequence()));
    interpolationActions.push_back(tchebishevSequenceAction);

    lagrangeErrorAction = new QAction(tr("Lagrange Error"), this);
    lagrangeErrorAction->setIcon(QIcon(":/images/errorLag.png"));
    lagrangeErrorAction->setStatusTip(tr("Calculate Lagrange interpolation error"));
    connect(lagrangeErrorAction, SIGNAL(triggered()),
            this, SLOT(lagrangeError()));
	lagrangeErrorAction->setEnabled(false);
    //interpolationActions.push_back(lagrangeErrorAction);
}

void MainWindow::createIntegrationActions()
{
    integrateSimpsonAction = new QAction(tr("Simpson formula"), this);
    integrateSimpsonAction->setIcon(QIcon(":/images/simpson.png"));
    integrateSimpsonAction->setStatusTip(tr("Integrate with Simpson formula"));
    connect(integrateSimpsonAction, SIGNAL(triggered()),
            this, SLOT(integrateSimpson()));
    integrationActions.push_back(integrateSimpsonAction);

    integrateRectangleAction = new QAction(tr("Rectangle formula"), this);
    integrateRectangleAction->setIcon(QIcon(":/images/rec.png"));
    integrateRectangleAction->setStatusTip(tr("Integrate with rectangles formula"));
    connect(integrateRectangleAction, SIGNAL(triggered()),
            this, SLOT(integrateRectangle()));
    integrationActions.push_back(integrateRectangleAction);

    integrateTrapezoidalAction = new QAction(tr("Trapezoid formula"), this);
    integrateTrapezoidalAction->setIcon(QIcon(":/images/trap.png"));
    integrateTrapezoidalAction->setStatusTip(tr("Integrate with trapezoids formula"));
    connect(integrateTrapezoidalAction, SIGNAL(triggered()),
            this, SLOT(integrateTrapezoidal()));
    integrationActions.push_back(integrateTrapezoidalAction);

    integrateGaussAction = new QAction(tr("Gauss formula"), this);
    integrateGaussAction->setIcon(QIcon(":/images/gauss.png"));
    integrateGaussAction->setStatusTip(tr("Integrate with Gauss formula"));
    connect(integrateGaussAction, SIGNAL(triggered()),
            this, SLOT(integrateGauss()));
    integrationActions.push_back(integrateGaussAction);

    simpsonStepAction = new QAction(tr("Simpson step"), this);
    simpsonStepAction->setIcon(QIcon(":/images/simpsonStep.png"));
    simpsonStepAction->setStatusTip(tr("Calculate step for the Simpson formula"));
    connect(simpsonStepAction, SIGNAL(triggered()),
            this, SLOT(simpsonStep()));
	simpsonStepAction->setEnabled(false);
    //integrationActions.push_back(simpsonStepAction);

    trapezoidalStepAction = new QAction(tr("Trapezoidal step"), this);
    trapezoidalStepAction->setIcon(QIcon(":/images/trapStep.png"));
    trapezoidalStepAction->setStatusTip(tr("Calculate step for the "
                                           "trapezoids formula"));
    connect(trapezoidalStepAction, SIGNAL(triggered()),
            this, SLOT(trapezoidalStep()));
	trapezoidalStepAction->setEnabled(false);
    //integrationActions.push_back(trapezoidalStepAction);

    rectangleStepAction = new QAction(tr("Rectangular step"), this);
    rectangleStepAction->setIcon(QIcon(":/images/recStep.png"));
    rectangleStepAction->setStatusTip(tr("Calculate step for the rectangles formula"));
    connect(rectangleStepAction, SIGNAL(triggered()),
            this, SLOT(rectangleStep()));
	rectangleStepAction->setEnabled(false);
    //integrationActions.push_back(rectangleStepAction);

    simpsonErrorAction = new QAction(tr("Simpson formula error"), this);
    simpsonErrorAction->setIcon(QIcon(":/images/errorSimp.png"));
    simpsonErrorAction->setStatusTip(tr("Calculate Simpson formula error"));
    connect(simpsonErrorAction, SIGNAL(triggered()),
            this, SLOT(simpsonError()));
	simpsonErrorAction->setEnabled(false);
    //integrationActions.push_back(simpsonErrorAction);

    trapezoidErrorAction = new QAction(tr("Trapezoids formula error"), this);
    trapezoidErrorAction->setIcon(QIcon(":/images/errorTrap.png"));
    trapezoidErrorAction->setStatusTip(tr("Calculate Trapezoids formula error"));
    connect(trapezoidErrorAction, SIGNAL(triggered()),
            this, SLOT(trapezoidError()));
	trapezoidErrorAction->setEnabled(false);
    //integrationActions.push_back(trapezoidErrorAction);

    rectangleErrorAction = new QAction(tr("Rectangles formula error"), this);
    rectangleErrorAction->setIcon(QIcon(":/images/errorRec.png"));
    rectangleErrorAction->setStatusTip(tr("Calculate Rectangles formula error"));
    connect(rectangleErrorAction, SIGNAL(triggered()),
            this, SLOT(rectangleError()));
	rectangleErrorAction->setEnabled(false);
    //integrationActions.push_back(rectangleErrorAction);
}

void MainWindow::createCalcActions()
{
    funcCalcAction = new QAction(tr("Function value"), this);
    funcCalcAction->setIcon(QIcon(":/images/funcCalc.png"));
    funcCalcAction->setStatusTip(tr("Calculate function value"));
    connect(funcCalcAction, SIGNAL(triggered()),
            this, SLOT(funcCalc()));
    interpolationActions.push_back(funcCalcAction);

    lagrangeCalcAction= new QAction(tr("Lagrange value"), this);
    lagrangeCalcAction->setIcon(QIcon(":/images/lagrangeCalc.png"));
    lagrangeCalcAction->setStatusTip(tr("Calculate Lagrange polynom value"));
    connect(lagrangeCalcAction, SIGNAL(triggered()),
            this, SLOT(lagrangeCalc()));
    interpolationActions.push_back(lagrangeCalcAction);

    newtonCalcAction = new QAction(tr("Newton value"), this);
    newtonCalcAction->setIcon(QIcon(":/images/newtonCalc.png"));
    newtonCalcAction->setStatusTip(tr("Calculate Newton polynom value"));
    connect(newtonCalcAction, SIGNAL(triggered()),
            this, SLOT(newtonCalc()));
    interpolationActions.push_back(newtonCalcAction);
}

void MainWindow::createMenus()
{
    sessionMenu = menuBar()->addMenu(tr("&Session"));
    sessionMenu->addAction(newAction);
    sessionMenu->addAction(openAction);
    sessionMenu->addAction(saveAction);
    sessionMenu->addAction(saveAsAction);

    separatorAction = sessionMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i) {
        sessionMenu->addAction(recentFileActions[i]);
    }
    sessionMenu->addSeparator();
    sessionMenu->addAction(closeAction);
    sessionMenu->addAction(exitAction);

    interpolationMenu = menuBar()->addMenu(tr("&Interpolation"));
    interpolateSubMenu = interpolationMenu->addMenu(tr("&Interpolate..."));
    interpolateSubMenu->addAction(interpolateLagrangeAction);
    interpolateSubMenu->addAction(interpolateNewtonAction);
    calcSubMenu = interpolationMenu->addMenu(tr("Calculate..."));
    calcSubMenu->addAction(funcCalcAction);
    calcSubMenu->addAction(lagrangeCalcAction);
    calcSubMenu->addAction(newtonCalcAction);
    constStepSubMenu = interpolationMenu->addMenu(tr("Calc const step..."));
    constStepSubMenu->addAction(linearConstStepAction);
    constStepSubMenu->addAction(quadrateConstStepAction);
    constStepSubMenu->addAction(cubeConstStepAction);
    varStepSubMenu = interpolationMenu->addMenu(tr("Calc variable step..."));
    varStepSubMenu->addAction(linearVarStepAction);
    varStepSubMenu->addAction(quadrateVarStepAction);
    varStepSubMenu->addAction(cubeVarStepAction);
    polynomSeqSubMenu = interpolationMenu->addMenu(tr("Polynom sequences with..."));
    polynomSeqSubMenu->addAction(constSequenceAction);
    polynomSeqSubMenu->addAction(tchebishevSequenceAction);
    interpolationMenu->addAction(dividedDiffsAction);
    interpolationMenu->addAction(lagrangeErrorAction);

    integrationMenu = menuBar()->addMenu(tr("I&ntegration"));
    integrateSubMenu = integrationMenu->addMenu(tr("Integrate..."));
    integrateSubMenu->addAction(integrateSimpsonAction);
    integrateSubMenu->addAction(integrateTrapezoidalAction);
    integrateSubMenu->addAction(integrateRectangleAction);
    integrateSubMenu->addAction(integrateGaussAction);
    integStepSubMenu = integrationMenu->addMenu(tr("Calculate step..."));
    integStepSubMenu->addAction(simpsonStepAction);
    integStepSubMenu->addAction(trapezoidalStepAction);
    integStepSubMenu->addAction(rectangleStepAction);
    integErrorsSubMenu = integrationMenu->addMenu(tr("Error..."));
    integErrorsSubMenu->addAction(simpsonErrorAction);
    integErrorsSubMenu->addAction(trapezoidErrorAction);
    integErrorsSubMenu->addAction(rectangleErrorAction);
    menuBar()->addSeparator();
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
    sessionToolBar = addToolBar(tr("&Session"));
    sessionToolBar->addAction(newAction);
    sessionToolBar->addAction(openAction);
    sessionToolBar->addAction(saveAction);

    interpolationToolBar = addToolBar(tr("&Interpolation"));
    interpolationToolBar->addAction(interpolateLagrangeAction);
    interpolationToolBar->addAction(interpolateNewtonAction);
    interpolationToolBar->addAction(dividedDiffsAction);
    interpolationToolBar->addSeparator();
    interpolationToolBar->addAction(constSequenceAction);
    interpolationToolBar->addAction(tchebishevSequenceAction);

    integrationToolBar = addToolBar(tr("I&ntegration"));
    integrationToolBar->addAction(integrateSimpsonAction);
    integrationToolBar->addAction(integrateTrapezoidalAction);
    integrationToolBar->addAction(integrateRectangleAction);
    integrationToolBar->addAction(integrateGaussAction);
}

void MainWindow::newFile()
{
	if (okToContinue()) {
		setSessionType();
	}
}

bool MainWindow::okToContinue()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("NuMeth"), tr("The session has been"
                                       " modified.\nDo you want to save your "
                                       "settings?"), QMessageBox::Yes |
                                     QMessageBox::No | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
            return save();
        }
        else if (r == QMessageBox::Cancel) {
            return false;
        }
    }

    return true;
}

void MainWindow::setSessionType()
{
    NuMethWizard wizard(this);
    if (wizard.exec()) {
		clear();
		disableAllActions();
		bool interpol = wizard.field("new.interpolation").toBool();
        if (interpol) {
			bool withPlainData = wizard.field("interpol.withPlainData").toBool();
            bool withFuncData = wizard.field("interpol.withFuncData").toBool();
            if (withPlainData) {
                QString fileName = wizard.field("interpol.plainFileName").toString();
                std::ifstream in(fileName.toStdString().c_str());
                std::vector<QPointF> data;
                getInterpolationData(in, data);
                interpolation = new Interpolation(data);
                enableActions(InterpolPlain_Session);
            }
            else if (withFuncData) {
                QString funcName = wizard.field("interpolF.function").toString();
                double beg = wizard.field("interpolF.intervalBegin").toDouble();
                double end = wizard.field("interpolF.intervalEnd").toDouble();
                Func f(funcName.toStdString());
                Interval interval(beg, end);
                interpolation = new Interpolation(f, interval);
                enableActions(InterpolFunc_Session);
            }
            else {
                QString fileName = wizard.field("interpolFP.plainFileName").toString();
                std::ifstream in(fileName.toStdString().c_str());
                std::vector<double> data;
                getInterpolationData(in, data);
                QString funcName = wizard.field("interpolFP.function").toString();
                double beg = wizard.field("interpolFP.intervalBegin").toDouble();
                double end = wizard.field("interpolFP.intervalEnd").toDouble();
                Func f(funcName.toStdString());
                Interval interval(beg, end);
                interpolation = new Interpolation(f, interval, data);
				enableActions(InterpolFuncPlain_Session);
            }
			interpolationMenu->setEnabled(true);
			interpolationToolBar->setEnabled(true);
			integrationMenu->setEnabled(false);
			integrationToolBar->setEnabled(false);
        }
        else {
			integrationMenu->setEnabled(true);
			integrationToolBar->setEnabled(true);
			interpolationMenu->setEnabled(false);
			interpolationMenu->setEnabled(false);
            QString funcName = wizard.field("integ.function").toString();
            double beg = wizard.field("integ.intervalBegin").toDouble();
            double end = wizard.field("integ.intervalEnd").toDouble();
			Func f(funcName.toStdString());
            Interval interval(beg, end);
            integration = new Integration(f, interval);
            enableIntegration();
        }
		setCurrentFile("");
		setWindowModified(true);
    }
}

void MainWindow::getInterpolationData(std::ifstream& input,
                                      std::vector<QPointF>& data)
{
    double x = 0;
    double y = 0;
    while (input >> x >> y) {
        data.push_back(QPointF(x, y));
    }
}

void MainWindow::getInterpolationData(std::ifstream& input,
                                      std::vector<double>& data)
{
    double x = 0;
    while (input >> x) {
        data.push_back(x);
    }
}

void MainWindow::open()
{
    if (okToContinue()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open NuMeth"),
                    ".", tr("NuMeth files (*.nm)"));

        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
        setWindowModified(false);
    }
}

bool MainWindow::loadFile(const QString& fileName)
{
    if (!readFile(fileName)) {
        statusBar()->showMessage(tr("Loading cancelled"), 2000);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        setWindowModified(false);
        return saveAs();
    }
    else {
        setWindowModified(false);
        return saveFile(curFile);
    }
}

bool MainWindow::saveFile(const QString& fileName)
{
    if (!writeFile(fileName)) {
        statusBar()->showMessage(tr("Saving cancelled"), 2000);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File Saved"), 2000);
    return true;
}

bool MainWindow::writeFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("NuMeth"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_3);
    out << quint32(MagicNumber);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool interpol = (interpolation != 0 ? true : false);
    out << quint8(interpol);
    if (interpolation) {
        std::string fN = interpolation->getFunc().func();
        Interval in = interpolation->getInterval();
        std::vector<QPointF> d;
        interpolation->getData(d);
        out << QString(fN.c_str()) << in.begin() << in.end();
        for (std::vector<QPointF>::const_iterator i = d.begin(); i != d.end(); ++i) {
            out << i->x() << i->y();
        }
    }
    else {
        std::string fN = integration->getFunc().func();
        Interval in = integration->getInterval();
        out << QString(fN.c_str()) << in.begin() << in.end();
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool MainWindow::readFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("NuMeth"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_3);
    quint32 mn;
    in >> mn;
    if (mn != MagicNumber) {
        QMessageBox::warning(this, tr("NuMeth"),
                             tr("The file is not a NuMeth file."));
        return false;
    }

    clear();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    quint8 interpol;
    in >> interpol;
    std::string fName;
    QString fN;
    double beg;
    double end;
    in >> fN >> beg >> end;
    fName = fN.toStdString();
    Func f(fName);
    Interval interval(beg, end);
    if (interpol) {
        double x;
        double y;
        std::vector<QPointF> data;
        while(!in.atEnd()) {
            in >> x >> y;
            QPointF point(x, y);
            data.push_back(point);
        }
        if (fName.empty()) {
            interpolation = new Interpolation(data);
            disableAllActions();
            enableInterpolation();
            disableFuncInterpolation();
            funcCalcAction->setEnabled(false);
        }
        else {
            if (data.empty()) {
                interpolation = new Interpolation(f, interval);
                disableAllActions();
                enableInterpolation();
                disablePlainInterpolation();
            }
            else {
                std::vector<double> xS;
                for (std::vector<QPointF>::const_iterator i = data.begin();
                                i != data.end(); ++i) {
                    xS.push_back(i->x());
                }
                interpolation = new Interpolation(f, interval, xS);
                disableAllActions();
                enableInterpolation();
            }
        }
        integrationMenu->setEnabled(false);
        integrationToolBar->setEnabled(false);
        interpolationMenu->setEnabled(true);
        interpolationToolBar->setEnabled(true);
    }
    else {
        integration = new Integration(f, interval);
        disableAllActions();
        enableIntegration();
        interpolationMenu->setEnabled(false);
        interpolationToolBar->setEnabled(false);
        integrationMenu->setEnabled(true);
        integrationToolBar->setEnabled(true);
    }
    QApplication::restoreOverrideCursor();

    return true;
}

void MainWindow::clear()
{
    if (interpolation) {
        delete interpolation;
        interpolation = 0;
    }
    if (integration) {
        delete integration;
        integration = 0;
    }

    if (plotter) {
        delete plotter;
        plotter = 0;
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save NuMeth"),
                    ".", tr("NuMeth files (*.nm)"));

    if (fileName.isEmpty()) {
        return false;
    }

    return saveFile(fileName);
}

void MainWindow::closeEvent(QCloseEvent* cEvent)
{
    if (okToContinue()) {
        writeSettings();
        cEvent->accept();
    }
    else {
        cEvent->ignore();
    }
}

void MainWindow::setCurrentFile(const QString& fileName)
{
    curFile = fileName;
    setWindowModified(false);
    QString name = tr("Untitled");
    if (!curFile.isEmpty()) {
        name = strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);
        updateRecentFileActions();
    }

	if (interpolation) {
		name += " Interpolation session";
	}
	else if (integration) {
		name += " Integration session";
	}

    setWindowTitle(tr("%1[*] - %2").arg(name).arg(tr("NuMeth")));
}

QString MainWindow::strippedName(const QString& fullName)
{
    return QFileInfo(fullName).fileName();
}

void MainWindow::updateRecentFileActions()
{
    QMutableStringListIterator i(recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next())) {
            i.remove();
        }
    }

    for (int j = 0; j < MaxRecentFiles; ++j) {
        if (j < recentFiles.count()) {
            QString text = tr("&%1 %2")
							.arg(j + 1)
                            .arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        }
        else {
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentFiles.isEmpty());
}

void MainWindow::openRecentFile()
{
    if (okToContinue()) {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action) {
            loadFile(action->data().toString());
        }
    }
}

void MainWindow::writeSettings()
{
    QSettings settings("A.H.A. Inc.", "NuMeth");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", recentFiles);
}

void MainWindow::readSettings()
{
    QSettings settings("A.H.A. Inc.", "NuMeth");
    restoreGeometry(settings.value("geometry").toByteArray());
    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();
}

void MainWindow::interpolateLagrange()
{
    QVector<QPointF> grData;
    interpolation->makeLagrangeGrData(grData);
    if (!plotter) {
        plotter = new Plotter(this);
		setCentralWidget(plotter);
    }

	PlotSettings settings;
	settings.minX = grData.front().x() - 0.5;
	settings.maxX = grData.back().x() + 0.5;
	settings.minY = getMinY(grData) - 0.5;
	settings.maxY = getMaxY(grData) + 0.5;
	plotter->setPlotSettings(settings);
    plotter->setCurveData(0, grData);
    plotter->show();
    plotter->raise();
    plotter->activateWindow();
}

void MainWindow::interpolateNewton()
{
    QVector<QPointF> grData;
    interpolation->makeDividedDiffs();
    interpolation->makeNewtonGrData(grData);
    if (!plotter) {
        plotter = new Plotter(this);
		setCentralWidget(plotter);
    }

	PlotSettings settings;
	settings.minX = grData.front().x() - 0.5;
	settings.maxX = grData.back().x() + 0.5;
	settings.minY = getMinY(grData) - 0.5;
	settings.maxY = getMaxY(grData) + 0.5;
	plotter->setPlotSettings(settings);
    plotter->setCurveData(1, grData);
    plotter->show();
    plotter->raise();
    plotter->activateWindow();
}

void MainWindow::linearConstStep()
{
    std::pair<QVariant, bool> eps = getEpsilon();
    if (eps.second) {
        QVariant h = interpolation->linearConstStep(eps.first.toDouble());
        QMessageBox::information(this, tr("NuMeth"), tr("The const step for linear "
                        "interpolation with eps = ") + eps.first.toString() + tr(" is:\n")
                        + h.toString(), QMessageBox::Ok);
    }
}

std::pair<QVariant, bool> MainWindow::getEpsilon()
{
    bool ok = false;
    QVariant eps = 0.0001;
    eps = QInputDialog::getDouble(this, tr("NuMeth"), tr("Epsilon:"),
                                             0.0001, 0, 1000, 7, &ok);
    return std::pair<QVariant, bool>(eps, ok);
}

void MainWindow::linearVarStep()
{
    //QVariant eps = getEpsilon();

}

void MainWindow::quadrateConstStep()
{
   std::pair<QVariant, bool> eps = getEpsilon();
    if (eps.second) {
        QVariant h = interpolation->quadrateConstStep(eps.first.toDouble());
        QMessageBox::information(this, tr("NuMeth"), tr("The const step for quadrate "
                 "interpolation with eps = ") + eps.first.toString() + tr(" is:\n")
                 + h.toString(), QMessageBox::Ok);
    }
}


void MainWindow::quadrateVarStep()
{
    //QVariant eps = getEpsilon();

}

void MainWindow::cubeConstStep()
{
    std::pair<QVariant, bool> eps = getEpsilon();
    if (eps.second) {
        QVariant h = interpolation->cubeConstStep(eps.first.toDouble());
        QMessageBox::information(this, tr("NuMeth"), tr("The const step for cube "
                "interpolation with eps = ") + eps.first.toString() + tr(" is:\n")
                + h.toString(), QMessageBox::Ok);
    }
}

void MainWindow::cubeVarStep()
{
    //QVariant eps = getEpsilon();

}

void MainWindow::lagrangeError()
{
    QVariant err = interpolation->calcLagrangeError();
    QMessageBox::information(this, tr("NuMeth"), tr("Lagrange error:\n") +
                             err.toString(), QMessageBox::Ok);
}

void MainWindow::integrateSimpson()
{
    QVariant val = integration->integrateWithSimpson();
    QMessageBox::information(this, tr("NuMeth"), tr("Simpson integral:\n") +
                             val.toString(), QMessageBox::Ok);
}

void MainWindow::integrateTrapezoidal()
{
    QVariant val = integration->integrateWithTrapezoid();
    QMessageBox::information(this, tr("NuMeth"), tr("Trapezoidal integral:\n") +
                             val.toString(), QMessageBox::Ok);
}

void MainWindow::integrateRectangle()
{
    QVariant val = integration->integrateWithRectangle();
    QMessageBox::information(this, tr("NuMeth"), tr("Rectangle integral:\n") +
                             val.toString(), QMessageBox::Ok);
}

void MainWindow::integrateGauss()
{
	bool ok = true;
    int n = QInputDialog::getInteger(this, tr("NuMeth"),
                        tr("Enter the number of points in Gauss formula(from 1 to 4): "),
                        3, 1, 4, 1, &ok);
	if (ok) {
		std::pair<double, std::string> g = integration->gauss(n);
		QString gauss(g.second.c_str());
        QMessageBox::information(this, tr("NuMeth"), tr("Gauss formula and the "
                        "value for %1 points are\n%2\n%3")
                                 .arg(n)
                                 .arg(gauss)
                                 .arg(g.first), QMessageBox::Ok);
    }
}

void MainWindow::funcCalc()
{
    bool ok = false;
    QVariant x = QInputDialog::getDouble(this, tr("NuMeth - Function value calculation"), tr("Enter the x: "),
                                         9.5, -1500, 1500, 4, &ok);
    if (ok) {
        Func g = interpolation->getFunc();
        QVariant d = g(x.toDouble());
        QMessageBox::information(this, tr("NuMeth"), tr("Function value in ") +
                    x.toString() + tr(" : ") + d.toString(), QMessageBox::Ok);
    }
}

void MainWindow::lagrangeCalc()
{
    bool ok = false;;
    QVariant x = QInputDialog::getDouble(this, tr("NuMeth"), tr("Enter the x: "),
                                         9.5, -15000, 15000, 4, &ok);
    if (ok) {
        QVariant d = interpolation->calcLagrange(x.toDouble());
        QMessageBox::information(this, tr("NuMeth"), tr("Lagrange polynom value in ")
                    + x.toString() + tr(" : ") + d.toString(), QMessageBox::Ok);
    }
}

void MainWindow::newtonCalc()
{
   bool ok = false;
   QVariant x = QInputDialog::getDouble(this, tr("NuMeth"), tr("Enter the x: "),
                                         9.5, -15000, 15000, 4, &ok);
   if (ok) {
       QVariant d = interpolation->calcNewton(x.toDouble());
       QMessageBox::information(this, tr("NuMeth"), tr("Newton polynom value in ")
                    + x.toString() + tr(" : ") + d.toString(), QMessageBox::Ok);
   }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About NuMeth"),
                       tr("<h2>NuMeth Demo</h2>"
                          "<p>Copyright &copy; A.H.A. Inc."
                          "<p>NuMeth is a small application that demonstrates the"
                          " main features of numerical methods such as "
                          "numerical interpolation and integration.\n It was developed"
                          " using C++ with it's toolkit Qt.\n\nDeveloped by:"
                          "Hamlet Aslanyan      A.H.A. Inc."));
}

/*void MainWindow::dividedDiffs()
{
    interpolation->makeDividedDiffs();
    std::vector< std::vector<double> > dd;
    interpolation->getDividedDiffs(dd);

}*/

void MainWindow::constSequence()
{
    if (!plotter) {
        plotter = new Plotter;
        setCentralWidget(plotter);
    }
    QVector<QPointF> data;
    interpolation->makeFunctionGrData(data);
    qStableSort(data.begin(), data.end(), pointXLess);
    PlotSettings settings;
    settings.minX = data.front().x() - 0.5;
    settings.maxX = data.back().x() + 0.5;
    settings.minY = getMinY(data) - 0.5;
    settings.maxY = getMaxY(data) + 0.5;
    plotter->setPlotSettings(settings);
    plotter->setCurveData(0, data);
    plotter->clearCurve(1);
    plotter->show();
    bool ok = true;
    int def = 1;
    do {
        int n = QInputDialog::getInteger(this, tr("NuMeth: Equidistance interpolation"), tr("Enter the "
                        "interpolation polynom degree:"), def, 1, 40, 1, &ok);
        if (ok) {
            QVector<QPointF> data;
            interpolation->constN(n, data);
            qStableSort(data.begin(), data.end(), pointXLess);
            plotter->setCurveData(1, data);
            def = n;
        }
    } while (ok);
}

void MainWindow::tchebishevSequence()
{
    if (!plotter) {
        plotter = new Plotter;
        setCentralWidget(plotter);
    }
    QVector<QPointF> data;
    interpolation->makeFunctionGrData(data);
    qStableSort(data.begin(), data.end(), pointXLess);
    PlotSettings settings;
    settings.minX = data.front().x() - 0.5;
    settings.maxX = data.back().x() + 0.5;
    settings.minY = getMinY(data) - 0.5;
    settings.maxY = getMaxY(data) + 0.5;
    plotter->setPlotSettings(settings);
    plotter->setCurveData(0, data);
    plotter->clearCurve(1);
    plotter->show();
    bool ok = true;
    int def = 1;
    do {
        int n = QInputDialog::getInteger(this, tr("NuMeth: Optimal interpolation"), tr("Enter the "
                    "interpolation polynom degree:"), def, 1, 40, 1, &ok);
        if (ok) {
            QVector<QPointF> data;
            interpolation->tchebishevN(n, data);
            qStableSort(data.begin(), data.end(), pointXLess);
            plotter->setCurveData(1, data);
            def = n;
        }
    } while (ok);
}

void MainWindow::disablePlainInterpolation()
{
    lagrangeCalcAction->setEnabled(false);
    newtonCalcAction->setEnabled(false);
    interpolateLagrangeAction->setEnabled(false);
    interpolateNewtonAction->setEnabled(false);
    dividedDiffsAction->setEnabled(false);
    lagrangeErrorAction->setEnabled(false);
}

void MainWindow::disableFuncInterpolation()
{
    linearConstStepAction->setEnabled(false);
    linearVarStepAction->setEnabled(false);
    quadrateConstStepAction->setEnabled(false);
    quadrateVarStepAction->setEnabled(false);
    cubeConstStepAction->setEnabled(false);
    cubeVarStepAction->setEnabled(false);
	constSequenceAction->setEnabled(false);
	tchebishevSequenceAction->setEnabled(false);
	funcCalcAction->setEnabled(false);
}

void MainWindow::enableInterpolation()
{
    for (QList<QAction*>::Iterator i = interpolationActions.begin();
    i != interpolationActions.end(); ++i) {
        (*i)->setEnabled(true);
    }
}

void MainWindow::enableIntegration()
{
    for (QList<QAction*>::Iterator i = integrationActions.begin();
    i != integrationActions.end(); ++i) {
        (*i)->setEnabled(true);
    }
}

void MainWindow::disableAllActions()
{
    for (QList<QAction*>::Iterator i = interpolationActions.begin();
    i != interpolationActions.end(); ++i) {
        (*i)->setEnabled(false);
    }

    for (QList<QAction*>::Iterator i = integrationActions.begin();
    i != integrationActions.end(); ++i) {
        (*i)->setEnabled(false);
    }
}

void MainWindow::enableActions(int id)
{
    switch (id) {
    case Interpol_Session: case InterpolFuncPlain_Session:
        enableInterpolation();
        break;
    case InterpolPlain_Session:
        enableInterpolation();
        disableFuncInterpolation();
        break;
    case InterpolFunc_Session:
        enableInterpolation();
        disablePlainInterpolation();
        break;
    case Integ_Session:
        enableIntegration();
    default:
        disableAllActions();
    }
}

