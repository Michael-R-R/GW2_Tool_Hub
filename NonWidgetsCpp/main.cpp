#include <QApplication>

#include "WidgetsHeader/MainWindow/MainWindow.h"

// TEST Push

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
