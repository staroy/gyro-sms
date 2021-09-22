#ifndef DBSMS_H
#define DBSMS_H

#include <QObject>
#include <QVariant>

#include "crypto/crypto.h"
#include "zyre/db.hpp"
#include "wallet/wallet_zyre.h"

#include <map>
#include <unordered_map>

namespace Gyro {
  class Wallet;
}

class DbSMS : public QObject
{
    Q_OBJECT
public:
    DbSMS(Gyro::Wallet *wallet, QObject *parent);

    Q_INVOKABLE QVariant getData(const std::string& address, int row, int role);
    Q_INVOKABLE uint64_t getCount(const std::string& address);
    Q_INVOKABLE uint64_t getUnreadCount(const std::string& address);
    Q_INVOKABLE QString getLastTime(const std::string& address);
    Q_INVOKABLE uint64_t append(
            const std::string& from,
            const std::string& from_label,
            const std::string& to,
            const std::string& to_label,
            const std::string& text);
    Q_INVOKABLE void set_viewed(const std::string& address, uint64_t n_index);

private:
    lldb::cxx::db m_db;
    std::string m_salt;
    crypto::public_key m_pub;
    crypto::secret_key m_sec;
    std::map<std::string,
      std::unordered_map<uint64_t,
        std::pair<uint64_t, zyre::wallet::data_t>>> m_heap;
};

#endif
