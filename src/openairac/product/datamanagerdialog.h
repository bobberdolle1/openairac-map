/*****************************************************************************
* OpenAIRAC Map — Navigation Data & Cache Manager Dialog
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

#ifndef OPENAIRAC_DATAMANAGERDIALOG_H
#define OPENAIRAC_DATAMANAGERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTreeWidget>

namespace openairac {

class DataManagerDialog : public QDialog {
    Q_OBJECT

public:
    explicit DataManagerDialog(QWidget *parent = nullptr);
    virtual ~DataManagerDialog() override = default;

public slots:
    void onCheckUpdatesClicked();
    void onInstallUpdateClicked();
    void onRollbackClicked();
    void onClearChartCacheClicked();
    void onClearWeatherCacheClicked();

private:
    QLabel *m_installedAiracLabel = nullptr;
    QLabel *m_availableAiracLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QPushButton *m_checkUpdatesBtn = nullptr;
    QPushButton *m_installUpdateBtn = nullptr;
    QPushButton *m_rollbackBtn = nullptr;
    QProgressBar *m_progressBar = nullptr;

    QLabel *m_chartCacheLabel = nullptr;
    QLabel *m_weatherCacheLabel = nullptr;
    QPushButton *m_clearChartCacheBtn = nullptr;
    QPushButton *m_clearWeatherCacheBtn = nullptr;

    void refreshStatus();
};

} // namespace openairac

#endif // OPENAIRAC_DATAMANAGERDIALOG_H
