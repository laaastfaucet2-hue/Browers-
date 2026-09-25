#pragma once

#include "Types.hpp"
#include <string>
#include <map>
#include <functional>
#include <memory>

namespace BrowserCore {

struct SchemeResponse {
    int statusCode = 200;
    std::string mimeType = "text/html; charset=utf-8";
    std::string content;
    std::map<std::string, std::string> headers;
};

using SchemeHandlerCallback = std::function<SchemeResponse(const std::string& path, const std::string& query)>;

class SchemeHandlerRegistry {
public:
    SchemeHandlerRegistry();
    ~SchemeHandlerRegistry() = default;

    // Register a scheme like "mybrowser"
    void registerScheme(const std::string& scheme);
    bool hasScheme(const std::string& scheme) const;

    // Register path handler, e.g. "mybrowser", "settings" -> callback
    void registerHandler(const std::string& scheme, const std::string& path, SchemeHandlerCallback callback);

    // Dispatch request
    SchemeResponse handleRequest(const std::string& url);

    // Helpers to generate internal HTML pages
    static std::string renderNewTabHtml(const BrowserConfig& config);
    static std::string renderSettingsHtml(const BrowserConfig& config);
    static std::string renderStatsHtml(uint64_t totalBlocked, uint64_t requestsChecked);
    static std::string renderBlockedPageHtml(const std::string& blockedUrl, const std::string& reason);
    static std::string renderAboutHtml(const BrowserConfig& config);

private:
    std::map<std::string, std::map<std::string, SchemeHandlerCallback>> m_handlers;
};

} // namespace BrowserCore
