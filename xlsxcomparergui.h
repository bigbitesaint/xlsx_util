#ifndef XLSXCOMPARERGUI_H
#define XLSXCOMPARERGUI_H

#include <QMainWindow>

namespace XLSX_COMPARE
{

class XlsxComparerGui : public QMainWindow
{
    Q_OBJECT
    public:
        explicit XlsxComparerGui(QWidget *parent = nullptr);
        void createGui();
        ~XlsxComparerGui();
    signals:

    private slots:
        void errorMsgHandler(const QString) const;
        void infoMsgHandler(const QString) const;



};

}
#endif // XLSXCOMPARERGUI_H
