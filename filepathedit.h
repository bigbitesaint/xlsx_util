#ifndef FILEPATHEDIT_H
#define FILEPATHEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

namespace XLSX_COMPARE
{
class FilepathEdit : public QWidget
{
    public:
        explicit FilepathEdit(const QString setting_id, QWidget* parent=nullptr);
        QString text() const { return m_edit.text();}

    protected:
        QSize minimumSizeHint() const override { return QSize(400, 40); }
        QSize sizeHint() const override{ return QSize(500, 40); }

    private:
        QLineEdit   m_edit;
        QPushButton m_button;
};
}
#endif // FILEPATHEDIT_H
