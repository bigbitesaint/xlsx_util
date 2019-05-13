#include "xlsxsettings.h"
#include <QDebug>
#include <QApplication>
#include "xlsxcomparerglobal.h"

QFile XlsxSettings::m_log_file;
QTextStream XlsxSettings::m_text_stream;
QSettings* XlsxSettings::m_singleton = nullptr;

/* redirect message */
void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context);

    const QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            XlsxSettings::m_text_stream << "Debug: " << msg << "\r\n";
            break;
        case QtInfoMsg:
            XlsxSettings::m_text_stream << "Info:" << msg << "\r\n";
            break;
        case QtWarningMsg:
            XlsxSettings::m_text_stream << "Warning:" << msg << "\r\n";
            break;
        case QtCriticalMsg:
            XlsxSettings::m_text_stream << "Critical:" << msg << "\r\n";
            break;
        case QtFatalMsg:
            XlsxSettings::m_text_stream << "Fatal:" << msg << "\r\n";
            abort();
    }
}


XlsxSettings::XlsxSettings()
{

}



void XlsxSettings::init()
{
    setOrganization();

    /*  QSettings has to be constructed after setOrganization()   */
    XlsxSettings::m_singleton = new QSettings;
    setLogFile();

}


void XlsxSettings::destroy()
{
    if (XlsxSettings::m_log_file.isOpen())
    {
        XlsxSettings::m_log_file.close();
    }

    delete XlsxSettings::m_singleton;
}

/*  set organization and etc for QSettings   */
void XlsxSettings::setOrganization()
{
    QCoreApplication::setOrganizationName("Mediatek");
    QCoreApplication::setApplicationName("XlsxComparer");
}


void XlsxSettings::setLogFile()
{
    QSettings setting;


    QVariant log_file_path = XlsxSettings::value(SETTING_LOG_FILE_STRING, DEFAULT_LOG_FILE);


    if (XlsxSettings::m_log_file.isOpen())
    {
        XlsxSettings::m_log_file.close();
    }

    XlsxSettings::m_log_file.setFileName(log_file_path.toString());


    if (!XlsxSettings::m_log_file.open(QIODevice::WriteOnly))
    {
        XlsxSettings::m_log_file.open(stderr, QIODevice::WriteOnly);
        qWarning() << "Cannot open log file.";
    }

    XlsxSettings::m_text_stream.setDevice(&XlsxSettings::m_log_file);

    qInstallMessageHandler(messageHandler);
}
