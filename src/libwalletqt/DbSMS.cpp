#include "DbSMS.h"
#include "model/SMSModel.h"
#include "wallet/api/wallet2_api.h"
#include "string_tools.h"
#include <QDateTime>

DbSMS::DbSMS(Gyro::Wallet *w, QObject *parent)
  : QObject(parent)
  , m_db(w->path()+".sms")
{
  epee::string_tools::hex_to_pod(w->secretSMSKey(), m_sec);
  m_pub = zyre::wallet::get_sms_public_key(m_sec);
  m_salt = zyre::wallet::get_sms_salt(m_sec);
}

QVariant DbSMS::getData(const std::string& address, int row, int role)
{
  zyre::wallet::hash_t h; std::string tmp = address + m_salt;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h.u.h);
  lldb::cxx::db db = m_db[MESSAGE][h];

  zyre::wallet::data_t *p_info = nullptr;
  bool sent_by_me = false;

  uint64_t n;

  auto it = db.begin() += row;
  if(it != db.end())
  {
    zyre::wallet::hash_t h_from; uint64_t rtx;
    it->first >> n >> h_from >> rtx;

    std::unordered_map<uint64_t,
       std::pair<uint64_t, zyre::wallet::data_t>>& heap = m_heap[address];

    if(heap.find(n) == heap.end())
    {
      zyre::wallet::data_cipher_t cipher;
      it->second >> cipher;

      std::string buf;
      try
      {
        zyre::wallet::decrypt(cipher.d, cipher.u.k, cipher.iv, m_sec, buf);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error decrypt: " << e.what());
        return QVariant();
      }

      zyre::wallet::data_t info;
      try
      {
        msgpack::object_handle o1 = msgpack::unpack(buf.data(), buf.size());
        o1.get().convert(info);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
        return QVariant();
      }

      heap[n] = {rtx, info};
    }

    sent_by_me = heap[n].first & MESSAGE_TOSEND;
    p_info = &heap[n].second;
  }

  if(p_info)
  {
    auto r_db = m_db[CONTACT][h][MESSAGES_VIEWED];
    uint64_t r = 0; r_db >> r;
    if(r < n) r_db = n;

    switch (role)
    {
      case UnsortedSMSModel::SMSFromAddressRole : return p_info->fa.c_str();
      case UnsortedSMSModel::SMSTextRole: return p_info->d.c_str();
      case UnsortedSMSModel::SMSTimeRole: return QDateTime().fromSecsSinceEpoch(p_info->ts).toString("dd.MM.yyyy hh:mm");
      case UnsortedSMSModel::SMSFromLabelRole: return p_info->f.c_str();
      case UnsortedSMSModel::SMSIndexRole: return qulonglong(n);
      case UnsortedSMSModel::SMSSendByMeRole: return sent_by_me;
    }
  }
  return QVariant();
}

uint64_t DbSMS::append(
        const std::string& from,
        const std::string& from_label,
        const std::string& to,
        const std::string& to_label,
        const std::string& text)
{
  std::string tmp = to + m_salt;
  zyre::wallet::hash_t h_to;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);

  tmp = from + m_salt;
  zyre::wallet::hash_t h_from;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);

  time_t tm = time(nullptr);

  zyre::wallet::data_t info{ text, to_label, to, from_label, from, tm };
  std::stringstream ss;
  msgpack::pack(ss, info);
  zyre::wallet::data_cipher_t cipher;

  try
  {
    zyre::wallet::encrypt(m_pub, ss.str(), cipher.d, cipher.u.k, cipher.iv);
  }
  catch (const std::exception& e)
  {
    MLOG_RED(el::Level::Warning, "failed encript message for store");
    return 0;
  }

  uint64_t n = 0;
  m_db[CONTACT][h_to][MESSAGES_COUNTER] >> n; n++;
  m_db[MESSAGE][h_to][n][h_from][MESSAGE_TOSEND] = cipher;
  m_db[CONTACT][h_to][MESSAGES_COUNTER] = n;
  m_db[CONTACT][h_to][MESSAGES_TIME] = tm;
  m_db[CONTACT][h_to][MESSAGES_VIEWED] = n;

  return n;
}

uint64_t DbSMS::getCount(const std::string& address)
{
  zyre::wallet::hash_t h; std::string tmp = address + m_salt;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h.u.h);
  uint64_t c = 0;
  m_db[CONTACT][h][MESSAGES_COUNTER] >> c;
  return c;
}

uint64_t DbSMS::getUnreadCount(const std::string& address)
{
  zyre::wallet::hash_t h; std::string tmp = address + m_salt;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h.u.h);
  uint64_t c = 0, v = 0;
  m_db[CONTACT][h][MESSAGES_COUNTER] >> c;
  m_db[CONTACT][h][MESSAGES_VIEWED] >> v;
  return c-v;
}

QString DbSMS::getLastTime(const std::string& address)
{
  zyre::wallet::hash_t h; std::string tmp = address + m_salt;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h.u.h);
  uint64_t tm = time(nullptr);
  m_db[CONTACT][h][MESSAGES_TIME] >> tm;
  return QDateTime().fromSecsSinceEpoch(qint64(tm)).toString("dd.MM.yyyy hh:mm");;
}

void DbSMS::set_viewed(const std::string& address, uint64_t n_index)
{
  zyre::wallet::hash_t h; std::string tmp = address + m_salt;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h.u.h);
  uint64_t r = 0;
  m_db[CONTACT][h][MESSAGES_VIEWED] >> r;
  if(r <= n_index)
    m_db[CONTACT][h][MESSAGES_VIEWED] = n_index;
}
