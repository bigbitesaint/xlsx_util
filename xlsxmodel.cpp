#include "xlsxmodel.h"
#include "xlsxdocument.h"

using namespace XLSX_COMPARE;

XlsxModel::XlsxModel(QXlsx::Document* document, QObject *parent)
    : QAbstractItemModel (parent),
      m_document(document)
{
    if (m_document != nullptr)
        init_data();
}


QVariant XlsxModel::data(const QModelIndex& index, int role) const
{

}

int XlsxModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        return m_document->sheetNames().size();
    }
    else
    {

    }
}

int XlsxModel::columnCount(const QModelIndex& parent) const
{

}

QModelIndex XlsxModel::index(int row, int column, const QModelIndex& parent) const
{

}
