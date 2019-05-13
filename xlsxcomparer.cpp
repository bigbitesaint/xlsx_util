#include "xlsxcomparer.h"
#include <QRegularExpression>
#include <QDebug>
#include <QPair>
#include <QTreeView>

const QString LABEL_NAME = "Name";
const QString LABEL_VISIBILITY = "Visibility";
const QString LABEL_VISIBILITY_WHITE = "white";
const QString LABEL_VISIBILITY_YELLOW = "yellow";
const QString LABEL_VISIBILITY_RED = "red";

using namespace XLSX_COMPARE;

XlsxComparer::XlsxComparer(QObject* parent)
    : QObject(parent),
      m_record_model(nullptr)
{

}



void XlsxComparer::load(const QString& src)
{
    QXlsx::Document wb(src);


    /*  iterate through each sheet */
    const QStringList titles = wb.sheetNames();
    for (const QString& title : titles)
    {
        /*  parse sheet tag   */
        SheetTag tag = parseSheetTag(title);
        if (!tag.isValid())
        {
            continue;
        }


        if (!wb.selectSheet(title))
        {
            qCritical() << "Cannot select sheet " << title;
            emit finished(false);
            return;
        }

        /* find header row */
        int header_row = findHeaderRow(wb);

        if (header_row == -1)
        {
            qWarning() << "Cannot find header row for sheet " << title;
            continue;
        }

        /*  Now find column of Name and Visibility */
        QMap<QString, int> coi_map = findColumnOfInterest(wb, header_row, QStringList() <<LABEL_NAME << LABEL_VISIBILITY);
        int column_for_name = coi_map.contains(LABEL_NAME) ? coi_map[LABEL_NAME] : 0;
        int column_for_visibility = coi_map.contains(LABEL_VISIBILITY) ? coi_map[LABEL_VISIBILITY] :0;



        if (column_for_name == 0)
        {
            qCritical() << "Cannot find column " << LABEL_NAME << " in " << title;
            emit finished(false);
            return;
        }

        if (column_for_visibility == 0)
        {
            qCritical() << "Cannot find column " << LABEL_VISIBILITY << " in " << title;
            emit finished(false);
            return;
        }


        /*  if an ip bank already exists, reuse that pointer   */
        RegMap* reg_map = nullptr;

        if (m_map.contains(tag))
        {
            reg_map = m_map[tag];
        }
        else
        {
            reg_map = new RegMap;
        }



        for (int row_idx=header_row+1; row_idx <= wb.dimension().lastRow(); ++ row_idx)
        {
            const QVariant high_address = wb.read(row_idx, 1);

            /*  if first cell starts with //, ignore it */
            if (high_address.isValid() && high_address.toString().startsWith("//"))
            {
                continue;
            }


            /*  if regname is invalid or empty, skip */
            const QVariant reg_name_label = wb.read(row_idx, column_for_name);

            if (!reg_name_label.isValid() || reg_name_label.toString().length() == 0)
            {
                continue;
            }

            const QString reg_name = reg_name_label.toString().trimmed();


            /*  if visibility is invalid, set to WHITE */
            const QVariant visibility_label = wb.read(row_idx, column_for_visibility);
            const QString visibility = visibility_label.toString().trimmed();
            Visibility visibility_enum = WHITE;

            if (!visibility_label.isValid() || visibility.length() == 0)
            {
                qWarning() << QString().sprintf("(%d,%d) at %s contains invalid visibility.", row_idx, column_for_visibility, title.toStdString().c_str());
            }

            if ( !QString::compare(visibility, LABEL_VISIBILITY_RED, Qt::CaseInsensitive) )
            {
                visibility_enum = RED;
            }
            else if ( !QString::compare(visibility, LABEL_VISIBILITY_YELLOW, Qt::CaseInsensitive) )
            {
                visibility_enum = YELLOW;
            }
            else if ( !QString::compare(visibility, LABEL_VISIBILITY_WHITE, Qt::CaseInsensitive) )
            {
                visibility_enum = WHITE;
            }
            else
            {
                qWarning() << "Unknown visibility " << visibility << " at " << title << " row " << QString::number(row_idx);
                continue;
            }

            if (reg_map->contains(reg_name))
            {
                Visibility& v = (*reg_map)[reg_name];
                v = qMax<Visibility>(visibility_enum, v);
            }
            else
            {
                reg_map->insert( reg_name, visibility_enum );
            }

        }


        m_map.insert(tag, reg_map);
    }



    emit finished(true);
}



