#include <graphenelib/asset.h>
#include <graphenelib/contract.hpp>
#include <graphenelib/contract_asset.hpp>
#include <graphenelib/dispatcher.hpp>
#include <graphenelib/global.h>
#include <graphenelib/crypto.h>
#include <graphenelib/multi_index.hpp>
#include <graphenelib/system.h>
#include <vector>

using namespace graphene;

using std::string;
using std::vector;

class htlc : public contract
{
  public:
    htlc(uint64_t account_id)
        : contract(account_id)
        , accounts(_self, _self)
        , htlcrecords(_self, _self)
        , sysconfigs(_self, _self)
    {
    }

    const uint64_t pf_status_unlock = 1; //Available
    const uint64_t pf_status_lock = 2; // locking
    const uint64_t max_lock_time_value = 24 * 60 * 60; // Maximum lock time, default 24 hours, can be updated using the configuration update operation interface
    const uint64_t preimage_max_len = 64; // The preimage string has the maximum length

    const uint64_t platform_status_sys_ID = 0;  // Platform status id
    const uint64_t htlc_db_next_id_sys_ID = 1; // The next available primary key in the database
    const uint64_t profit_account_sys_ID = 2;  // Administrator account id
    const uint64_t max_lock_time_sys_ID = 3;  // Maximum expiration time of lock (seconds)
    const uint64_t preimage_max_len_sys_ID = 4; // Record the maximum length of the preimage

    // Hash algorithm has been implemented
    const string has_impl_hash = "sha256_sha512_ripemd160_";

    //@abi action
    void init();

    //@abi action
    void updateconfig(uint64_t id, uint64_t value);

    //@abi action
    //@abi payable
    void htlccreate(const uint64_t from, const uint64_t to, string hash_algorithm, const string preimage_hash, uint64_t preimage_size, uint64_t expiration);

    //@abi action
    void htlcredeem(const uint64_t htlc_db_id, const string preimage);

    //@abi action
    void htlcrefund(const uint64_t htlc_db_id);

    // Method added during testing
    //@abi action
    void clear(uint64_t count);

    // User contract trading account record
    //@abi table account i64
    struct account {
        uint64_t owner;
        // Account transaction balance
        vector<contract_asset> balances;

        uint64_t primary_key() const { return owner; }
        GRAPHENE_SERIALIZE(account, (owner)(balances))
    };

    //@abi table htlcrecord i64
    struct htlcrecord {
        uint64_t id; //
        uint64_t from; // Transaction payer
        uint64_t to; // Transaction recipient
        contract_asset amount; // Asset type and quantity
        string hash_algorithm; //
        string preimage_hash; // hashlock
        uint64_t preimage_size; // preimage size
        uint64_t expiration; // timelock

        uint64_t primary_key() const { return id; }
    };

    // System configuration table
    // @abi table sysconfig i64
    struct sysconfig {
        uint64_t id;
        uint64_t value;

        uint64_t primary_key() const { return id; }
        GRAPHENE_SERIALIZE(sysconfig, (id)(value))
    };

    // Configuration related
    void insert_sysconfig(uint64_t id, uint64_t value, uint64_t fee_payer);
    void update_sysconfig(uint64_t id, uint64_t value, uint64_t fee_payer);
    uint64_t get_sysconfig(uint64_t id);

    // Account related
    void add_balances(uint64_t user, contract_asset quantity, uint64_t fee_payer);
    void sub_balances(uint64_t user, contract_asset quantity, uint64_t fee_payer); 
    void inner_withdraw_asset(uint64_t from, uint64_t to, uint64_t asset_id, int64_t amount);

    // other
    void auth_verify(uint64_t sender);
    void status_verify();

    void insert_htlc(uint64_t from, uint64_t to, string hash_algorithm, contract_asset amount, string preimage_hash, uint64_t preimage_size, uint64_t expiration, uint64_t fee_payer);

    void hash_verify(string t_preimage, string preimage_hash, string hash_algorithm);
    string toHex(const uint8_t *c, uint32_t s);

    

  private:

    typedef graphene::multi_index<N(account), account> account_index;
    typedef graphene::multi_index<N(htlcrecord), htlcrecord> htlcrecord_index;
    typedef graphene::multi_index<N(sysconfig), sysconfig> sysconfig_index;

    account_index accounts;
    htlcrecord_index htlcrecords;
    sysconfig_index sysconfigs;

};
