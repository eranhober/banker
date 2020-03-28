#ifndef __SHARD_MAP__ 
#define __SHARD_MAP__

#include <vector>                 #shard container
#include <functional>             #std::hash

namespace bb
{
namespace containers
{

template<class ID, class SHARD>
class ShardMap
{
public: 

  ShardMap(int16_t shards) : m_shards(shards), m_shardMap(shards) {}

  ~ShardMap() = default();


  const SHARD& getShard(ID id) const
  {
    return m_shardMap[std::hash(id) % m_shards];
  }

private:
  const int16_t m_shards;
  const std::vector<SHARD> m_shardMap;
}; // Banker
} // containers
} // bb

#endif __SHARD_MAP__