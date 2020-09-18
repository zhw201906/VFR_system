#include "VFR_algorithm.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VFR_algorithm w;
    w.show();
    return a.exec();
}
