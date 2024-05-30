#include "QtWidgetsApplication6.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtWidgetsApplication6 w;
    w.show();
    return a.exec();
}
