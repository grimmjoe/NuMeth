#include <QtGui>
#include "numethwizard.h"

NuMethWizard::NuMethWizard(QWidget *parent)
    : QWizard(parent)
{
    setPage(Page_New, new NewPage);
    setPage(Page_Interpol, new InterpolPage);
    setPage(Page_Integ, new IntegPage);
    setPage(Page_InterPlainData, new InterpolPlainDataPage);
    setPage(Page_InterFuncData, new InterpolFuncDataPage);
    setPage(Page_InterFuncPlainData, new InterpolFuncPlainDataPage);
    setPage(Page_Conclusion, new ConclusionPage);
    setStartId(Page_New);

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif
    setOption(HaveHelpButton, true);
    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo.png"));
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    setWindowTitle(tr("NuMeth wizard"));
}

void NuMethWizard::showHelp()
{
    static QString lastMessage;
    QString message;

    switch(currentId()) {
    case Page_New:
        message = tr("You must click one of the buttons to choose the "
                     "appropriate session.");
        break;
    case Page_Interpol:
        message = tr("You must click one of the buttons to choose the interpolation"
                     " mode.");
        break;
    case Page_Integ:
        message = tr("You must enter a valid function and an interval.");
        break;
    case Page_InterPlainData:
        message = tr("You must type the file path or click Browse to choose the file "
                     "with plain interpolation data.");
        break;
    case Page_InterFuncData:
        message = tr("You must enter a valid function and an interval.");
        break;
    case Page_InterFuncPlainData:
        message = tr("You must enter a valid function with an interval and "
                     "type the file path or click the Browse to choose the file "
                     "with plain interpolation data.");
        break;
    case Page_Conclusion:
        message = tr("You must click finish to finish the wizard.");
        break;
    default:
        message = tr("This help wasn't a help at all.");
        break;
    }

    if (lastMessage == message) {
        message = tr("Sorry, I've already helped you as much as I could. "
                     "Maybe you should ask a human.?");
    }

    QMessageBox::information(this, tr("NuMeth Wizard Help"), message);
    lastMessage = message;
}

