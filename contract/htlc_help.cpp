#include "htlc.hpp"

using namespace graphene;

void htlc::insert_htlc(uint64_t from, uint64_t to, string hash_algorithm, contract_asset amount, string preimage_hash, uint64_t preimage_size, uint64_t expiration, uint64_t fee_payer) {
    uint64_t pk = get_sysconfig(htlc_db_next_id_sys_ID);
    int64_t now = get_head_block_time();
    htlcrecords.emplace(fee_payer, [&](auto &a_htlc) {
        a_htlc.id = pk;
        a_htlc.from = from;
        a_htlc.to = to;
        a_htlc.hash_algorithm = hash_algorithm;
        a_htlc.amount = amount;
        a_htlc.preimage_hash = preimage_hash;
        a_htlc.preimage_size = preimage_size;
        a_htlc.expiration = now + expiration;
    });
    update_sysconfig(htlc_db_next_id_sys_ID, pk + 1, fee_payer);
}

void htlc::insert_sysconfig(uint64_t id, uint64_t value, uint64_t fee_payer) {
    auto it = sysconfigs.find(id);
    if (it == sysconfigs.end()) {
        sysconfigs.emplace(fee_payer, [&](auto &a_config) {
            a_config.id = id;
            a_config.value = value;
        });
    } else {
        update_sysconfig(id, value, fee_payer);
    }
}

// Increase account lockout balance
void htlc::add_balances(uint64_t user, contract_asset quantity, uint64_t fee_payer){
    graphene_assert(quantity.amount >= 0, "Funding operations cannot be negative");
    auto it_user = accounts.find(user);
    if (it_user == accounts.end()) {
        accounts.emplace(fee_payer, [&](auto &o) {
            o.owner = user;
            o.balances.emplace_back(quantity);
        });
    } else {
        int asset_index = 0;
        bool add = false;
        for (auto asset_it = it_user->balances.begin(); asset_it != it_user->balances.end(); ++asset_it) {
            if ((quantity.asset_id) == asset_it->asset_id) {
                accounts.modify(it_user, user, [&](auto &o) {
                    o.balances[asset_index] += quantity;
                });
                add = true;
                break;
            }
            asset_index++;
        }
        if (!add) {
            accounts.modify(it_user, fee_payer, [&](auto &o) {
                o.balances.emplace_back(quantity);
            });
        }
    }
}

// Reduce account lockout balance
void htlc::sub_balances(uint64_t user, contract_asset quantity, uint64_t fee_payer){
    graphene_assert(quantity.amount >= 0, "Funding operations cannot be negative");
    auto it_user = accounts.find(user);
    graphene_assert(it_user != accounts.end(), "Account record does not exist");

    int asset_index = 0;
    bool  sub = false;
    for (auto asset_it = it_user->balances.begin(); asset_it != it_user->balances.end(); ++asset_it) {
        if ((quantity.asset_id) == asset_it->asset_id) {
            graphene_assert(asset_it->amount >= quantity.amount, "Insufficient balance of account-related assets");
            accounts.modify(it_user, fee_payer, [&](auto &o) {
                o.balances[asset_index] -= quantity;
            });
            sub = true;
            break;
        }
        asset_index++;
    }
    graphene_assert(sub , "Account has no related assets");
}

void htlc::update_sysconfig(uint64_t id, uint64_t value, uint64_t fee_payer) {
    auto it = sysconfigs.find(id);
    if (it == sysconfigs.end()) {
        insert_sysconfig(id, value, fee_payer);
        return;
    }

    if (id == platform_status_sys_ID) {
        graphene_assert(value == pf_status_unlock || value == pf_status_lock, "Exchange status value is not supported");
    }

    sysconfigs.modify(it, fee_payer, [&](auto &o) {
        o.value = value;
    });
}

uint64_t htlc::get_sysconfig(uint64_t id) {
    auto it = sysconfigs.get(id, "Configuration information does not exist when querying");
    return it.value;
}

void htlc::auth_verify(uint64_t sender) {
    auto itr = sysconfigs.find(profit_account_sys_ID);
    if (itr != sysconfigs.end()) {
        graphene_assert(sender == itr->value, "Excessive operation");
    }
}

void htlc::status_verify() {
    uint64_t platform_status = get_sysconfig(platform_status_sys_ID);
    graphene_assert(platform_status == pf_status_unlock, "The platform is locked and cannot be traded temporarily");
}

void htlc::inner_withdraw_asset(uint64_t from, uint64_t to, uint64_t asset_id, int64_t amount) {
    if (amount > 0) {
        withdraw_asset(from, to, asset_id, amount);
    }
}

void htlc::hash_verify(string t_preimage, string preimage_hash, string hash_algorithm) {
    string t_preimage_hash;
    if (hash_algorithm.compare("sha256") == 0) {
        checksum256 t_hash;
        sha256(t_preimage.c_str(),t_preimage.length(),&t_hash);
        t_preimage_hash = toHex(t_hash.hash, 32);
    } else if (hash_algorithm.compare("sha512") == 0) {
        checksum512 t_hash;
        sha512(t_preimage.c_str(),t_preimage.length(),&t_hash);
        t_preimage_hash = toHex(t_hash.hash, 64);
    } else if (hash_algorithm.compare("ripemd160") == 0) {
        checksum160 t_hash;
        ripemd160(t_preimage.c_str(),t_preimage.length(),&t_hash);
        t_preimage_hash = toHex(t_hash.hash, 20);
    } else {
        graphene_assert(false, "The hash algorithm is no longer supported.");
    }

    graphene_assert(preimage_hash.compare(t_preimage_hash) == 0, "The preimage is not in line with expectations");
}


string htlc::toHex(const uint8_t *c, uint32_t s) {
    string r;
    const char *hexStr = "0123456789abcdef";
    for (uint32_t i = 0; i < s; ++i) {
        (r += hexStr[(c[i] >> 4)]) += hexStr[(c[i] & 0x0f)];
    }
    return r;
}
