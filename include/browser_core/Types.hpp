#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <cstdint>

namespace BrowserCore {

enum class ResourceType {
    MainFrame,
    SubFrame,
    Script,
    Image,
    StyleSheet,
    Font,
    XmlHttpRequest,
    Media,
    WebSocket,
    Other
};

inline std::string resourceTypeToString(ResourceType type) {
    switch (type) {
        case ResourceType::MainFrame: return "MainFrame";
        case ResourceType::SubFrame: return "SubFrame";
        case ResourceType::Script: return "Script";
        case ResourceType::Image: return "Image";
        case ResourceType::StyleSheet: return "StyleSheet";
        case ResourceType::Font: return "Font";
        case ResourceType::XmlHttpRequest: return "XHR/Fetch";
        case ResourceType::Media: return "Media";
        case ResourceType::WebSocket: return "WebSocket";
        default: return "Other";
    }
}

struct HttpRequest {
    std::string url;
    std::string method = "GET";
    std::map<std::string, std::string> headers;
    std::string body;
    ResourceType resourceType = ResourceType::MainFrame;
    std::string initiator; // Parent URL or origin
};

enum class InterceptAction {
    Allow,
    Block,
    Redirect,
    Modify
};

struct InterceptResult {
    InterceptAction action = InterceptAction::Allow;
    std::string redirectedUrl;
    std::map<std::string, std::string> modifiedHeaders;
    std::string reason;
};

struct TabInfo {
    uint32_t id = 0;
    std::string title;
    std::string currentUrl;
    bool isLoading = false;
    bool isMuted = false;
    bool isPinned = false;
    bool isDiscarded = false; // Memory saver
    uint32_t containerId = 0; // 0 = Default, 1 = Personal, 2 = Work, 3 = Banking, 4 = Shopping
    std::string containerName = "Default";
    std::string containerColor = "#94a3b8";
    uint32_t workspaceId = 1; // 1 = Work/Dev, 2 = Personal, 3 = Finance, 4 = Research
    size_t historyIndex = 0;
    std::vector<std::string> historyStack;
    std::chrono::system_clock::time_point lastAccessed;
};

struct SplitViewState {
    bool enabled = false;
    uint32_t primaryTabId = 0;
    uint32_t secondaryTabId = 0;
};

struct BrowserConfig {
    std::string browserName = "AtlasBrowser";
    std::string version = "1.0.0";
    std::string userAgent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36 AtlasBrowser/1.0";
    bool enableAdBlocker = true;
    bool enableHttpsUpgrade = true;
    bool stripTrackingParams = true;
    bool blockThirdPartyCookies = true;
    bool antiFingerprinting = true;
    bool preventWebRtcLeak = true;
    std::string defaultSearchEngine = "https://duckduckgo.com/?q=";
    std::string newTabPageUrl = "mybrowser://newtab";
};

} // namespace BrowserCore
