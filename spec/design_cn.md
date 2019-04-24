这篇文档参考bitshares的哈希时间锁合约（HTLC），介绍GXChain上HTLC的实现细节。

HTLC合约基于hashlock和timelock，实现从存款人(depositor)到收款人(recipient)的价值传输：

1）hashlock，哈希锁，需要提供hash值对应的原像（preimage），才可以解锁。hashlock的解锁具有时效性，在timelock过期之前有效；

2）timelock， 时间锁，在timelock过期之后，depositor可以从合约取回资产。

## 合约接口：

#### 1. htlccreate

存款人（depositor）可以调用合约的htlccreate接口，向合约锁定一笔资产，指定收款人(recipient)、资产类型和数量、preimage的SHA256和length（即hashlock）、锁的过期时间(即timelock)。

合约action定义：

```c++
/// @abi action
void htlccreate(const std::string& from, const std::string& to, const checksum256& preimage_hash, uint64_t preimage_length, uint64_t expiration)
{
    // check from, assert to
    // check preimage_length
    // check expiration
    // get asset_id and amount
    // create htlc record
}
```

#### 2. htlcredeem

收款人（recipient）在timelock过期之前，调用合约htlcredeem接口，从合约中赎回资产，指定合约中的htlc_db_id、用于解锁hashlock的preimage。

合约action定义：
```c++
/// @abi action
void htlcredeem(const std::string& htlc_db_id, const std::string& preimage)
{
    // check htlc_db_id
    // check sender
    // check preimage
    // redeem fund to sender
}
```

#### 3. htlcrefund

如果收款人（recipient）在timelock过期之前未赎回资产，那么存款人（depositor）可以调用合约的htlcrefund接口，将之前锁定的资产取回。调用htlcrefund接口，只需要指定htlc_db_id。

合约action定义：
```c++
/// @abi action
void htlcrefund(const std::string& htlc_db_id)
{
    // check htlc_db_id
    // check sender
    // refund to sender
}
```

## 表结构：
htlcrecord表用于存储HTLC合约中锁定的资产和锁信息。

```c++
    //@abi table htlcrecord i64
    struct htlcrecord {
        uint64_t                htlc_db_id; // 自增id
        uint64_t                from; // 存款人（depositor
        uint64_t                to; // 收款人（recipient
        contract_asset          amount; // 资产类型和数量
        checksum256             preimage_hash; // hashlock
        uint64_t                preimage_size; // preimage size
        uint64_t                expiration; // timelock

        uint64_t primary_key() const { return htlc_db_id; }
    };

```
