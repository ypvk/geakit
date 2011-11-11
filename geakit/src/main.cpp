#include <QApplication>

#include "gui/gmainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	GMainWindow w;
	w.show();
	return app.exec();
}
