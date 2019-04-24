本文介绍GXChain上哈希时间锁合约的具体实现。
HTLC合约基于hashlock和timelock，实现从存款人(depositor)到收款人(recipient)的价值传输：
1）hashlock，哈希锁，需要提供hash值对应的preimage，才可以解锁。hashlock的解锁，在timelock过期之前有效；
2）timelock， 时间锁，在timelock过期之后，depositor可以从合约取回资产。

### 合约接口：
#### htlccreate
depositor可以调用htlccreate接口，向合约锁定一笔资产。

合约action定义：
```c++
    /// @abi action
    void htlccreate(const std::string& from, const std::string& to, const sha256& preimage_hash, uint64_t preimage_length, uint64_t expiration)
```


#### htlcredeem
recipient在timelock过期之前，调用htlcredeem接口，从合约中赎回资产。

合约action定义：
```c++
    /// @abi action
    void htlcredeem(const std::string& htlc_database_id, const std::string& preimage, const std::string& to)
```

#### htlcrefund
如果recipient在timelock过期之前未赎回资产，那么depositor可以调用htlcrefund接口，将锁定的资产取回。

合约action定义：
```c++
    /// @abi action
    void htlcrefund(const std::string& htlc_database_id)
```
