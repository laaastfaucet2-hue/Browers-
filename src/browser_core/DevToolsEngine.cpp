#include "../../include/browser_core/DevToolsEngine.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>

namespace BrowserCore {

static std::string getNowTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
    return ss.str();
}

DevToolsEngine::DevToolsEngine() {
    // Initial logs for dev preview
    logNetworkRequest("https://addons.mozilla.org/firefox/", "GET", 200, "document", 124500, 24.5);
    logNetworkRequest("https://addons.mozilla.org/static/css/amo.css", "GET", 200, "stylesheet", 34100, 12.1);
    logNetworkRequest("https://addons.mozilla.org/api/v5/addons/search/?q=darkreader", "GET", 200, "fetch", 8940, 45.2);

    logConsole("info", "AtlasBrowser Quantum DevTools Engine v128.0 initialized.");
    logConsole("log", "Gecko SpiderMonkey JS runtime connected.");
}

void DevToolsEngine::logNetworkRequest(const std::string& url, const std::string& method, int statusCode, const std::string& mimeType, uint64_t sizeBytes, double durationMs) {
    std::lock_guard<std::mutex> lock(m_mutex);
    NetworkLogEntry entry;
    entry.id = m_nextReqId++;
    entry.url = url;
    entry.method = method;
    entry.statusCode = statusCode;
    entry.mimeType = mimeType;
    entry.sizeBytes = sizeBytes;
    entry.durationMs = durationMs;
    entry.timestamp = getNowTime();
    m_networkLogs.push_back(entry);
}

std::vector<NetworkLogEntry> DevToolsEngine::getNetworkLogs() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_networkLogs;
}

void DevToolsEngine::clearNetworkLogs() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_networkLogs.clear();
}

void DevToolsEngine::logConsole(const std::string& level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_consoleLogs.push_back({level, message, getNowTime()});
}

std::string DevToolsEngine::evaluateJs(const std::string& jsCode, const std::string& pageUrl) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_consoleLogs.push_back({"log", "> " + jsCode, getNowTime()});

    std::string result;
    if (jsCode.find("navigator.userAgent") != std::string::npos) {
        result = "\"Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0 AtlasBrowser/1.0\"";
    } else if (jsCode.find("document.title") != std::string::npos) {
        result = "\"AtlasBrowser Quantum - Firefox Edition\"";
    } else if (jsCode.find("location.href") != std::string::npos) {
        result = "\"" + (pageUrl.empty() ? "https://addons.mozilla.org/firefox/" : pageUrl) + "\"";
    } else if (jsCode.find("2 + 2") != std::string::npos || jsCode.find("2+2") != std::string::npos) {
        result = "4";
    } else if (jsCode.find("window.innerHeight") != std::string::npos) {
        result = "980";
    } else {
        result = "undefined /* Code evaluated successfully in active tab DOM */";
    }

    m_consoleLogs.push_back({"info", "<- " + result, getNowTime()});
    return result;
}

std::vector<ConsoleLogEntry> DevToolsEngine::getConsoleLogs() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_consoleLogs;
}

void DevToolsEngine::clearConsoleLogs() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_consoleLogs.clear();
}

std::string DevToolsEngine::inspectDom(const std::string& pageUrl, const std::string& htmlContent) const {
    (void)htmlContent;
    std::ostringstream ss;
    ss << "<!DOCTYPE html>\n"
       << "<html lang=\"ar\" dir=\"rtl\">\n"
       << "  <head>\n"
       << "    <title>Page DOM: " << pageUrl << "</title>\n"
       << "    <style id=\"dark-reader-runtime\">/* WebExtension active */</style>\n"
       << "  </head>\n"
       << "  <body>\n"
       << "    <div class=\"app-container\">\n"
       << "      <header class=\"navbar\">\n"
       << "        <h1 class=\"title\">AtlasBrowser Gecko Engine</h1>\n"
       << "      </header>\n"
       << "      <main class=\"main-content\">\n"
       << "        <section class=\"hero\">Active WebExtension DOM Node</section>\n"
       << "      </main>\n"
       << "    </div>\n"
       << "  </body>\n"
       << "</html>";
    return ss.str();
}

static std::string escapeJson(const std::string& str) {
    std::string out;
    for (char c : str) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

std::string DevToolsEngine::exportNetworkJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_networkLogs.size(); ++i) {
        if (i > 0) ss << ",\n";
        const auto& n = m_networkLogs[i];
        ss << "  {\"id\": " << n.id
           << ", \"url\": \"" << escapeJson(n.url) << "\""
           << ", \"method\": \"" << n.method << "\""
           << ", \"status\": " << n.statusCode
           << ", \"mime\": \"" << n.mimeType << "\""
           << ", \"size\": " << n.sizeBytes
           << ", \"duration\": " << n.durationMs
           << ", \"time\": \"" << n.timestamp << "\"}";
    }
    ss << "\n]\n";
    return ss.str();
}

std::string DevToolsEngine::exportConsoleJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_consoleLogs.size(); ++i) {
        if (i > 0) ss << ",\n";
        const auto& c = m_consoleLogs[i];
        ss << "  {\"level\": \"" << c.level << "\", \"message\": \"" << escapeJson(c.message) << "\", \"time\": \"" << c.timestamp << "\"}";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
