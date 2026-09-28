#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct NetworkLogEntry {
    uint32_t id;
    std::string url;
    std::string method;
    int statusCode;
    std::string mimeType;
    uint64_t sizeBytes;
    double durationMs;
    std::string timestamp;
};

struct ConsoleLogEntry {
    std::string level; // "info", "warn", "error", "log"
    std::string message;
    std::string timestamp;
};

class DevToolsEngine {
public:
    DevToolsEngine();
    ~DevToolsEngine() = default;

    // Network Inspector
    void logNetworkRequest(const std::string& url, const std::string& method, int statusCode, const std::string& mimeType, uint64_t sizeBytes, double durationMs);
    std::vector<NetworkLogEntry> getNetworkLogs() const;
    void clearNetworkLogs();

    // Console Runtime
    void logConsole(const std::string& level, const std::string& message);
    std::string evaluateJs(const std::string& jsCode, const std::string& pageUrl = "");
    std::vector<ConsoleLogEntry> getConsoleLogs() const;
    void clearConsoleLogs();

    // Elements DOM Tree
    std::string inspectDom(const std::string& pageUrl, const std::string& htmlContent) const;

    // JSON export
    std::string exportNetworkJson() const;
    std::string exportConsoleJson() const;

private:
    std::vector<NetworkLogEntry> m_networkLogs;
    std::vector<ConsoleLogEntry> m_consoleLogs;
    uint32_t m_nextReqId = 1;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