NewPage::NewPage(QWidget* parent)
        : QWizardPage(parent)
{
    setTitle(tr("New Session"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.jpg"));

    nameLabel = new QLabel(tr("This wizard will help you to make a new "
                              "<i>NuMeth</i>&trade; session."));
    nameLabel->setWordWrap(true);

    interpolButton = new QRadioButton(tr("&Interpolation session"));
    integButton = new QRadioButton(tr("I&ntegration session"));
    interpolButton->setChecked(true);

    registerField("new.interpolation", interpolButton);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(nameLabel);
    layout->addWidget(interpolButton);
    layout->addWidget(integButton);
    setLayout(layout);
}

int NewPage::nextId() const
{
    if (interpolButton->isChecked()) {
        return NuMethWizard::Page_Interpol;
    }
    else {
        return NuMethWizard::Page_Integ;
    }
}

IntegPage::IntegPage(QWidget* parent)
        : QWizardPage(parent)
{
    setTitle(tr("Integration session."));
    setSubTitle(tr("Please enter a valid function and an interval for "
                   "<i>NuMeth</i>&trade; integration session."));

    funcLabel = new QLabel(tr("&Function:"));
    funcLineEdit = new QLineEdit;
    funcLabel->setBuddy(funcLineEdit);
    intervalLabel = new QLabel(tr("Interval:"));
    intervalBegin = new QLineEdit;
    intervalEnd = new QLineEdit;
	QDoubleValidator* dValidator = new QDoubleValidator(-15000, 15000, 10, this);
	intervalBegin->setValidator(dValidator);
	intervalEnd->setValidator(dValidator);
    
	registerField("integ.function*", funcLineEdit);
    registerField("integ.intervalBegin*", intervalBegin);
    registerField("integ.intervalEnd*", intervalEnd);

    QHBoxLayout* hFuncLayout = new QHBoxLayout;
    hFuncLayout->addWidget(funcLabel);
    hFuncLayout->addWidget(funcLineEdit);
    QHBoxLayout* hInterLayout = new QHBoxLayout;
    hInterLayout->addWidget(intervalLabel);
    hInterLayout->addWidget(intervalBegin);
    hInterLayout->addWidget(intervalEnd);
	hInterLayout->addStretch();
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(hFuncLayout);
    layout->addLayout(hInterLayout);
    setLayout(layout);
}

int IntegPage::nextId() const
{
    return NuMethWizard::Page_Conclusion;
}

InterpolPage::InterpolPage(QWidget* parent)
        : QWizardPage(parent)
{
    setTitle(tr("Interpolation session"));
    setSubTitle(tr("Please choose the type of "
                   "<i>NuMeth</i>&trade; interpolation session."));

    plainDataButton = new QRadioButton(tr("With &plain data"));
    funcDataButton = new QRadioButton(tr("With &function"));
    funcPlainDataButton = new QRadioButton(tr("With f&unction and plain data"));
    plainDataButton->setChecked(true);

    registerField("interpol.withPlainData", plainDataButton);
    registerField("interpol.withFuncData", funcDataButton);
    //registerField("interpol.withFuncPlainData", funcPlainDataButton);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(plainDataButton);
    layout->addWidget(funcDataButton);
    layout->addWidget(funcPlainDataButton);
    setLayout(layout);
}

int InterpolPage::nextId() const
{
    if (plainDataButton->isChecked()) {
        return NuMethWizard::Page_InterPlainData;
    }
    else if (funcDataButton->isChecked()) {
        return NuMethWizard::Page_InterFuncData;
    }

    return NuMethWizard::Page_InterFuncPlainData;
}

InterpolPlainDataPage::InterpolPlainDataPage(QWidget* parent)
        : QWizardPage(parent)
{
    setTitle(tr("Plain data interpolation session"));
    setSubTitle(tr("Please choose the file with plain interpolation data."));

    fileLabel = new QLabel(tr("&File:"));
    fileLineEdit = new QLineEdit;
    fileLabel->setBuddy(fileLineEdit);
    browseButton = new QPushButton(tr("&Browse..."));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
    registerField("interpol.plainFileName*", fileLineEdit);
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(fileLabel);
    layout->addWidget(fileLineEdit);
    layout->addWidget(browseButton);
    setLayout(layout);
}

void InterpolPlainDataPage::browse()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                    ".", tr("Text files(*.txt)"));
    if (!fileName.isEmpty()) {
        fileLineEdit->setText(QFileInfo(fileName).absoluteFilePath());
    }
}

int InterpolPlainDataPage::nextId() const
{
    return NuMethWizard::Page_Conclusion;
}

InterpolFuncDataPage::InterpolFuncDataPage(QWidget* parent)
        : QWizardPage(parent)
{
    setTitle(tr("Function interpolation session."));
    setSubTitle(tr("Please enter a valid function and an interval for "
                   "<i>NuMeth</i>&trade; interpolation session."));

    funcLabel = new QLabel(tr("&Function:"));
    funcLineEdit = new QLineEdit;
    funcLabel->setBuddy(funcLineEdit);
    intervalLabel = new QLabel(tr("Interval:"));
    intervalBegin = new QLineEdit;
    intervalEnd = new QLineEdit;
	QDoubleValidator* dValidator = new QDoubleValidator(-15000, 15000, 10, this);
	intervalBegin->setValidator(dValidator);
	intervalEnd->setValidator(dValidator);
    
	registerField("interpolF.function*", funcLineEdit);
    registerField("interpolF.intervalBegin*", intervalBegin);
    registerField("interpolF.intervalEnd*", intervalEnd);

    QHBoxLayout* hFuncLayout = new QHBoxLayout;
    hFuncLayout->addWidget(funcLabel);
    hFuncLayout->addWidget(funcLineEdit);
    QHBoxLayout* hInterLayout = new QHBoxLayout;
    hInterLayout->addWidget(intervalLabel);
    hInterLayout->addWidget(intervalBegin);
    hInterLayout->addWidget(intervalEnd);
	hInterLayout->addStretch();
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(hFuncLayout);
    layout->addLayout(hInterLayout);
    setLayout(layout);
}

