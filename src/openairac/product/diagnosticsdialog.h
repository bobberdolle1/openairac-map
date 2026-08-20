/*****************************************************************************
* OpenAIRAC Map — System Diagnostics Dialog
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

#ifndef OPENAIRAC_DIAGNOSTICSDIALOG_H
#define OPENAIRAC_DIAGNOSTICSDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>

namespace openairac {

class DiagnosticsDialog : public QDialog {
    Q_OBJECT

public:
    explicit DiagnosticsDialog(QWidget *parent = nullptr);
    virtual ~DiagnosticsDialog() override = default;

public slots:
    void onRunDiagnostics();
    void onCopyReport();
    void onOpenLogFolder();

private:
    QTextBrowser *m_reportBrowser = nullptr;
    QPushButton *m_runBtn = nullptr;
    QPushButton *m_copyBtn = nullptr;
    QPushButton *m_logBtn = nullptr;

    QString generateSanitizedReport() const;
};

} // namespace openairac

#endif // OPENAIRAC_DIAGNOSTICSDIALOG_H
