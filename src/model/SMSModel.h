
#ifndef SMSMODEL_H
#define SMSMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "DbSMS.h"

class UnsortedSMSModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum SMSRole {
        SMSFromAddressRole = Qt::UserRole + 1,
        SMSFromLabelRole,
        SMSTextRole,
        SMSTimeRole,
        SMSIndexRole,
        SMSSendByMeRole
    };
    //Q_ENUM(SMSRole)

    UnsortedSMSModel(QObject *parent, DbSMS *db);

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void append(const QString& message);
    Q_INVOKABLE void set_current(const QString& address, const QString& label);
    Q_INVOKABLE void set_self(const QString& address, const QString& label);

    QHash<int, QByteArray> roleNames() const  override;
    const std::string& getCurrentAddress() { return m_currentAddress; }

private:
    DbSMS *m_db;
    std::string m_currentAddress;
    std::string m_currentLabel;
    std::string m_selfAddress;
    std::string m_selfLabel;
};

class SMSModel : public QSortFilterProxyModel
{
  Q_OBJECT

  UnsortedSMSModel src_;
public:
  Q_ENUM(UnsortedSMSModel::SMSRole)

  SMSModel(QObject *parent, DbSMS *db);
  ~SMSModel();

  Q_INVOKABLE void setFilterString(QString string);
  Q_INVOKABLE void setSortOrder(bool checked);

  Q_INVOKABLE void append(const QString& text);
  Q_INVOKABLE void set_current(const QString& address, const QString& label);

  Q_INVOKABLE void onReceived(
      QString,    /* from address */
      QString,    /* from label */
      QString,    /* to address */
      QString,    /* to label */
      quint64,   /* n index */
      QString);   /* sms text */

  UnsortedSMSModel& source();

signals:
  void addressBookInvalidate();
};

#endif // SMSMODEL_H