int InterpolFuncDataPage::nextId() const
{
    return NuMethWizard::Page_Conclusion;
}

InterpolFuncPlainDataPage::InterpolFuncPlainDataPage(QWidget* parent)
        : QWizardPage(parent)
{
    setTitle(tr("Function and plain data interpolation session."));
    setSubTitle(tr("Please enter a valid function with an interval and "
                   "a file name with plain interpolation Xs for "
                   "<i>NuMeth</i>&trade; interpolation session."));

    funcLabel = new QLabel(tr("&Function:"));
    funcLineEdit = new QLineEdit;
    funcLabel->setBuddy(funcLineEdit);
    intervalLabel = new QLabel(tr("Interval:"));
    intervalBegin = new QLineEdit;
    intervalEnd = new QLineEdit;
	QDoubleValidator* dValidator = new QDoubleValidator(-15000, 15000, 10, this);
	intervalBegin->setValidator(dValidator);
	intervalEnd->setValidator(dValidator);
    
	registerField("interpolFP.function*", funcLineEdit);
    registerField("interpolFP.intervalBegin*", intervalBegin);
    registerField("interpolFP.intervalEnd*", intervalEnd);

    QHBoxLayout* hFuncLayout = new QHBoxLayout;
    hFuncLayout->addWidget(funcLabel);
    hFuncLayout->addWidget(funcLineEdit);
    QHBoxLayout* hInterLayout = new QHBoxLayout;
    hInterLayout->addWidget(intervalLabel);
    hInterLayout->addWidget(intervalBegin);
    hInterLayout->addWidget(intervalEnd);
	hInterLayout->addStretch();
    QVBoxLayout* fLayout = new QVBoxLayout;
    fLayout->addLayout(hFuncLayout);
    fLayout->addLayout(hInterLayout);

    fileLabel = new QLabel(tr("&File:"));
    fileLineEdit = new QLineEdit;
    fileLabel->setBuddy(fileLineEdit);
    browseButton = new QPushButton(tr("&Browse..."));
    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
    registerField("interpolFP.plainFileName*", fileLineEdit);
    QHBoxLayout* pLayout = new QHBoxLayout;
    pLayout->addWidget(fileLabel);
    pLayout->addWidget(fileLineEdit);
    pLayout->addWidget(browseButton);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(fLayout);
    layout->addLayout(pLayout);

    setLayout(layout);
}

void InterpolFuncPlainDataPage::browse()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                    ".", tr("Text files(*.txt)"));
    if (!fileName.isEmpty()) {
        fileLineEdit->setText(QFileInfo(fileName).absoluteFilePath());
    }
}

int InterpolFuncPlainDataPage::nextId() const
{
    return NuMethWizard::Page_Conclusion;
}

ConclusionPage::ConclusionPage(QWidget* parent)
        : QWizardPage(parent)
{
    setTitle(tr("Complete the wizard"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.jpg"));

    conclusionLabel = new QLabel;
    conclusionLabel->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(conclusionLabel);
    setLayout(layout);
}

int ConclusionPage::nextId() const
{
    return -1;
}

void ConclusionPage::initializePage()
{
    QString text = tr("The wizard completed.\n");
    if (wizard()->hasVisitedPage(NuMethWizard::Page_Integ)) {
        text += tr("The integration session was made.");
    }
    else if (wizard()->hasVisitedPage(NuMethWizard::Page_InterPlainData)) {
        text += tr("The interpolation session with plain data was made.");
    }
    else if (wizard()->hasVisitedPage(NuMethWizard::Page_InterFuncData)) {
        text += tr("The interpolation session with function was made.");
    }
    else {
        text += tr("The interpolation with both plain data and function was made.");
    }

    conclusionLabel->setText(text);
}
