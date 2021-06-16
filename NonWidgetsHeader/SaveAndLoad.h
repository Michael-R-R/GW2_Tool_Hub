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

    // Save/Load all tracked materials
    QByteArray LoadTrackedMaterialsFromFile(QWidget* parent);
    int SaveTrackedMaterialsToFile(QWidget* parent, int tabAmt,
                                   const QVector<QString>& tabNames,
                                   const QVector<int>& amtOfMatsInTab,
                                   const QVector<QVector<QString>>& namesInTab,
                                   const QVector<QVector<int>>& currentAmtsInTab,
                                   const QVector<QVector<int>>& goalAmtsInTab);


    // Save/Load notes
    QString LoadNotes(QWidget* parent);
    void SaveNotes(QWidget* parent, QString fileContent);

    // Import excel sheet
    QByteArray LoadExcelSheet(QWidget* parent);

private:
    // --- Error Handling ---
    ErrorHandling error;

    QString fileName;

};

#endif // SAVEANDLOAD_H