void XlsxComparer::inherit(const QString& tgt)
{
    QXlsx::Document wb(tgt);


    /*  insert categories   */
    QStandardItem* new_category_item = new QStandardItem(tr("New"));
    QStandardItem* unmodified_category_item = new QStandardItem(tr("Unmodified"));
    QStandardItem* total_inherited_category_item = new QStandardItem(tr("Totally Inherited"));
    QStandardItem* partial_inherited_category_item= new QStandardItem(tr("Partially Inherited"));

    /*  iterate through each sheet */
    const QStringList titles = wb.sheetNames();
    for (const QString& title : titles)
    {
        int status = NEW;

        SheetTag tag = parseSheetTag(title);
        if (!tag.isValid())
        {
            continue;
        }



        /*  search for this sheet   */
        if (!m_map.contains(tag))
        {
            qInfo() << "Cannot find sheet info:" << title;
            new_category_item->appendRow(new QStandardItem(title));
            continue;
        }




        /*  reference to RegMap   */
        const RegMap* reg_map = m_map[tag];

        if (!wb.selectSheet(title))
        {
            qCritical() << "Cannot select sheet " << title;
            emit finished(false);
            return;
        }




        /* find header row */
        int header_row = findHeaderRow(wb);

        if (header_row == -1)
        {
            qWarning() << "Cannot find header row for sheet " << title;
            continue;
        }


        /*  Now find column of Name and Visibility */
        QMap<QString, int> coi_map = findColumnOfInterest(wb, header_row, QStringList() <<LABEL_NAME << LABEL_VISIBILITY);
        int column_for_name = coi_map.contains(LABEL_NAME) ? coi_map[LABEL_NAME] : 0;
        int column_for_visibility = coi_map.contains(LABEL_VISIBILITY) ? coi_map[LABEL_VISIBILITY] :0;



        if (column_for_name == 0)
        {
            qCritical() << "Cannot find column " << LABEL_NAME << " in " << title;
            emit finished(false);
            return;
        }

        if (column_for_visibility == 0)
        {
            qCritical() << "Cannot find column " << LABEL_VISIBILITY << " in " << title;
            emit finished(false);
            return;
        }



        for (int row_idx=header_row+1; row_idx <= wb.dimension().lastRow(); ++ row_idx)
        {
            const QVariant high_address = wb.read(row_idx, 1);

            /*  if first cell starts with //, ignore it */
            if (high_address.isValid() && high_address.toString().startsWith("//"))
            {
                continue;
            }


            /*  if regname is invalid or empty, skip */
            const QVariant reg_name_label = wb.read(row_idx, column_for_name);

            if (!reg_name_label.isValid() || reg_name_label.toString().length() == 0)
            {
                continue;
            }

            const QString reg_name = reg_name_label.toString().trimmed();


            /*  if register name not found, mark as unchecked   */
            if (!reg_map->contains(reg_name))
            {
                status |= AUGMENTED;
                QXlsx::Format format;
                format.setPatternBackgroundColor(QColor(Qt::green));
                const QVariant v = wb.read(row_idx, column_for_visibility);
                wb.write(row_idx, column_for_visibility, v, format);
                continue;
            }



            /*  if visibility is invalid, set to WHITE */
            const QVariant visibility_label = wb.read(row_idx, column_for_visibility);
            const QString visibility = visibility_label.toString().trimmed();
            Visibility visibility_enum = WHITE;

            if (!visibility_label.isValid() || visibility.length() == 0)
            {
                qWarning() << QString().sprintf("(%d,%d) at %s contains invalid visibility.", row_idx, column_for_visibility, title.toStdString().c_str());
            }

            if ( !QString::compare(visibility, LABEL_VISIBILITY_RED, Qt::CaseInsensitive) )
            {
                visibility_enum = RED;
            }
            else if ( !QString::compare(visibility, LABEL_VISIBILITY_YELLOW, Qt::CaseInsensitive) )
            {
                visibility_enum = YELLOW;
            }
            else if ( !QString::compare(visibility, LABEL_VISIBILITY_WHITE, Qt::CaseInsensitive) )
            {
                visibility_enum = WHITE;
            }
            else
            {
                qWarning() << "Unknown visibility " << visibility << " at " << title << " row " << QString::number(row_idx);
                continue;
            }


            const Visibility& history_visibility = (*reg_map)[reg_name];



            if (history_visibility > visibility_enum)
            {
                QXlsx::Format format;
                format.setPatternBackgroundColor(QColor(Qt::red));
                status |= INHERITED;

                switch (history_visibility)
                {
                    case WHITE:
                        wb.write(row_idx, column_for_visibility, LABEL_VISIBILITY_WHITE, format);
                        break;
                    case YELLOW:
                        wb.write(row_idx, column_for_visibility, LABEL_VISIBILITY_YELLOW, format);
                        break;
                    case RED:
                        wb.write(row_idx, column_for_visibility, LABEL_VISIBILITY_RED, format);
                        break;
                    default:
                        qCritical() << "History data contains unknown visibility level.";

                }

            }
        }


        if (NEW == status)
        {
            unmodified_category_item->appendRow(new QStandardItem(title));
        }
        else if (INHERITED == status )
        {
            total_inherited_category_item->appendRow(new QStandardItem(title));
        }
        else if (status & AUGMENTED)
        {
            partial_inherited_category_item->appendRow(new QStandardItem(title));
        }
        else
        {
            qWarning() << "Unknown status:" << status << "\n";
        }


    }

    if (m_record_model != nullptr)
    {
        m_record_model->appendRow(new_category_item);
        m_record_model->appendRow(unmodified_category_item);
        m_record_model->appendRow(total_inherited_category_item);
        m_record_model->appendRow(partial_inherited_category_item);
    }
    else
    {
        qInfo() << "Result not saved.";
    }


    if (!wb.save())
    {
        qCritical() << tr("Cannot save changes to") << tgt;
    }

    emit finished(true);
}




