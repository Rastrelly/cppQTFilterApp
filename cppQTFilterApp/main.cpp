#include "cppQTFilterApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    cppQTFilterApp w;
    w.show();
    return a.exec();
}
