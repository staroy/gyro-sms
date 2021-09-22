// Copyright (c) 2014-2019, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "AddressBookModel.h"
#include "AddressBook.h"
#include <QDebug>
#include <QHash>
#include <wallet/api/wallet2_api.h>
#include "wallet/wallet_zyre.h"

UnsortedAddressBookModel::UnsortedAddressBookModel(QObject *parent, AddressBook *addressBook, DbSMS *db)
    : QAbstractListModel(parent)
    , m_addressBook(addressBook)
    , m_db(db)
{
    connect(m_addressBook,SIGNAL(refreshStarted()),this,SLOT(startReset()));
    connect(m_addressBook,SIGNAL(refreshFinished()),this,SLOT(endReset()));
}

void UnsortedAddressBookModel::startReset(){
    beginResetModel();
}
void UnsortedAddressBookModel::endReset(){
    endResetModel();
}

int UnsortedAddressBookModel::rowCount(const QModelIndex &) const
{
    return m_addressBook->count();
}

QVariant UnsortedAddressBookModel::data(const QModelIndex &index, int role) const
{
    QVariant result;

    bool found = m_addressBook->getRow(index.row(), [&](const Gyro::AddressBookRow &row) {
        switch (role) {
        case AddressBookAddressRole:
            result = QString::fromStdString(row.getAddress());
            break;
        case AddressBookLabelRole:
            result = QString::fromStdString(row.getLabel());
            break;
        case AddressBookDescriptionRole:
            result = QString::fromStdString(row.getDescription());
            break;
        case AddressBookPaymentIdRole:
            result = QString::fromStdString(row.getPaymentId());
            break;
        case AddressBookRowIdRole:
            // Qt doesnt support size_t overload type casting
            result.setValue(row.getRowId());
            break;
        case AddressBookUnreadCountRole:
            result = int(m_db->getUnreadCount(row.getAddress()));
            break;
        case AddressBookLastTimeRole:
            result = m_db->getLastTime(row.getAddress());
            break;
        default:
            qCritical() << "Unimplemented role " << role;
        }
    });
    if (!found) {
        qCritical("%s: internal error: invalid index %d", __FUNCTION__, index.row());
    }

    return result;
}

bool UnsortedAddressBookModel::deleteRow(int row)
{
    return m_addressBook->deleteRow(row);
}

int UnsortedAddressBookModel::lookupPaymentID(const QString &payment_id) const
{
    return m_addressBook->lookupPaymentID(payment_id);
}

QHash<int, QByteArray> UnsortedAddressBookModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames.insert(AddressBookAddressRole, "address");
    roleNames.insert(AddressBookLabelRole, "label");
    roleNames.insert(AddressBookPaymentIdRole, "paymentId");
    roleNames.insert(AddressBookDescriptionRole, "description");
    roleNames.insert(AddressBookRowIdRole, "rowId");
    roleNames.insert(AddressBookUnreadCountRole, "unreadCount");
    roleNames.insert(AddressBookLastTimeRole, "lastTime");

    return roleNames;
}

AddressBookModel::AddressBookModel(QObject *parent, AddressBook *addressBook, DbSMS *db)
  : QSortFilterProxyModel(parent)
  , src_(parent, addressBook, db)
{
  setSourceModel(&src_);
  setSortRole(UnsortedAddressBookModel::AddressBookLastTimeRole);
  setFilterRole(UnsortedAddressBookModel::AddressBookLabelRole);
  setSortOrder(true);
}

AddressBookModel::~AddressBookModel()
{
}

void AddressBookModel::setFilterString(QString string)
{
  this->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->setFilterFixedString(string);
  filter_ = string;
}

QString AddressBookModel::getFilterString()
{
  return filter_;
}

void AddressBookModel::setSortOrder(bool checked)
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

bool AddressBookModel::deleteRow(int row)
{
    return src_.deleteRow(row);
}

int AddressBookModel::lookupPaymentID(const QString &payment_id) const
{
    return src_.lookupPaymentID(payment_id);
}

void AddressBookModel::invalidate()
{
    QSortFilterProxyModel::invalidate();
}
