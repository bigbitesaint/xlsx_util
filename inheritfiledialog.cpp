#include "xlsxcomparerglobal.h"
#include "inheritfiledialog.h"
#include "filepathedit.h"
#include "xlsxcomparer.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTreeView>
#include <QSizePolicy>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QMessageBox>
#include <QFrame>
#include <QEventLoop>
#include <QMetaObject>
#include <QDebug>

using namespace XLSX_COMPARE;

InheritFileDialog::InheritFileDialog(QWidget* parent)
    : QWidget(parent),
      m_result_model(nullptr)
{
    createGui();

}


void InheritFileDialog::createGui()
{
    QVBoxLayout *main_layout = new QVBoxLayout;


    //=================================================
    //  Begin of Top Frame
    //=================================================
    QFrame* top_frame = new QFrame;
    top_frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    top_frame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    FilepathEdit* srcfile_edit = new FilepathEdit(SETTING_LAST_SRCFILE_STRING, this);
    QVBoxLayout* top_frame_inner_layout = new QVBoxLayout;
    top_frame_inner_layout->addWidget(new QLabel(tr("Please select an xlsx file to be modified:"), this));
    top_frame_inner_layout->addWidget(srcfile_edit);
    top_frame->setLayout(top_frame_inner_layout);

    main_layout->addWidget(top_frame);


    //=================================================
    //  Begin of Middle Frame
    //=================================================
    QFrame* middle_frame = new QFrame;
    middle_frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    middle_frame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);


    FilepathEdit *reffile_edit= new FilepathEdit(SETTING_LAST_REFFILE_STRING, this);
    m_reffile_list.append(reffile_edit);


    QVBoxLayout* middle_frame_inner_layout = new QVBoxLayout;
    QVBoxLayout* middle_frame_expandable_layout = new QVBoxLayout;
    middle_frame_expandable_layout->setMargin(0);
    middle_frame_expandable_layout->setContentsMargins(0, 0, 0, 0);
    middle_frame_expandable_layout->addWidget(new QLabel(tr("Please select reference xlsx file(s):"), this));


    middle_frame_inner_layout->addLayout(middle_frame_expandable_layout);
    middle_frame_inner_layout->addWidget(reffile_edit);

    QPushButton *create_edit_button = new QPushButton(tr("Add"), this);
    create_edit_button->setFixedSize(40, 20);
    middle_frame_inner_layout->addWidget(create_edit_button);
    connect(create_edit_button, &QPushButton::clicked, this,
    [this, middle_frame_expandable_layout](){
        FilepathEdit *tmp_edit = new FilepathEdit(SETTING_LAST_REFFILE_STRING, this);
        m_reffile_list.append(tmp_edit);
        middle_frame_expandable_layout->addWidget(tmp_edit);
    });

    middle_frame->setLayout(middle_frame_inner_layout);


    main_layout->addWidget(middle_frame);

    //=================================================
    //  Begin of Bottom layout
    //=================================================
    QWidget *bottom_widget = new QWidget(this);
    bottom_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    QHBoxLayout *bottom_layout = new QHBoxLayout;




    QPushButton *close_button = new QPushButton(tr("Cancel"), this);
    QPushButton *ok_button = new QPushButton(tr("Start"), this);
    QPushButton *show_button = new QPushButton(tr("Show Result"), this);
    show_button->setDisabled(true);

    connect(close_button, SIGNAL(clicked()), this, SLOT(close()));
    connect(ok_button, &QPushButton::clicked, this,
    [this, srcfile_edit, show_button]()
    {
        /* allocate QStandardItemModel to store result */
        m_result_model = new QStandardItemModel(this);

        /*  create XlsxComparer and attach it to QThread   */
        XlsxComparer *comparer = new XlsxComparer;
        comparer->setResultModel(m_result_model);
        comparer->moveToThread(&m_thread);
        m_thread.start();

        for (const auto& p : this->m_reffile_list)
        {
            if (p->text().length() == 0)
            {
                continue;
            }

            QMessageBox status;
            status.setText(tr("Processing ") + p->text());
            status.setWindowTitle(tr("Please wait..."));
            status.setStandardButtons(nullptr);
            status.show();


            QEventLoop q;
            connect(comparer, &XlsxComparer::finished, &status, &QDialog::close);
            connect(comparer, &XlsxComparer::finished, &q, &QEventLoop::exit);
            connect(comparer, &XlsxComparer::finished, this, [this, p](bool ok){
                if (!ok)
                {
                    const QString msg = tr("Cannot load ") + p->text();
                    qCritical() << msg;
                    emit error(msg);
                }
            });

            QMetaObject::invokeMethod(comparer, "load", Qt::QueuedConnection, QGenericReturnArgument(nullptr), Q_ARG(const QString&, p->text()));
            q.exec();
        }

        QMessageBox status;
        status.setText(tr("Inheriting ") + srcfile_edit->text());
        status.setWindowTitle(tr("Please wait..."));
        status.setStandardButtons(nullptr);
        status.show();

        QEventLoop q;
        connect(comparer, &XlsxComparer::finished, &status, &QDialog::close);
        connect(comparer, &XlsxComparer::finished, &q, &QEventLoop::exit);
        connect(comparer, &XlsxComparer::finished, comparer, &QObject::deleteLater);
        connect(comparer, &XlsxComparer::finished, this, [this, srcfile_edit, show_button](bool ok){
            if (!ok)
            {
                const QString msg = tr("Cannot inherit ") + srcfile_edit->text();
                qCritical() << msg;
                emit error(msg);
            }
            else
            {
                show_button->setDisabled(false);
            }
        });


        QMetaObject::invokeMethod(comparer, "inherit", Qt::QueuedConnection, QGenericReturnArgument(nullptr), Q_ARG(const QString&, srcfile_edit->text()));
        q.exec();



        emit info(tr("Completed!"));



    });


    connect(show_button, &QPushButton::clicked, this, [this](){
        QDialog *result_dialog = new QDialog(this, Qt::WindowTitleHint);
        QVBoxLayout *layout = new QVBoxLayout;

        QTreeView *view = new QTreeView(result_dialog);
        view->setModel(m_result_model);
        m_result_model->setHeaderData(0, Qt::Horizontal, tr("Sheet Name"), Qt::DisplayRole);

        QPushButton *btn = new QPushButton(tr("Ok"), result_dialog);
        connect(btn, &QPushButton::clicked, result_dialog, &QDialog::close);


        QPushButton *save = new QPushButton(tr("Save"), result_dialog);
        connect(save, &QPushButton::clicked, this, [this]()
        {
            QStandardItemModel *model = this->m_result_model;

            QFile f("result.txt");
            if (!f.open(QIODevice::Text | QIODevice::WriteOnly))
            {
                emit error(tr("Cannot open file:") + f.fileName());
                return;
            }

            QTextStream fout(&f);

            for (int p_idx=0; p_idx < model->rowCount(); ++p_idx)
            {
                QModelIndex idx = model->index(p_idx, 0);

                fout << idx.data().toString() << "\n";
                for (int c_idx=0; c_idx < model->rowCount(idx); ++c_idx)
                {
                    fout << "-" << m_result_model->index(c_idx, 0, idx).data().toString() << "\n";
                }
            }

            emit info(tr("File saved!"));
        });

        QHBoxLayout *btn_layout = new QHBoxLayout;
        btn_layout->addWidget(btn);
        btn_layout->addWidget(save);

        layout->addWidget(view);
        layout->addLayout(btn_layout);

        result_dialog->setLayout(layout);

        result_dialog->exec();
    });



    bottom_layout->addWidget(ok_button, 0, Qt::AlignBottom);
    bottom_layout->addWidget(close_button, 0, Qt::AlignBottom);
    bottom_layout->addWidget(show_button, 0, Qt::AlignBottom);
    bottom_widget->setLayout(bottom_layout);


    main_layout->addWidget(bottom_widget);

    setLayout(main_layout);
}




void InheritFileDialog::setResult(QStandardItemModel *result)
{
    m_result_model = result;
}
