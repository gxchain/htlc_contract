This document describes an implementation of a Hashed Time-Locked Contract (HTLC) on GXChain.

The HTLC contract is based on hashlock and timelock for value transfer from depositor to recipient:

1) hashlock, hash lock, need to provide the original image (preimage) corresponding to the hash value, in order to unlock. The unlocking of hashlock is time-sensitive and valid before the timelock expires;

2) timelock, time lock, after the timelock expires, the depositor can retrieve the assets from the contract.

## Contract Interface

#### 1. htlccreate

The depositor can call the `htlccreate` action, lock some assets into the contract, specify the `recipient`, `asset type` and `quantity`, `preimage` SHA256 and `length` (ie hashlock), `lock expiration` (ie timelock).

action definition:

``` c++
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

The recipient calls the `htlcredeem` action before the timelock expires, redeems the asset from the contract, specifies the `htlc_db_id` and `preimage` to unlock the hashlock.

action definition:

```c++
/// @abi action
void htlcredeem(const std::string& htlc_db_id, const std::string& preimage)
{
    // check htlc_db_id
    // check sender
    // check preimage
    // redeem fund to sender
    // remove record
}
```

#### 3. htlcrefund

If the recipient does not redeem the asset before the timelock expires, the depositor can call the `htlcrefund` action to retrieve the previously locked asset. To call the `htlcrefund` action, you only need to specify `htlc_db_id`.

action definition:

```c++
/// @abi action
void htlcrefund(const std::string& htlc_db_id)
{
    // check htlc_db_id
    // check sender
    // refund to sender
    // remove record
}
```

## Table Structure:

The `htlcrecord` table is used to store the asset and lock information in the HTLC contract.

```c++
    //@abi table htlcrecord i64
    struct htlcrecord {
        uint64_t                htlc_db_id; // auto increase id
        uint64_t                from; // depositor
        uint64_t                to; // recipient
        contract_asset          amount; // asset type and amount
        checksum256             preimage_hash; // hashlock
        uint64_t                preimage_size; // preimage size
        uint64_t                expiration; // timelock

        uint64_t primary_key() const { return htlc_db_id; }
    };

```


