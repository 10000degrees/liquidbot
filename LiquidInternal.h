#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <windows.h>
#include <winhttp.h>
#include "Liquid.h"

namespace Liquid {

    struct AccountContext {
        std::string token;
        UserInfo info;
    };

    extern std::map<std::string, AccountContext> g_accounts;
    extern std::recursive_mutex g_mutex;
    extern std::string g_prefix;
    
    std::string httpRequest(const std::string& token, const std::wstring& verb, const std::string& pathStr, const std::string& body = "");
    std::string getRawJsonValue(const std::string& json, const std::string& key);
    std::string urlEncode(const std::string& value);
}
