#ifndef XLSXCOMPARER_H
#define XLSXCOMPARER_H

#include <QMainWindow>
#include <QMap>
#include <QStandardItemModel>
#include "xlsxdocument.h"


namespace XLSX_COMPARE{

class SheetTag
{
    public:
        SheetTag() : m_tag(""), m_bank(0) {}

        SheetTag(const QString& tag, const quint32& bank)
        :   m_tag(tag.toUpper())
        {
            m_bank = (bank <0x100) ? bank +0x1300 : bank;
        }


        SheetTag& operator=(const SheetTag& other)
        {
            m_tag = other.m_tag;
            m_bank = other.m_bank;
            m_filename = other.m_filename;
            return *this;
        }


        friend inline bool operator < (const SheetTag& here, const SheetTag& other)
        {
            if (here.m_tag == other.m_tag)
            {
                return false;
            }

            return (here.m_tag < other.m_tag);
        }


        bool isValid() const {return (m_tag.size() !=0); }

    private:
        QString m_tag;
        QString m_filename;
        quint32 m_bank;
};



enum Visibility
{
    WHITE,
    YELLOW,
    RED
};


enum Status
{
    NEW,
    INHERITED,
    AUGMENTED
};


typedef QMap<QString, Visibility> RegMap;


class XlsxComparer : public QObject
{
    Q_OBJECT
    public:
        XlsxComparer(QObject* parent = nullptr);
        void dump() const;
        void setResultModel(QStandardItemModel* model) { m_record_model = model; }

        ~XlsxComparer();

    public slots:
        void load(const QString& src);
        void inherit(const QString& src);

    signals:
        bool finished(bool);

    private:
        QMap<SheetTag, RegMap*> m_map;
        QStandardItemModel *m_record_model;

        SheetTag parseSheetTag(const QString& title) const;
        int findHeaderRow(QXlsx::Document& wb) const;
        QMap<QString, int> findColumnOfInterest(QXlsx::Document& wb, const int header_row, const QStringList& coi) const;
};
}
#endif // XLSXCOMPARER_H
