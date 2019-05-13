#ifndef XLSXSETTINGS_H
#define XLSXSETTINGS_H
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QTextStream>

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

class XlsxSettings
{
    friend void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    public:
        static QVariant value(const QString key, const QVariant& default_value = QVariant())
        {

            return XlsxSettings::m_singleton->value(key, default_value);
        }
        static void setValue(const QString& key, const QVariant& value) { XlsxSettings::m_singleton->setValue(key, value); }
        static void init();
        static void destroy();
        static void setLogFile();
        static void setOrganization();
    private:
        explicit XlsxSettings();
        static QSettings *m_singleton;
        static QFile m_log_file;
        static QTextStream m_text_stream;
};

#endif // XLSXSETTINGS_H
