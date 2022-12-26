#include "SecurityLab.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	SecurityLabModel model;
	SecurityLab window(&model);
	window.show();
	return a.exec();
}
