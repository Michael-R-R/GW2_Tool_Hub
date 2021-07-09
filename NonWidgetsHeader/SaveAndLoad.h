#ifndef SAVEANDLOAD_H
#define SAVEANDLOAD_H

#include <NonWidgetsHeader/ErrorHandling.h>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QString>

class SaveAndLoad : QObject
{
public:
    SaveAndLoad();
    ~SaveAndLoad();

    QString GetFileName() const { return fileName; }
    void SetFileName(QString val) { fileName = val; }

    // --- Save/Load all tracked materials ---
    QByteArray OpenTrackedMaterialsFile(QWidget* parent);
    int SaveTrackedMaterials(QWidget* parent, int tabAmt,
                                   const QVector<QString>& tabNames,
                                   const QVector<int>& amtOfMatsInTab,
                                   const QVector<QVector<QString>>& namesInTab,
                                   const QVector<QVector<int>>& currentAmtsInTab,
                                   const QVector<QVector<int>>& goalAmtsInTab);
    int SaveAsTrackedMaterials(QWidget* parent, int tabAmt,
                               const QVector<QString>& tabNames,
                               const QVector<int>& amtOfMatsInTab,
                               const QVector<QVector<QString>>& namesInTab,
                               const QVector<QVector<int>>& currentAmtsInTab,
                               const QVector<QVector<int>>& goalAmtsInTab);


    // --- Save/Load notes ---
    QString OpenNotes(QWidget* parent);
    QString SaveNotes(QWidget* parent, QString fileContent);
    QString SaveAsNotes(QWidget* parent, QString fileContent);

    // --- Import excel sheet ---
    QByteArray LoadExcelSheet(QWidget* parent);

private:
    QString fileName;

    // --- Error Handling ---
    ErrorHandling error;

    // --- Notes ---
    QString notesFileDir;

    // --- Material Tracker
    QString matTrackFileDir;

private:
    void NonModuleMessage(QWidget* parent, QString title, QString text);
};

#endif // SAVEANDLOAD_H
