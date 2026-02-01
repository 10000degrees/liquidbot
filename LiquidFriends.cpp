#include "LiquidInternal.h"
#include <iostream>
#include <cctype>

namespace Liquid {
    std::vector<Friend> liquidFriends(const std::string& accountID) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        if (!g_accounts.count(accountID)) return {};
        std::string token = g_accounts[accountID].token;
        std::string relJson = httpRequest(token, L"GET", "/api/v10/users/@me/relationships");
        if (relJson.empty() || relJson == "[]") return {};

        std::vector<Friend> friends;
        size_t pos = 0;
        while(true) {
            size_t start = relJson.find("{", pos);
            if (start == std::string::npos) break;
            int braceCount = 0; size_t end = start; bool found = false;
            for (size_t i = start; i < relJson.length(); i++) {
                if (relJson[i] == '{') braceCount++; else if (relJson[i] == '}') braceCount--;
                if (braceCount == 0) { end = i; found = true; break; }
            }
            if (!found) break;
            std::string block = relJson.substr(start, end - start + 1);
            pos = end + 1;

            bool isFriend = false;
            size_t typeKey = block.find("\"type\":");
            if (typeKey != std::string::npos) {
                size_t valStart = typeKey + 7;
                while(valStart < block.length() && (block[valStart] == ' ' || block[valStart] == '\t' || block[valStart] == '\n')) valStart++;
                if (valStart < block.length() && block[valStart] == '1') {
                    if (valStart + 1 >= block.length() || !isdigit(block[valStart+1])) isFriend = true;
                }
            }

            if (isFriend) {
                 Friend f;
                 f.id = getRawJsonValue(block, "id"); 
                 f.username = getRawJsonValue(block, "username");
                 f.global_name = getRawJsonValue(block, "global_name");
                 size_t uStart = block.find("\"user\"");
                 if (uStart != std::string::npos) {
                     std::string uBlock = block.substr(uStart);
                     std::string uid = getRawJsonValue(uBlock, "id");
                     if (!uid.empty()) f.id = uid;
                 }
                 friends.push_back(f);
            }
        }
        if (friends.empty()) return {};

        std::string dmJson = httpRequest(token, L"GET", "/api/v10/users/@me/channels");
        if (!dmJson.empty()) {
             pos = 0;
             while(true) {
                size_t start = dmJson.find("{", pos);
                if (start == std::string::npos) break;
                int braceCount = 0; size_t end = start; bool found=false;
                for(size_t i=start; i<dmJson.length(); i++) {
                    if (dmJson[i] == '{') braceCount++; else if (dmJson[i] == '}') braceCount--;
                    if (braceCount==0) {end=i; found=true; break;}
                }
                if (!found) break;
                std::string block = dmJson.substr(start, end - start + 1);
                pos = end + 1;
                std::string chId = getRawJsonValue(block, "id");
                for (auto& f : friends) {
                    std::string idSearch = "\"" + f.id + "\"";
                    if (block.find(idSearch) != std::string::npos) f.channel_id = chId;
                }
             }
        }
        return friends;
    }
}
