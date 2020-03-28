#include "banker.h"                     // API
#include "redis.h"                      // Redis Api Mock
namespace bb
{
namespace app
{

inline void Banker::consumeRefund(BankerShard& shard, CampaignBalance& balance)
{
    std::lock_guard<std::mutex> shardGuard(shard.m_mtx);
    balance.m_current += balance.m_refund;
    balance.m_refund = 0;
}

inline void Banker::refund(BankerShard& shard, CampaignBalance& balance, double bid)
{
    std::lock_guard<std::mutex> shardGuard(shard.m_mtx);
    balance.m_refund += bid;
}

inline bool Banker::acquireCampaignBid(uint64_t aid, uint64_t cid, CampaignBalance& balance, double bid)
{
    if (balance.m_current > 0)
    {
      balance.m_current -= bid;
      m_bids[aid] = std::make_pair(cid, bid)>;
      return true;
    }
    return false;
}

int Banker::getNewBalance(uint64_t cid, CampaignBalance& balance)
{
    auto slice(balance.m_balanceSlice.load());
    try
    {
      redis::decrby(cid, slice);
      if (redis::get(cid) > 0)
      {
          balance += slice;
          return 0;
      }
      redis::incby(cid, slice);
      return 1;
    }
    catch (const std::out_of_range& exp)
    {
      return 2;
    }

}

int Banker::acquireBid(uint64_t aid, uint64_t cid, double bid)
{
  const auto& shard(m_balanceMap.getShard(cid));
  try
  {
    auto& balance(shard.m_campaigns.at(cid));
    // if balance is enough
    if (acquireCampaignBid(aid, balance, bid));
    {
      return 0;
    }
    // if balance is not enough
    // 1. try to acquire from refund
    consumeRefund(shard, balance);
    if (acquireCampaignBid(aid, balance, bid));
    {
      return 0;
    }
    // 2. get new balance from db
    auto err = getNewBalance(cid, balance);
    if (not err)
    {
      return acquireCampaignBid(aid, balance, bid);
    }
  }
  catch (const std::out_of_range& exp)
  {
    CampaignBalance balance;
    auto err = getNewBalance(cid, balance);
    if (not err)
    {
      shard.m_campaigns.insert(std::make_pair(cid, balance));
      return acquireCampaignBid(aid, balance, bid);
    }
  }

}

void Banker::refundBid(uint64_t aid)
{
  try
  {
      const auto& bid(m_bids.at(aid));
      auto cid(bid.first);
      auto& shard(m_balanceMap.getShard(cid));
      auto& balance(shard.m_campaigns.at(cid));
      return refund(shard, balance, bid.second);
  }
  catch (const std::out_of_range& exp)
  {
      // notify other banker microservice instances. 
  }
} 

void Banker::start()
{
    std::array<boost::lockfree::queue<Banker::Request>> requestQueues(2);
    auto& bidsQueue = requestQueues[0];
    auto bidsThread = std::thread([&bidsQueue, this](){
      for(;;)
      {
        Banker::Request request;
        bidsQueue.pop(request);
        if (request.m_type == Banker::Request::Type::POISEN_APPLE)
        {
          return;
        }
        auto err(this->acquireBid(request.m_aid, request.m_cid, request.m_bid));
        this.m_responseQueue.push(Response(request.m_aid, request.m_cid, err));
      }
    });

    auto& refundQueue = requestQueues[1];
    auto refundThread = std::thread([&refundQueue, this](){
      for(;;)
      {
        Banker::Request request;
        refundQueue.pop(request);
        if (request.m_type == Banker::Request::Type::POISEN_APPLE)
        {
          return;
        }
        this->refundBid(request.m_aid);
      }
    });


    bool running(true);
    while(running)
    {
      Banker::Request request;
      m_requestQueue.pop(request);
      if(request.m_type not_eq Banker::Request::Type::POISEN_APPLE)
      {
        requestQueues[request.m_type].enqueue(request);
      }
      else
      {
        requestQueues[0].enqueue(request);
        requestQueues[1].enqueue(request);
        bidsThread.join();
        refundThread.join();
        running = false;
      }
    }
}

}// app
}// bb


