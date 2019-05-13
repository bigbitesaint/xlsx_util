#include "filepathedit.h"
#include "xlsxcomparerglobal.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>

using namespace XLSX_COMPARE;

FilepathEdit::FilepathEdit(const QString setting_id, QWidget *parent)
    :   QWidget(parent),
        m_edit("", this),
        m_button(tr("Select"), this)
{    

    QHBoxLayout *main_layout = new QHBoxLayout;
    main_layout->addWidget(&m_edit);
    main_layout->addWidget(&m_button);


    connect(&m_button, &QPushButton::clicked, this, [this, setting_id]()
    {
        QSettings setting;
        const QVariant last_srcfile_path = setting.value(setting_id, QVariant());
        const QString srcfile_path = (last_srcfile_path.isValid()) ? last_srcfile_path.toString() : ".";



        const QString path = QFileDialog::getOpenFileName(this, tr("Please select files(s)"), srcfile_path, "Excel 2013 (*.xlsx, *.XLSX)");
        if (path.length() == 0)
        {
            return;
        }


        setting.setValue(setting_id, path);
        m_edit.setText(path);
    }
    );


    setLayout(main_layout);
}
