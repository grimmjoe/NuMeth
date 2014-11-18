#ifndef NUMETHWIZARD_H
#define NUMETHWIZARD_H

#include <QWizard>
#include <QVector>

class QLabel;
class QRadioButton;
class QLineEdit;

class NuMethWizard : public QWizard
{
    Q_OBJECT

public:
    NuMethWizard(QWidget *parent = 0);
    enum { Page_New, Page_Interpol, Page_Integ, Page_InterPlainData,
		Page_InterFuncData, Page_InterFuncPlainData, Page_Conclusion };
private slots:
    void showHelp();
};

class NewPage : public QWizardPage
{
    Q_OBJECT
public:
    NewPage(QWidget* parent = 0);
    int nextId() const;
private:
    QLabel* nameLabel;
    QRadioButton* interpolButton;
    QRadioButton* integButton;
};

class InterpolPage : public QWizardPage
{
    Q_OBJECT
public:
    InterpolPage(QWidget* parent = 0);
    int nextId() const;
private:
    QRadioButton* plainDataButton;
    QRadioButton* funcDataButton;
    QRadioButton* funcPlainDataButton;
};

class IntegPage : public QWizardPage
{
    Q_OBJECT
public:
    IntegPage(QWidget* parent = 0);
    int nextId() const;
private:
    QLabel* funcLabel;
    QLabel* intervalLabel;
    QLineEdit* funcLineEdit;
    QLineEdit* intervalBegin;
    QLineEdit* intervalEnd;
};

class InterpolPlainDataPage : public QWizardPage
{
    Q_OBJECT
public:
    InterpolPlainDataPage(QWidget* parent = 0);
    int nextId() const;
private slots:
    void browse();
private:
    QLabel* fileLabel;
    QPushButton* browseButton;
    QLineEdit* fileLineEdit;
};

class InterpolFuncDataPage : public QWizardPage
{
    Q_OBJECT
public:
    InterpolFuncDataPage(QWidget* parent = 0);
    int nextId() const;
private:
    QLabel* funcLabel;
    QLabel* intervalLabel;
    QLineEdit* funcLineEdit;
    QLineEdit* intervalBegin;
    QLineEdit* intervalEnd;
};

class InterpolFuncPlainDataPage : public QWizardPage
{
    Q_OBJECT
public:
    InterpolFuncPlainDataPage(QWidget* parent = 0);
    int nextId() const;
private slots:
    void browse();
private:
    QLabel* fileLabel;
    QLabel* funcLabel;
    QLabel* intervalLabel;
    QPushButton* browseButton;
    QLineEdit* fileLineEdit;
    QLineEdit* funcLineEdit;
    QLineEdit* intervalBegin;
    QLineEdit* intervalEnd;
};

class ConclusionPage : public QWizardPage
{
    Q_OBJECT
public:
    ConclusionPage(QWidget* parent = 0);
    int nextId() const;
    void initializePage();
private:
    QLabel* conclusionLabel;
};

#endif // NUMETHWIZARD_H
