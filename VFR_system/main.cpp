#include "VFR_system.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VFR_system w;
	w.show();
	return a.exec();
}
