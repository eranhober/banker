#ifndef __REDIS__ 
#define __REDIS__

namespace redis
{

class Redis
{
public: 

  static void decrby(uint64 cid, double bid)
  {
      std::lock_guard<std::mutex> guard(m_mtx);
      m_map.at(cid) -= bid;
  }

  static void incby(uint64 cid, double bid)
  {
      std::lock_guard<std::mutex> guard(m_mtx);
      m_map.at(cid) += bid;
  }

  static uint64_t get(uint64 cid)
  {
      std::lock_guard<std::mutex> guard(m_mtx);
      return m_map.at(cid)
  }

private:
  static std::unordered_map<uint64_t, double> m_map;
  static std::mutex m_mtx;

} // redis

#endif __REDIS__