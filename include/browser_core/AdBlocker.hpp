#pragma once

#include "Types.hpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <regex>
#include <mutex>
#include <atomic>

namespace BrowserCore {

enum class BlockCategory {
    Advertisement,
    Tracker,
    Analytics,
    Malware,
    SocialWidget,
    Custom
};

struct FilterRule {
    std::string pattern;
    bool isWhitelist = false;
    BlockCategory category = BlockCategory::Advertisement;
    std::string regexPattern;
    std::unique_ptr<std::regex> compiledRegex;
};

struct AdBlockStats {
    uint64_t totalBlocked = 0;
    uint64_t adsBlocked = 0;
    uint64_t trackersBlocked = 0;
    uint64_t analyticsBlocked = 0;
    uint64_t socialBlocked = 0;
    uint64_t malwareBlocked = 0;
    uint64_t requestsChecked = 0;
};

class AdBlocker {
public:
    AdBlocker();
    ~AdBlocker() = default;

    // Load filter rules
    void addRule(const std::string& pattern, BlockCategory category = BlockCategory::Advertisement, bool isWhitelist = false);
    void addDomainRule(const std::string& domain, BlockCategory category = BlockCategory::Advertisement);
    void addWhitelistDomain(const std::string& domain);
    size_t loadRulesFromEasyList(const std::string& filePathOrContent, bool isContent = false);

    // Matching
    bool shouldBlock(const HttpRequest& request, BlockCategory* outCategory = nullptr, std::string* outMatchedRule = nullptr);

    // Configuration & Stats
    void setEnabled(bool enabled);
    bool isEnabled() const;
    AdBlockStats getStats() const;
    void resetStats();

    size_t getRuleCount() const;

private:
    void initDefaultRules();
    static std::string extractHost(const std::string& url);

    bool m_enabled = true;
    std::vector<FilterRule> m_rules;
    std::unordered_map<std::string, BlockCategory> m_blockedDomains;
    std::unordered_set<std::string> m_whitelistedDomains;
    mutable std::mutex m_mutex;

    // Thread-safe statistics
    std::atomic<uint64_t> m_totalBlocked{0};
    std::atomic<uint64_t> m_adsBlocked{0};
    std::atomic<uint64_t> m_trackersBlocked{0};
    std::atomic<uint64_t> m_analyticsBlocked{0};
    std::atomic<uint64_t> m_socialBlocked{0};
    std::atomic<uint64_t> m_malwareBlocked{0};
    std::atomic<uint64_t> m_requestsChecked{0};
};

} // namespace BrowserCore
