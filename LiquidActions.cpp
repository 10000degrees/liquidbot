#include "Liquid.h"
#include "LiquidInternal.h"
#include <iostream>

namespace Liquid {
    std::string liquidSend(const std::string& accountID, const std::string& channelID, const std::string& content) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        if (!g_accounts.count(accountID)) return "";
        std::string token = g_accounts[accountID].token;
        std::string path = "/api/v10/channels/" + channelID + "/messages";
        std::string escapedContent = "";
        for (char c : content) { if (c == '"') escapedContent += "\\\""; else if (c == '\\') escapedContent += "\\\\"; else escapedContent += c; }
        std::string body = "{\"content\": \"" + escapedContent + "\"}";
        std::string resp = httpRequest(token, L"POST", path, body);
        return getRawJsonValue(resp, "id");
    }

    void liquidReact(const std::string& accountID, const std::string& channelID, const std::string& messageID, const std::string& emoji) {
         std::lock_guard<std::recursive_mutex> lock(g_mutex);
         if (!g_accounts.count(accountID)) return;
         std::string path = "/api/v10/channels/" + channelID + "/messages/" + messageID + "/reactions/" + urlEncode(emoji) + "/@me";
         httpRequest(g_accounts[accountID].token, L"PUT", path);
    }

    void liquidDelete(const std::string& accountID, const std::string& channelID, const std::string& messageID) {
         std::lock_guard<std::recursive_mutex> lock(g_mutex);
         if (!g_accounts.count(accountID)) return;
         std::string path = "/api/v10/channels/" + channelID + "/messages/" + messageID;
         httpRequest(g_accounts[accountID].token, L"DELETE", path);
    }

    void liquidCreateChannel(const std::string& accountID, const std::string& guildID, const std::string& name, int type) {
         std::lock_guard<std::recursive_mutex> lock(g_mutex);
         if (!g_accounts.count(accountID)) return;
         std::string path = "/api/v10/guilds/" + guildID + "/channels";
         std::string body = "{\"name\": \"" + name + "\", \"type\": " + std::to_string(type) + "}";
         httpRequest(g_accounts[accountID].token, L"POST", path, body);
    }

    void liquidDeleteChannel(const std::string& accountID, const std::string& channelID) {
         std::lock_guard<std::recursive_mutex> lock(g_mutex);
         if (!g_accounts.count(accountID)) return;
         std::string path = "/api/v10/channels/" + channelID;
         httpRequest(g_accounts[accountID].token, L"DELETE", path);
    }
}
