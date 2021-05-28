#ifndef SAVEANDLOAD_H
#define SAVEANDLOAD_H

#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
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
                                    QVector<QString> tabNames,
                                    QVector<int> amtOfMatsInTab,
                                    QVector<QVector<QString>> namesInTab,
                                    QVector<QVector<int>> currentAmtsInTab,
                                    QVector<QVector<int>> goalAmtsInTab);


    // Save/Load notes
    QString LoadNotes(QWidget* parent);
    void SaveNotes(QWidget* parent, QString fileContent);

    // Import excel sheet
    QString LoadExcelSheet(QWidget* parent);

private:

    QString fileName;

};

#endif // SAVEANDLOAD_H
