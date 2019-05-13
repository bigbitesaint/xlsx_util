#include "xlsxcomparergui.h"
#include "xlsxsettings.h"
#include <QApplication>
#include <QtGlobal>
#include <QDebug>







int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XLSX_COMPARE::XlsxComparerGui w;
    w.show();


    return a.exec();
}
