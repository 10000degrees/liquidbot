#include "Liquid.h"
#include "LiquidInternal.h"
#include <iostream>
#include <ctime>
#include <iomanip>

namespace Liquid {
    void liquidKick(const std::string& accountID, const std::string& guildID, const std::string& userID) {
         std::lock_guard<std::recursive_mutex> lock(g_mutex);
         if (!g_accounts.count(accountID)) return;
         std::string path = "/api/v10/guilds/" + guildID + "/members/" + userID;
         httpRequest(g_accounts[accountID].token, L"DELETE", path);
    }

    void liquidBan(const std::string& accountID, const std::string& guildID, const std::string& userID) {
         std::lock_guard<std::recursive_mutex> lock(g_mutex);
         if (!g_accounts.count(accountID)) return;
         std::string path = "/api/v10/guilds/" + guildID + "/bans/" + userID;
         std::string body = "{\"delete_message_days\": 0}";
         httpRequest(g_accounts[accountID].token, L"PUT", path, body);
    }

    void liquidTimeout(const std::string& accountID, const std::string& guildID, const std::string& userID, const std::string& duration) {
         std::lock_guard<std::recursive_mutex> lock(g_mutex);
         if (!g_accounts.count(accountID)) return;
         
         int value = 0;
         try { value = std::stoi(duration); } catch(...) { return; }
         
         long long secondsToAdd = 0;
         char unit = duration.empty() ? ' ' : duration.back();
         if (unit == 'h') secondsToAdd = value * 3600;
         else if (unit == 'd') secondsToAdd = value * 86400;
         else if (unit == 'm') secondsToAdd = value * 60;
         else return;
         
         std::time_t target = std::time(nullptr) + secondsToAdd;
         std::tm* gmt = std::gmtime(&target);
         char buffer[64];
         std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S.000Z", gmt);
         std::string isoTime(buffer);
         
         std::string path = "/api/v10/guilds/" + guildID + "/members/" + userID;
         std::string body = "{\"communication_disabled_until\": \"" + isoTime + "\"}";
         httpRequest(g_accounts[accountID].token, L"PATCH", path, body);
    }
}
