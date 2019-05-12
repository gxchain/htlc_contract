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

const uint64_t pf_status_unlock = 1; //可使用
const uint64_t pf_status_lock = 2; // 锁定
const uint64_t max_lock_time_value = 24 * 60 * 60; // 最大锁定时间， 默认24小时
const uint64_t preimage_max_len = 64; // 最大锁定时间， 默认24小时

const uint64_t platform_status_sys_ID = 0;  // 平台状态id
const uint64_t htlc_db_next_id_sys_ID = 1; // hash锁定数据库下一个可用主键
const uint64_t profit_account_sys_ID = 2;  // 管理员账号id 
const uint64_t max_lock_time_sys_ID = 3;  // hash 锁定得最大过期时间
const uint64_t preimage_max_len_sys_ID = 4; // 记录原象得最大长度

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

    // 初始化
    //@abi action
    void init();

    // 配置更新
    //@abi action
    void updateconfig(uint64_t id, uint64_t value);

    //@abi action
    //@abi payable
    void htlccreate(const uint64_t from, const uint64_t to, const checksum256& preimage_hash, uint64_t preimage_size, uint64_t expiration);

    //@abi action
    void htlcredeem(const uint64_t htlc_db_id, const string preimage);

    //@abi action
    void htlcrefund(const uint64_t htlc_db_id);

    // 账户记录
    //@abi table account i64
    struct account {
        uint64_t owner;
        // 可用余额
        std::vector<contract_asset> balances;

        uint64_t primary_key() const { return owner; }
        GRAPHENE_SERIALIZE(account, (owner)(balances))
    };

    //@abi table htlcrecord i64
    struct htlcrecord {
        uint64_t                id; // 自增id
        uint64_t                from; // 存款人（depositor
        uint64_t                to; // 收款人（recipient
        contract_asset          amount; // 资产类型和数量
        checksum256             preimage_hash; // hashlock
        uint64_t                preimage_size; // preimage size
        uint64_t                expiration; // timelock

        uint64_t primary_key() const { return id; }
    };

    // 系统配置表 128
    // @abi table sysconfig i64
    struct sysconfig {
        uint64_t id;
        uint64_t value;

        uint64_t primary_key() const { return id; }
        GRAPHENE_SERIALIZE(sysconfig, (id)(value))
    };

    // 配置相关
    void insert_sysconfig(uint64_t id, uint64_t value, uint64_t fee_payer);
    void update_sysconfig(uint64_t id, uint64_t value, uint64_t fee_payer);
    uint64_t get_sysconfig(uint64_t id);

    // 账户相关
    void add_balances(uint64_t user, contract_asset quantity, uint64_t fee_payer);
    void sub_balances(uint64_t user, contract_asset quantity, uint64_t fee_payer); 
    void my_withdraw_asset(uint64_t from, uint64_t to, uint64_t asset_id, int64_t amount);

    void authverify(uint64_t sender);
    void statusverify();

    void insert_htlc(uint64_t from, uint64_t to, contract_asset amount, checksum256 preimage_hash, uint64_t preimage_size, uint64_t expiration, uint64_t fee_payer);
    void verify_htlc(uint64_t htlc_id, checksum256 preimage);

  private:

    typedef graphene::multi_index<N(account), account> account_index;
    typedef graphene::multi_index<N(htlcrecord), htlcrecord> htlcrecord_index;
    typedef graphene::multi_index<N(sysconfig), sysconfig> sysconfig_index;

    account_index accounts;
    htlcrecord_index htlcrecords;
    sysconfig_index sysconfigs;

};