SheetTag XlsxComparer::parseSheetTag(const QString& title) const
{
    const QStringList split = title.split("_BK");

    if (split.length() != 2)
    {
        qInfo() << title << " is not a valid sheet.";
        return SheetTag();
    }
    bool converted = false;
    const QString& label = split[0];
    const quint32 bank = split[1].toUInt(&converted, 16);

    if (!converted)
    {
        qWarning() << bank << " in " << title << " is not a valid hex number.";
        SheetTag();
    }

    return SheetTag(label, bank);
}

int XlsxComparer::findHeaderRow(QXlsx::Document& wb) const
{
    const QString header_mark = "High Address";
    int header_row = -1;
    for (int row_idx=1; row_idx <= wb.dimension().lastRow(); ++ row_idx)
    {
        if (header_row >=1 )
        {
            break;
        }


        const QVariant first_cell = wb.read(row_idx, 1);
        if (first_cell.isValid() && !QString::compare(first_cell.toString(), header_mark, Qt::CaseInsensitive))
        {
            header_row = row_idx;
        }

    }

    return header_row;
}


QMap<QString, int> XlsxComparer::findColumnOfInterest(QXlsx::Document &wb, const int header_row, const QStringList& coi) const
{
    QMap<QString, int> coi_map;

    for (int col_idx=1; col_idx <= wb.dimension().lastColumn(); ++ col_idx)
    {
        const QVariant header_cell = wb.read(header_row, col_idx);
        if (!header_cell.isValid())
        {
            continue;
        }

        const QString header_cell_value = header_cell.toString();

        for (const auto& c : coi)
        {
            if ( !QString::compare(header_cell_value, c, Qt::CaseInsensitive) )
            {
                coi_map.insert(c, col_idx);
            }
        }
    }

    return coi_map;
}


XlsxComparer::~XlsxComparer()
{
    for (auto const& pair : m_map.toStdMap())
    {
        delete pair.second;
    }
}
