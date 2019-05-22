# Decentralized asset trading contract based on hash time locking


## todo
+ Add other hash algorithm validation implementations

## 说明
+ dev_user : Contract issuer

## Contract deployment related
### Contract compilation
``````
gxx -o htlc.wast htlc.cpp && gxx -g htlc.abi htlc.cpp
``````
#### Contract deployment
``````
deploy_contract htlc dev_user 0 0 ./contract/htlc GXC true
``````
#### Contract update
``````
update_contract htlc dev_user ./contract/htlc GXC true
``````

## Interface call
### clear data 
``````
void clear(uint64_t count);

call_contract dev_user htlc null clear "{\"count\":20}" GXS true
``````
### init
``````
void init();

call_contract dev_user htlc null init "{}" GXS true
``````
### update config
``````
void updateconfig(uint64_t id, uint64_t value);

call_contract dev_user htlc null updateconfig "{\"id\":1,\"value\":1}" GXS true
``````
### create trade
``````
void htlccreate(const uint64_t from, const uint64_t to, string hash_algorithm, const string preimage_hash, uint64_t preimage_size, uint64_t expiration);

call_contract dev_user htlc {"amount":100000,"asset_id":1.3.1} htlccreate "{\"from\":980,\"to\":979, \"preimage_hash\":\"0782709d75f72a13026c8fbf8ba761045210bd0bbeae5678a16752a7e2ce3a31\",\"preimage_size\":64,\"expiration\":3600}" GXS true
``````
### Get assets by locking the preimage
``````
void htlcredeem(const uint64_t htlc_db_id, const string preimage);

call_contract dev_user htlc null htlcredeem "{\"htlc_db_id\":0,\"preimage\":\"9c490308664645d104eaef0a80c24ec253f8f656ef7d0832929ce46a1655b35c\"}" GXS true
``````
### Return of transaction
``````
void htlcrefund(const uint64_t htlc_db_id);

call_contract dev_user htlc null htlcrefund "{\"htlc_db_id\":2}" GXS true
``````

transfer dev_user wyqcfl-hhhhh 100 GXC "this is a test" true

