#include "xlsxcomparergui.h"
#include "xlsxsettings.h"
#include "xlsxcomparer.h"
#include "inheritfiledialog.h"
#include "preferencedialog.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QTreeView>
#include <QDialog>
#include <QHBoxLayout>
#include <QStandardItem>

using namespace XLSX_COMPARE;

XlsxComparerGui::XlsxComparerGui(QWidget *parent)
    : QMainWindow(parent)
{
    XlsxSettings::init();
    createGui();    
}


XlsxComparerGui::~XlsxComparerGui()
{
    XlsxSettings::destroy();
}

void XlsxComparerGui::createGui()
{
    /*  File Menu
     *  -Inherit
     *  -Compare
     *
     *
     */
    QMenu *file_menu = menuBar()->addMenu(tr("File"));
    QAction *file_inherit_action = file_menu->addAction(tr("Inherit"));


    connect(file_inherit_action, &QAction::triggered, this, [this]()
    {
        InheritFileDialog *fdialog = new InheritFileDialog(this);
        connect(fdialog, SIGNAL(error(const QString)), this, SLOT(errorMsgHandler(const QString)));
        connect(fdialog, SIGNAL(info(const QString)), this, SLOT(infoMsgHandler(const QString)));
        //fdialog.exec();

        QDockWidget* dock_widget = new QDockWidget(this);
        dock_widget->setWidget(fdialog);

        /*  when fdialog is closed, close dock widget as well   */
        fdialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(fdialog, &QObject::destroyed, dock_widget, &QObject::deleteLater);

        this->addDockWidget(Qt::LeftDockWidgetArea, dock_widget);
    });

    QAction *file_compare_action = file_menu->addAction(tr("Compare"));
    connect(file_compare_action, &QAction::triggered, this, [this]()
    {
        InheritFileDialog *fdialog = new InheritFileDialog(this);
        connect(fdialog, SIGNAL(error(const QString)), this, SLOT(errorMsgHandler(const QString)));
        connect(fdialog, SIGNAL(info(const QString)), this, SLOT(infoMsgHandler(const QString)));
        //fdialog.exec();

        QDockWidget* dock_widget = new QDockWidget(this);
        dock_widget->setWidget(fdialog);

        /*  when fdialog is closed, close dock widget as well   */
        fdialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(fdialog, &QObject::destroyed, dock_widget, &QObject::deleteLater);

        this->addDockWidget(Qt::LeftDockWidgetArea, dock_widget);
    });

    /*  Option Menu
     *  -Preference
     *
     *
     */
    QMenu *option_menu = menuBar()->addMenu(tr("Option"));
    QAction *option_preference_action = option_menu->addAction(tr("Preference"));

    connect(option_preference_action, &QAction::triggered, this, [this]()
    {
        PreferenceDialog *pref_dialog = new PreferenceDialog(this);
        connect(pref_dialog, SIGNAL(error(const QString)), this, SLOT(errorMsgHandler(const QString)));
        connect(pref_dialog, SIGNAL(info(const QString)), this, SLOT(infoMsgHandler(const QString)));

        QDialog* dialog = new QDialog(this);
        dialog->setLayout(new QHBoxLayout);
        dialog->layout()->addWidget(pref_dialog);

        /*  when fdialog is closed, close dock widget as well   */
        pref_dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(pref_dialog, &QObject::destroyed, dialog, &QObject::deleteLater);


        dialog->exec();
    });



}



void XlsxComparerGui::errorMsgHandler(const QString text) const
{
    QMessageBox::critical(this->centralWidget(), tr("Error"), text);
}


void XlsxComparerGui::infoMsgHandler(const QString text) const
{
    QMessageBox::information(this->centralWidget(), tr("Information"), text);
}
