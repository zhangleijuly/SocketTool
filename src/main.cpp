#include <QApplication>

#include "mymainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load("../i18n/SocketTool_zh_CN.qm");
    a.installTranslator(&translator);
    MyMainWindow w;
    w.showMaximized();
    return a.exec();
}
