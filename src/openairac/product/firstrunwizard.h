/*****************************************************************************
* OpenAIRAC Map — First-Run Onboarding Setup Wizard
*
* Copyright 2026 OpenAIRAC Contributors
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*****************************************************************************/

#ifndef OPENAIRAC_FIRSTRUNWIZARD_H
#define OPENAIRAC_FIRSTRUNWIZARD_H

#include <QWizard>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>

namespace openairac {

class IntroPage : public QWizardPage {
    Q_OBJECT
public:
    explicit IntroPage(QWidget *parent = nullptr);
};

class SimulatorDetectionPage : public QWizardPage {
    Q_OBJECT
public:
    explicit SimulatorDetectionPage(QWidget *parent = nullptr);
    virtual void initializePage() override;

    QString detectedSimulator() const;
    QString simulatorPath() const;

private:
    QLabel *m_detectionStatusLabel = nullptr;
    QLineEdit *m_pathEdit = nullptr;
    QPushButton *m_browseBtn = nullptr;
    QCheckBox *m_standaloneModeBox = nullptr;

    void autoDetectSimulators();
};

class NavdataSetupPage : public QWizardPage {
    Q_OBJECT
public:
    explicit NavdataSetupPage(QWidget *parent = nullptr);

    QString selectedBundle() const;

private slots:
    void onInstallDataClicked();

private:
    QRadioButton *m_worldOpenRadio = nullptr;
    QRadioButton *m_usRadio = nullptr;
    QRadioButton *m_europeRadio = nullptr;
    QPushButton *m_installBtn = nullptr;
    QProgressBar *m_progressBar = nullptr;
    QLabel *m_statusLabel = nullptr;
};

class OptionalProvidersPage : public QWizardPage {
    Q_OBJECT
public:
    explicit OptionalProvidersPage(QWidget *parent = nullptr);
    virtual void initializePage() override;

private:
    QLabel *m_navigraphStatusLabel = nullptr;
    QCheckBox *m_enableNavigraphBox = nullptr;
};

class ChartsSetupPage : public QWizardPage {
    Q_OBJECT
public:
    explicit ChartsSetupPage(QWidget *parent = nullptr);
};

class LiveFeedsPage : public QWizardPage {
    Q_OBJECT
public:
    explicit LiveFeedsPage(QWidget *parent = nullptr);
};

class FinishPage : public QWizardPage {
    Q_OBJECT
public:
    explicit FinishPage(QWidget *parent = nullptr);
    virtual void initializePage() override;

private:
    QLabel *m_summaryLabel = nullptr;
};

class FirstRunWizard : public QWizard {
    Q_OBJECT

public:
    enum PageId {
        Page_Intro,
        Page_SimulatorDetection,
        Page_NavdataSetup,
        Page_OptionalProviders,
        Page_ChartsSetup,
        Page_LiveFeeds,
        Page_Finish
    };

    explicit FirstRunWizard(QWidget *parent = nullptr);
    virtual ~FirstRunWizard() override = default;

    static bool shouldRunWizard();
    static void markWizardCompleted();
};

} // namespace openairac

#endif // OPENAIRAC_FIRSTRUNWIZARD_H
