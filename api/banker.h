#ifndef __BANKER__ 
#define __BANKER__

// standard includes
#include <unordered_map>            // shard map container
#include <mutex>                    // mtx, lock_guard
#include <boost/lockfree/queue.hpp> // lock free queue

// project includes
#include "shard_map.h"          // shard map
namespace bb
{
namespace app
{

class Banker
{
public:

  struct Request
  {
      enum Type {
        BID = 0, 
        REFUND = 1,
        POISEN_APPLE = 2
      }; // Type

      Type m_type; 
      uint64_t m_aid; 
      uint64_t m_cid;
      double m_bid;
  };// Request

  struct Response
  {
      Response(uint64_t aid, uint64_t cid, int err) : m_aid(aid), m_cid(cid), m_err(err) {}
      uint64_t m_aid;
      uint64_t m_cid;
      int m_err;
  }


  using shardMap = bb::containers::ShardMap;

  Banker() = default;
  ~Banker() = default;

  /**
   @brief start servicing
   */
  void start();

  /**
   @brief enequeue a request
   */
  void enqueueRequest(const Request& request) {m_requestQueue.push(request);}

  /**
   @brief: attempt to acquire bid for a specific compain under a specific auction
   @param aid: auction id
   @param cid: campaign id
   @param bid: wanted bid value
   @returns int
   @retval 0 - success
   @retval 1 - not enough money
   @retval 2 - campaign not exist
   */
  int acquireBid(uint64_t aid, uint64_t cid, double bid);

  /**
   @brief: refund the bid under a specific auction id
   @param aid: auction id
   */
  void refundBid(uint64_t aid);

private:

  void consumeRefund(BankerShard& shard, CampaignBalance& balance);

  void refund(BankerShard& shard, CampaignBalance& balance, double bid);

  bool acquireCampaignBid(uint64_t aid, uint64_t cid, CampaignBalance& balance, double bid);

  int getNewBalance(uint64_t cid, CampaignBalance& balance);

  struct CampaignBalance
  {
    double m_current = 0;
    double m_refund = 0;
    std::atomic<double> m_balanceSlice = 0;
  }; // CampaignBalance

  struct BankerShard
  { 
    std::unordered_map<cid, CampaignBalance> m_campaigns;
    mutable std::mutex m_mtx; // consider spinlock to avoid thread going to sleep
  }; // BankerShard

  shardMap<uint64_t, BankerShard> m_balanceMap;
  std::unordered_map<uint64_t, std::pair<uint64_t, double>> m_bids;
  boost::lockfree::queue<Banker::Request> m_requestQueue;
  boost::lockfree::queue<Banker::Response> m_responseQueue;
}; // Banker
} // app
} // bb


#endif __BANKER__