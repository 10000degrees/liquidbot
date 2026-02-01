#include "LiquidInternal.h"
#include <iostream>
#include <sstream>
#include "Liquid.h"

namespace Liquid {
    std::map<std::string, AccountContext> g_accounts;
    std::recursive_mutex g_mutex;
    std::string g_prefix = "!";

    std::string httpRequest(const std::string& token, const std::wstring& verb, const std::string& pathStr, const std::string& body) {
        if (token.empty()) return "";
        HINTERNET hSession = WinHttpOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36", 0, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) return "";
        HINTERNET hConnect = WinHttpConnect(hSession, L"discord.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }
        std::wstring path(pathStr.begin(), pathStr.end());
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, verb.c_str(), path.c_str(), 0, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }
        
        std::string authHeader = "Authorization: " + token;
        std::string xSuper = "eyJvcyI6IldpbmRvd3MiLCJicm93c2VyIjoiQ2hyb21lIiwiZGV2aWNlIjoiIiwic3lzdGVtX2xvY2FsZSI6ImVuLVVTIiwiYnJvd3Nlcl91c2VyX2FnZW50IjoiTW96aWxsYS81LjAgKFdpbmRvd3MgTlQgMTAuMDsgV2luNjQ7IHg2NCkgQXBwbGVXZWJLaXQvNTM3LjM2IChLSFRNTCwgbGlrZSBHZWNrbykgQ2hyb21lLzEyMC4wLjAuMCBTYWZhcmkvNTM3LjM2IiwiYnJvd3Nlcl92ZXJzaW9uIjoiMTIwLjAuMC4wIiwib3NfdmVyc2lvbiI6IjEwIiwicmVmZXJyZXIiOiIiLCJyZWZlcnJpbmdfZG9tYWluIjoiIiwicmVmZXJyZXJfY3VycmVudCI6IiIsInJlZmVycmluZ19kb21haW5fY3VycmVudCI6IiIsInJlbGVhc2VfY2hhbm5lbCI6InN0YWJsZSIsImNsaWVudF9idWlsZF9udW1iZXIiOjI1NDkyMSwiY2xpZW50X2V2ZW50X3NvdXJjZSI6bnVsbH0=";
        std::string extra = "x-super-properties: " + xSuper + "\r\nx-discord-locale: en-US\r\nx-discord-timezone: America/New_York";

        std::wstring wAuthHeader(authHeader.begin(), authHeader.end());
        std::wstring wExtra(extra.begin(), extra.end());
        std::wstring headers = wAuthHeader + L"\r\n" + wExtra;

        if (!body.empty()) {
             std::string contentType = "Content-Type: application/json";
             std::wstring wContentType(contentType.begin(), contentType.end());
             headers += L"\r\n" + wContentType;
        }

        BOOL bResults = WinHttpSendRequest(hRequest, headers.c_str(), headers.length(), (LPVOID)body.c_str(), body.length(), body.length(), 0);
        std::string responseData = "";
        if (bResults) {
            bResults = WinHttpReceiveResponse(hRequest, 0);
            if (bResults) {
                DWORD dwSize = 0, dwDownloaded = 0;
                do {
                    dwSize = 0; if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break; if (dwSize == 0) break;
                    std::vector<char> buffer(dwSize + 1); if (WinHttpReadData(hRequest, &buffer[0], dwSize, &dwDownloaded)) { buffer[dwDownloaded] = '\0'; responseData.append(buffer.data()); }
                } while (dwSize > 0);
            }
        }
        WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
        return responseData;
    }

    std::string urlEncode(const std::string& value) {
        std::ostringstream escaped; escaped.fill('0'); escaped << std::hex;
        for (char c : value) { if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') escaped << c; else { escaped << std::uppercase << '%' << int((unsigned char)c); escaped << std::nouppercase; } }
        return escaped.str();
    }
    
    std::string getRawJsonValue(const std::string& json, const std::string& key) {
        std::string search = "\"" + key + "\":";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return ""; 
        pos += search.length();
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n')) pos++;
        size_t end;
        if (json[pos] == '"') {
             pos++; end = pos;
             while (end < json.length()) { if (json[end] == '"' && json[end-1] != '\\') break; end++; }
             return json.substr(pos, end - pos);
        } else {
             end = pos;
             while (end < json.length() && json[end] != ',' && json[end] != '}') end++;
             return json.substr(pos, end - pos);
        }
    }

    std::vector<std::string> liquidGetActiveAccounts() {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        std::vector<std::string> accounts;
        for(auto const& [id, ac] : g_accounts) accounts.push_back(id);
        return accounts;
    }
}
