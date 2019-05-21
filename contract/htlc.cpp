#include "htlc_help.cpp"

using namespace graphene;

// Initialization contract
void htlc::init() {
    uint64_t sender = get_trx_sender();
    if (sysconfigs.begin() != sysconfigs.end()) {
        auth_verify(sender);
    }
    
    insert_sysconfig(platform_status_sys_ID, pf_status_unlock, sender);
    insert_sysconfig(htlc_db_next_id_sys_ID, 0, sender);
    insert_sysconfig(profit_account_sys_ID, sender, sender);
    insert_sysconfig(max_lock_time_sys_ID, max_lock_time_value, sender);
    insert_sysconfig(preimage_max_len_sys_ID, preimage_max_len, sender);
}

// Contract configuration update
void htlc::updateconfig(uint64_t id, uint64_t value) {
    uint64_t sender = get_trx_sender();
    auth_verify(sender);
    update_sysconfig(id, value, sender);
}

// Hash lock transaction creation
void htlc::htlccreate(const uint64_t from, const uint64_t to, const string hash_algorithm, const checksum256& preimage_hash, uint64_t preimage_size, uint64_t expiration) {
    uint64_t sender = get_trx_sender();
    uint64_t max_lock_time = get_sysconfig(max_lock_time_sys_ID);
    graphene_assert(max_lock_time >= expiration, "The transaction lockout time cannot be longer than the maximum lock time set by the contract.");

    uint64_t preimage_max_len = get_sysconfig(preimage_max_len_sys_ID);
    graphene_assert(preimage_max_len >= preimage_size, "preimage length cannot be greater than the maximum length");

    int64_t asset_amount = get_action_asset_amount();
    uint64_t asset_id = get_action_asset_id();
    contract_asset amount{asset_amount, asset_id};
    add_balances(from, amount, sender);

    string::size_type idx;
    string t_hash_algorithm = hash_algorithm;
    // t_hash_algorithm.append(hash_algorithm);
    t_hash_algorithm.append("_");
    idx = has_impl_hash.find(t_hash_algorithm);
    graphene_assert(idx != string::npos, "Hash verification algorithm has not been implemented");

    insert_htlc(from, to, amount, hash_algorithm, preimage_hash, preimage_size, expiration, sender);
}

// Transaction redemption
void htlc::htlcredeem(const uint64_t htlc_db_id, const string preimage) {
    uint64_t sender = get_trx_sender();

    uint64_t preimage_max_len = get_sysconfig(preimage_max_len_sys_ID);
    uint64_t preimage_len = preimage.length();
    graphene_assert(preimage_max_len >= preimage_len, "preimage length cannot be greater than the maximum length");

    auto t_htlc = htlcrecords.find(htlc_db_id);
    graphene_assert(t_htlc != htlcrecords.end(), "htlc transaction record does not exist");
    graphene_assert(t_htlc->preimage_size == preimage_len, "The given preimage and the original transaction are not the same length.");

    int64_t now = get_head_block_time();
    graphene_assert(t_htlc->expiration >= now, "htlc transaction has expired");

    // hash_verify(preimage, t_htlc->preimage_hash, t_htlc->hash_algorithm);
    checksum256 t_preimage_hash;
    sha256((char *) &preimage, preimage.length(), &t_preimage_hash);
    // graphene_assert(t_htlc->preimage_hash == t_preimage_hash, "the preimage is not in line with expectations");
    hashlogs.emplace(sender, [&](auto &a_hashlog) {
        a_hashlog.id = hashlogs.available_primary_key();
        a_hashlog.hash = t_preimage_hash;
    });

    sub_balances(t_htlc->from, t_htlc->amount, sender);
    inner_withdraw_asset(_self, t_htlc->to, t_htlc->amount.asset_id, t_htlc->amount.amount);

    htlcrecords.erase(t_htlc);
}

// Return of transaction
void htlc::htlcrefund(const uint64_t htlc_db_id) {
    auto t_htlc = htlcrecords.find(htlc_db_id);
    graphene_assert(t_htlc != htlcrecords.end(), "htlc transaction record does not exist");

    int64_t now = get_head_block_time();
    graphene_assert(t_htlc->expiration <= now, "The transaction cannot be revoked within the validity period");

    uint64_t sender = get_trx_sender();
    sub_balances(t_htlc->from, t_htlc->amount, sender);
    inner_withdraw_asset(_self, t_htlc->from, t_htlc->amount.asset_id, t_htlc->amount.amount);

    htlcrecords.erase(t_htlc);
}

void htlc::clear(uint64_t count) {
    uint64_t sender = get_trx_sender();
    auth_verify(sender);
    uint64_t t_delete_count = 0;

    for(auto itr = htlcrecords.begin(); itr != htlcrecords.end();) {
        itr = htlcrecords.erase(itr);
        t_delete_count += 1;
        if (t_delete_count >= count) {
            break;
        }
    }

    for(auto itr = accounts.begin(); itr != accounts.end();) {
        for (auto asset_it = itr->balances.begin(); asset_it != itr->balances.end(); ++asset_it) {
            if (asset_it->amount > 0) {
                inner_withdraw_asset(_self, itr->owner, asset_it->asset_id, asset_it->amount);
            }
        }
        itr = accounts.erase(itr);
        t_delete_count += 1;
        if (t_delete_count >= count) {
            break;
        }
    }

    for(auto itr = hashlogs.begin(); itr != hashlogs.end();) {
        itr = hashlogs.erase(itr);
        t_delete_count += 1;
        if (t_delete_count >= count) {
            break;
        }
    }

    graphene_assert(t_delete_count > 0, "no any data have been clear");

}

GRAPHENE_ABI(htlc, (init)(updateconfig)(htlccreate)(htlcredeem)(htlcrefund)(clear))
