#ifndef INHERITFILEDIALOG_H
#define INHERITFILEDIALOG_H

#include <QLineEdit>
#include <QDialog>
#include <QDockWidget>
#include <QList>
#include <QStandardItemModel>
#include <QThread>

namespace XLSX_COMPARE {
class FilepathEdit;

class InheritFileDialog : public QWidget
{
    Q_OBJECT
    public:
        InheritFileDialog(QWidget *parent = nullptr);
        ~InheritFileDialog() {m_thread.quit(); m_thread.wait();}

    private:
        void createGui();
        void setResult(QStandardItemModel* result);


        QList<FilepathEdit*> m_reffile_list;
        QStandardItemModel *m_result_model;
        QThread m_thread;

    signals:
        void error(const QString);
        void info(const QString);
};
}

#endif // INHERITFILEDIALOG_H
