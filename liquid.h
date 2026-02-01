#pragma once
#include <string>
#include <vector>
#include <functional>

namespace Liquid {
    struct UserInfo {
        std::string id;
        std::string username;
        std::string global_name;
        int nitro_type;
        bool has_billing;
        std::string avatar_url;
        bool is_bot;
    };

    struct Friend {
        std::string id;
        std::string username;
        std::string global_name;
        std::string channel_id; 
    };

    std::string liquidLogin(const std::string& token);
    UserInfo liquidGetInfo(const std::string& accountID);
    std::vector<std::string> liquidGetActiveAccounts();

    std::vector<Friend> liquidFriends(const std::string& accountID);

    std::string liquidSend(const std::string& accountID, const std::string& channelID, const std::string& content);
    void liquidReact(const std::string& accountID, const std::string& channelID, const std::string& messageID, const std::string& emoji);
    void liquidDelete(const std::string& accountID, const std::string& channelID, const std::string& messageID);
    
    void liquidCreateChannel(const std::string& accountID, const std::string& guildID, const std::string& name, int type);
    void liquidDeleteChannel(const std::string& accountID, const std::string& channelID);
    
    void liquidListen(const std::string& accountID, const std::string& channelID);

    void liquidPrefix(const std::string& newPrefix);
    void liquidOnCommand(const std::string& commandName, std::function<void(const std::string&, const std::string&, const std::vector<std::string>&)> callback);
    void liquidWhitelistUser(const std::string& userID);

    void liquidKick(const std::string& accountID, const std::string& guildID, const std::string& userID);
    void liquidBan(const std::string& accountID, const std::string& guildID, const std::string& userID);
    void liquidTimeout(const std::string& accountID, const std::string& guildID, const std::string& userID, const std::string& duration);

    void liquidRPC(const std::string& status);
}
