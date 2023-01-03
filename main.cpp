#include "MainWindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QTextStream out(stdout);
    qSetMessagePattern("[%{time hh:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{file}:%{line}%{endif}%{if-critical}C%{file}:%{line}%{endif}%{if-fatal}F%{file}:%{line}%{endif}] -  %{message}");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
