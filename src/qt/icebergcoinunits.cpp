#include "icebergcoinunits.h"

#include <QStringList>

IcebergcoinUnits::IcebergcoinUnits(QObject *parent):
        QAbstractListModel(parent),
        unitlist(availableUnits())
{
}

QList<IcebergcoinUnits::Unit> IcebergcoinUnits::availableUnits()
{
    QList<IcebergcoinUnits::Unit> unitlist;
    unitlist.append(ICB);
    unitlist.append(mICB);
    unitlist.append(uICB);
    return unitlist;
}

bool IcebergcoinUnits::valid(int unit)
{
    switch(unit)
    {
    case ICB:
    case mICB:
    case uICB:
        return true;
    default:
        return false;
    }
}

QString IcebergcoinUnits::name(int unit)
{
    switch(unit)
    {
    case ICB: return QString("ICB");
    case mICB: return QString("mICB");
    case uICB: return QString::fromUtf8("μICB");
    default: return QString("???");
    }
}

QString IcebergcoinUnits::description(int unit)
{
    switch(unit)
    {
    case ICB: return QString("Icebergcoins");
    case mICB: return QString("Milli-Icebergcoins (1 / 1,000)");
    case uICB: return QString("Micro-Icebergcoins (1 / 1,000,000)");
    default: return QString("???");
    }
}

qint64 IcebergcoinUnits::factor(int unit)
{
    switch(unit)
    {
    case ICB:  return 100000000;
    case mICB: return 100000;
    case uICB: return 100;
    default:   return 100000000;
    }
}

int IcebergcoinUnits::amountDigits(int unit)
{
    switch(unit)
    {
    case ICB: return 8; // 21,000,000 (# digits, without commas)
    case mICB: return 11; // 21,000,000,000
    case uICB: return 14; // 21,000,000,000,000
    default: return 0;
    }
}

int IcebergcoinUnits::decimals(int unit)
{
    switch(unit)
    {
    case ICB: return 8;
    case mICB: return 5;
    case uICB: return 2;
    default: return 0;
    }
}

QString IcebergcoinUnits::format(int unit, qint64 n, bool fPlus)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if(!valid(unit))
        return QString(); // Refuse to format invalid unit
    qint64 coin = factor(unit);
    int num_decimals = decimals(unit);
    qint64 n_abs = (n > 0 ? n : -n);
    qint64 quotient = n_abs / coin;
    qint64 remainder = n_abs % coin;
    QString quotient_str = QString::number(quotient);
    QString remainder_str = QString::number(remainder).rightJustified(num_decimals, '0');

    // Right-trim excess zeros after the decimal point
    int nTrim = 0;
    for (int i = remainder_str.size()-1; i>=2 && (remainder_str.at(i) == '0'); --i)
        ++nTrim;
    remainder_str.chop(nTrim);

    if (n < 0)
        quotient_str.insert(0, '-');
    else if (fPlus && n > 0)
        quotient_str.insert(0, '+');
    return quotient_str + QString(".") + remainder_str;
}

QString IcebergcoinUnits::formatWithUnit(int unit, qint64 amount, bool plussign)
{
    return format(unit, amount, plussign) + QString(" ") + name(unit);
}

bool IcebergcoinUnits::parse(int unit, const QString &value, qint64 *val_out)
{
    if(!valid(unit) || value.isEmpty())
        return false; // Refuse to parse invalid unit or empty string
    int num_decimals = decimals(unit);
    QStringList parts = value.split(".");

    if(parts.size() > 2)
    {
        return false; // More than one dot
    }
    QString whole = parts[0];
    QString decimals;

    if(parts.size() > 1)
    {
        decimals = parts[1];
    }
    if(decimals.size() > num_decimals)
    {
        return false; // Exceeds max precision
    }
    bool ok = false;
    QString str = whole + decimals.leftJustified(num_decimals, '0');

    if(str.size() > 18)
    {
        return false; // Longer numbers will exceed 63 bits
    }
    qint64 retvalue = str.toLongLong(&ok);
    if(val_out)
    {
        *val_out = retvalue;
    }
    return ok;
}

int IcebergcoinUnits::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant IcebergcoinUnits::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < unitlist.size())
    {
        Unit unit = unitlist.at(row);
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QVariant(name(unit));
        case Qt::ToolTipRole:
            return QVariant(description(unit));
        case UnitRole:
            return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}
