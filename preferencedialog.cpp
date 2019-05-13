#include "xlsxcomparerglobal.h"
#include "preferencedialog.h"
#include "xlsxsettings.h"
#include <QFrame>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSettings>


PreferenceDialog::PreferenceDialog(QWidget* parent)
    : QWidget(parent),
      m_log_file(new QLineEdit(this))
{
    QSettings setting;
    QVariant log_file = setting.value(SETTING_LOG_FILE_STRING, DEFAULT_LOG_FILE);
    m_log_file->setText(log_file.toString());
    createGui();
}


void PreferenceDialog::createGui()
{
    QVBoxLayout *main_layout = new QVBoxLayout;


    QFrame *top_frame = new QFrame(this);
    top_frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    QGridLayout *top_grid_layout = new QGridLayout;
    top_grid_layout->addWidget(new QLabel(tr("Error Log"), this), 0, 0);
    top_grid_layout->addWidget(m_log_file, 0, 1);
    top_frame->setLayout(top_grid_layout);
    top_frame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    main_layout->addWidget(new QLabel(tr("Log")));
    main_layout->addWidget(top_frame);




    QWidget* button_group = new QWidget(this);
    QHBoxLayout* button_layout = new QHBoxLayout;
    QPushButton *ok_button = new QPushButton(tr("Ok"), this);
    QPushButton *apply_button = new QPushButton(tr("Apply"), this);

    connect(ok_button, &QPushButton::clicked, this, [this]()
    {
        if (this->saveOption())
        {
            XlsxSettings::setLogFile();
            this->deleteLater();
            this->close();
        }
    });
    connect(apply_button, &QPushButton::clicked, this, &PreferenceDialog::saveOption);


    button_layout->addWidget(ok_button, 0, Qt::AlignBottom);
    button_layout->addWidget(apply_button, 0, Qt::AlignBottom);
    button_group->setLayout(button_layout);
    button_group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    main_layout->addWidget(button_group);



    setLayout(main_layout);
}





bool PreferenceDialog::saveOption()
{
    if (m_log_file->text().size() == 0)
    {
        emit error(tr("Log file cannot be empty."));
        return false;
    }

    QSettings setting;
    setting.setValue(SETTING_LOG_FILE_STRING, m_log_file->text());


    return true;
}
