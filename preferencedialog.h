#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QDebug>

class PreferenceDialog : public QWidget
{
    Q_OBJECT
    public:
        explicit PreferenceDialog(QWidget* parent=nullptr);
        ~PreferenceDialog() { qDebug() << Q_FUNC_INFO; }

    signals:
        void error(const QString);
        void info(const QString);


    private slots:
        bool saveOption();


    private:
        void createGui();
        QLineEdit *m_log_file;


};

#endif // PREFERENCEDIALOG_H
