#include "Liquid.h"
#include "LiquidInternal.h"
#include <iostream>
#include <thread>
#include <functional>
#include <sstream>
#include <algorithm>

namespace Liquid {
    static std::map<std::string, std::function<void(const std::string&, const std::string&, const std::vector<std::string>&)>> g_commands;
    static std::vector<std::string> g_whitelist;

    void liquidRPC(const std::string& status) { std::cout << "RPC: " << status << std::endl; }
    void liquidPrefix(const std::string& newPrefix) { g_prefix = newPrefix; }

    void liquidOnCommand(const std::string& commandName, std::function<void(const std::string&, const std::string&, const std::vector<std::string>&)> callback) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        g_commands[commandName] = callback;
    }

    void liquidWhitelistUser(const std::string& userID) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        if (std::find(g_whitelist.begin(), g_whitelist.end(), userID) == g_whitelist.end()) g_whitelist.push_back(userID);
    }

    void liquidListen(const std::string& accountID, const std::string& channelID) {
        std::thread([accountID, channelID]() {
            std::string token;
            {
                std::lock_guard<std::recursive_mutex> lock(g_mutex);
                if (!g_accounts.count(accountID)) return;
                token = g_accounts[accountID].token;
            }
            std::cout << "[Liquid] Started Async Listener on " << channelID << " for " << accountID << std::endl;
            std::string lastMessageID = "";
            std::string fastPath = "/api/v10/channels/" + channelID + "/messages?limit=1";
            std::string resp = httpRequest(token, L"GET", fastPath);
            if (!resp.empty()) lastMessageID = getRawJsonValue(resp, "id");
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                std::string path = fastPath;
                if (!lastMessageID.empty()) path += "&after=" + lastMessageID;
                resp = httpRequest(token, L"GET", path);
                if (resp.empty() || resp == "[]") continue;
                std::string msgId = getRawJsonValue(resp, "id");
                if (msgId == lastMessageID || msgId.empty()) continue;
                lastMessageID = msgId;
                std::string content = getRawJsonValue(resp, "content");
                std::string authorId = "";
                size_t authorPart = resp.find("\"author\"");
                if (authorPart != std::string::npos) authorId = getRawJsonValue(resp.substr(authorPart), "id");
                
                bool isWhitelisted = false;
                {
                    std::lock_guard<std::recursive_mutex> lock(g_mutex);
                    for (const auto& u : g_whitelist) if (u == authorId) { isWhitelisted = true; break; }
                }

                if (content.rfind(g_prefix, 0) == 0) {
                     if (!isWhitelisted) continue;
                     std::string cmdLine = content.substr(g_prefix.length());
                     std::istringstream iss(cmdLine);
                     std::string cmdName; iss >> cmdName;
                     std::vector<std::string> args; std::string arg;
                     while(iss >> arg) args.push_back(arg);
                     std::lock_guard<std::recursive_mutex> lock(g_mutex);
                     if (g_commands.count(cmdName)) g_commands[cmdName](accountID, channelID, args);
                } else {
                     std::cout << "[(" << accountID << ") Msg] " << authorId << ": " << content << std::endl;
                }
            }
        }).detach();
    }
}
