#include "Liquid.h"
#include "LiquidInternal.h"
#include <iostream>

namespace Liquid {
    std::string liquidLogin(const std::string& token) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        std::string me = httpRequest(token, L"GET", "/api/v10/users/@me");
        
        AccountContext ac;
        ac.token = token;
        ac.info.nitro_type = 0; ac.info.has_billing = false;

        if (me.find("\"id\":") != std::string::npos) {
             ac.info.id = getRawJsonValue(me, "id");
             ac.info.username = getRawJsonValue(me, "username");
             ac.info.global_name = getRawJsonValue(me, "global_name");
             ac.info.avatar_url = getRawJsonValue(me, "avatar"); 
             ac.info.is_bot = (me.find("\"bot\":true") != std::string::npos || me.find("\"bot\": true") != std::string::npos);
             std::string nitroStr = getRawJsonValue(me, "premium_type");
             if (!nitroStr.empty()) { try { ac.info.nitro_type = std::stoi(nitroStr); } catch(...) {} }
             liquidWhitelistUser(ac.info.id);
        } else {
            return "";
        }

        std::string billing = httpRequest(token, L"GET", "/api/v10/users/@me/billing/payment-sources");
        if (billing.find("\"id\":") != std::string::npos) ac.info.has_billing = true;

        g_accounts[ac.info.id] = ac;
        return ac.info.id;
    }

    UserInfo liquidGetInfo(const std::string& accountID) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        if (g_accounts.count(accountID)) {
             Liquid::UserInfo u;
             u.id = g_accounts[accountID].info.id;
             u.username = g_accounts[accountID].info.username;
             u.global_name = g_accounts[accountID].info.global_name;
             u.nitro_type = g_accounts[accountID].info.nitro_type;
             u.has_billing = g_accounts[accountID].info.has_billing;
             u.avatar_url = g_accounts[accountID].info.avatar_url;
             u.is_bot = g_accounts[accountID].info.is_bot;
             return u;
        }
        return UserInfo();
    }
}
