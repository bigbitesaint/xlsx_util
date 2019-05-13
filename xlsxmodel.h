#ifndef XLSXMODEL_H
#define XLSXMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QString>
#include <QVariantList>


namespace QXlsx
{
    class Document;
}


namespace XLSX_COMPARE
{

    struct ModelEntry
    {
        QString sheet;
        QList<QVariantList> data;
    };

    class XlsxModel : public QAbstractItemModel
    {
        Q_OBJECT
        public:
            explicit XlsxModel(QXlsx::Document* document, QObject *parent);
            QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
            int rowCount(const QModelIndex& parent=QModelIndex()) const override;
            int columnCount(const QModelIndex& parent=QModelIndex()) const override;
            QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const override;

        private:
            void init_data();

            QXlsx::Document*    m_document;
            QList<ModelEntry>   m_data;

    };
}
#endif // XLSXMODEL_H
