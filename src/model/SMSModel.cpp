#include "SMSModel.h"
#include <QDebug>
#include <QHash>

#include <wallet/api/wallet2_api.h>
#include "wallet/wallet_zyre.h"

UnsortedSMSModel::UnsortedSMSModel(QObject *parent, DbSMS *db)
    : QAbstractListModel(parent)
    , m_db(db)
{
}

int UnsortedSMSModel::rowCount(const QModelIndex &) const
{
    return m_db->getCount(m_currentAddress);
}

QVariant UnsortedSMSModel::data(const QModelIndex &index, int role) const
{
    return m_db->getData(m_currentAddress, index.row(), role);
}

void UnsortedSMSModel::append(const QString& sms)
{
    int n = int(m_db->getCount(m_currentAddress));
    beginInsertRows(QModelIndex(), n, n);
    m_db->append(m_selfAddress, m_selfLabel, m_currentAddress, m_currentLabel, sms.toStdString());
    endInsertRows();
}

void UnsortedSMSModel::set_current(const QString& address, const QString& label)
{
    beginResetModel();
    m_currentAddress = address.toStdString();
    m_currentLabel = label.toStdString();
    endResetModel();
}

void UnsortedSMSModel::set_self(const QString& address, const QString& label)
{
    m_selfAddress = address.toStdString();
    m_selfLabel = label.toStdString();
}

QHash<int, QByteArray> UnsortedSMSModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames.insert(SMSFromAddressRole, "fromAddress");
    roleNames.insert(SMSFromLabelRole, "fromLabel");
    roleNames.insert(SMSTextRole, "textSMS");
    roleNames.insert(SMSTimeRole, "time");
    roleNames.insert(SMSIndexRole, "n_index");
    roleNames.insert(SMSSendByMeRole, "sentByMe");

    return roleNames;
}

SMSModel::SMSModel(QObject *parent, DbSMS *db)
  : QSortFilterProxyModel(parent)
  , src_(parent, db)
{
  setSortOrder(false);
  setSourceModel(&src_);
  setSortRole(UnsortedSMSModel::SMSIndexRole);
  setFilterRole(UnsortedSMSModel::SMSTextRole);
}

SMSModel::~SMSModel()
{
}

void SMSModel::setFilterString(QString string)
{
  this->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->setFilterFixedString(string);
}

void SMSModel::setSortOrder(bool checked)
{
  if(checked)
  {
    this->sort(0, Qt::DescendingOrder);
  }
  else
  {
    this->sort(0, Qt::AscendingOrder);
  }
}

void SMSModel::append(const QString& sms)
{
  src_.append(sms);
  emit addressBookInvalidate();
}

void SMSModel::set_current(const QString& address, const QString& label)
{
  src_.set_current(address, label);
  emit addressBookInvalidate();
}

void SMSModel::onReceived(
    QString fromAddress, QString fromLabel,
    QString toAddress, QString toLabel,
    quint64 nIndex, QString smsText)
{
  if(fromAddress.toStdString() == src_.getCurrentAddress())
  {
    invalidate();
  }
  emit addressBookInvalidate();
}

UnsortedSMSModel& SMSModel::source()
{
  return src_;
}
