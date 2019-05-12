#include "htlc_help.cpp"

using namespace graphene;

// 初始化
void htlc::init() {
    uint64_t sender = get_trx_sender();
    if (sysconfigs.begin() != sysconfigs.end()) {
        authverify(sender);
    }
    
    insert_sysconfig(platform_status_sys_ID, pf_status_unlock, sender);
    insert_sysconfig(htlc_db_next_id_sys_ID, 0, sender);
    insert_sysconfig(profit_account_sys_ID, sender, sender);
    insert_sysconfig(max_lock_time_sys_ID, max_lock_time_value, sender);
    insert_sysconfig(preimage_max_len_sys_ID, preimage_max_len, sender);
}

void htlc::updateconfig(uint64_t id, uint64_t value) {
    uint64_t sender = get_trx_sender();
    authverify(sender);
    update_sysconfig(id, value, sender);
}

// 创建交易
void htlc::htlccreate(const uint64_t from, const uint64_t to, const checksum256& preimage_hash, uint64_t preimage_size, uint64_t expiration) {
    uint64_t sender = get_trx_sender();
    uint64_t max_lock_time = get_sysconfig(max_lock_time_sys_ID);
    graphene_assert(max_lock_time >= expiration, "The effective time cannot be greater than the maximum time set by the system.");

    uint64_t preimage_max_len = get_sysconfig(preimage_max_len_sys_ID);
    graphene_assert(preimage_max_len >= preimage_size, "preimage length cannot be greater than the maximum length");

    int64_t asset_amount = get_action_asset_amount();
    uint64_t asset_id = get_action_asset_id();
    contract_asset amount{asset_amount, asset_id};
    add_balances(from, amount, sender);

    insert_htlc(from, to, amount, preimage_hash, preimage_size, expiration, sender);
}

// 交易赎回
void htlc::htlcredeem(const uint64_t htlc_db_id, const string preimage) {
    uint64_t sender = get_trx_sender();

    uint64_t preimage_max_len = get_sysconfig(preimage_max_len_sys_ID);
    uint64_t preimage_len = preimage.length();
    graphene_assert(preimage_max_len >= preimage_len, "preimage length cannot be greater than the maximum length");

    auto t_htlc = htlcrecords.find(htlc_db_id);
    graphene_assert(t_htlc != htlcrecords.end(), "htlc transaction record does not exist");
    graphene_assert(t_htlc->preimage_size == preimage_len, "Given the preimage and the length of the recorded preimage do not want to be the same");

    int64_t now = get_head_block_time();
    graphene_assert(t_htlc->expiration >= now, "htlc transaction has expired");

    checksum256 t_preimage_hash;
    sha256((char *) &preimage, preimage.length(), &t_preimage_hash);

    graphene_assert(t_htlc->preimage_hash == t_preimage_hash, "the preimage is not in line with expectations");

    sub_balances(t_htlc->from, t_htlc->amount, sender);
    my_withdraw_asset(_self, t_htlc->to, t_htlc->amount.asset_id, t_htlc->amount.amount);

    htlcrecords.erase(t_htlc);
}

// 交易退回
void htlc::htlcrefund(const uint64_t htlc_db_id) {
    auto t_htlc = htlcrecords.find(htlc_db_id);
    graphene_assert(t_htlc != htlcrecords.end(), "htlc transaction record does not exist");

    int64_t now = get_head_block_time();
    graphene_assert(t_htlc->expiration <= now, "The transaction cannot be revoked within the validity period");

    uint64_t sender = get_trx_sender();
    sub_balances(t_htlc->from, t_htlc->amount, sender);
    my_withdraw_asset(_self, t_htlc->from, t_htlc->amount.asset_id, t_htlc->amount.amount);

    htlcrecords.erase(t_htlc);
}

GRAPHENE_ABI(htlc, (init)(updateconfig)(htlccreate)(htlcredeem)(htlcrefund))
