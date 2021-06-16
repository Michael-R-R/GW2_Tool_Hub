#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <QString>
#include <QUrl>
#include <QMessageBox>
#include <QAbstractButton>
#include <QDebug>

class ErrorHandling : public QObject
{
public:
    ErrorHandling();
    ~ErrorHandling();

    // --- Popup Message Boxes ---
    void NonModalErrorMessage(QWidget* parent, QString title, QString text);

    // --- Material Tracker ---
    bool CheckForValidTabName(QWidget* parent, QString name, const QVector<QString>& activeNames);
    bool CheckForValidImportSheet(QWidget* parent, QString sheetName);

    // --- Browser ---
    bool CheckForValidPage(QWidget* parent, const QUrl& url);

private:

    // --- Detailed Message Boxes ---
    void HyperlinkErrorMessage(QWidget* parent, QString title, QString text);
};

#endif // ERRORHANDLING_H
