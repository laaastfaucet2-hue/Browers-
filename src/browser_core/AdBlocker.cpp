#include "../../include/browser_core/AdBlocker.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace BrowserCore {

AdBlocker::AdBlocker() {
    initDefaultRules();
}

void AdBlocker::initDefaultRules() {
    // Common ad & tracking networks
    const std::vector<std::string> defaultAdDomains = {
        "doubleclick.net",
        "googleadservices.com",
        "googlesyndication.com",
        "adnxs.com",
        "advertising.com",
        "criteo.com",
        "outbrain.com",
        "taboola.com",
        "adroll.com",
        "popads.net",
        "propellerads.com",
        "adform.net",
        "rubiconproject.com",
        "pubmatic.com",
        "openx.net"
    };

    for (const auto& domain : defaultAdDomains) {
        addDomainRule(domain, BlockCategory::Advertisement);
    }

    // Common trackers & analytics
    const std::vector<std::string> defaultTrackerDomains = {
        "google-analytics.com",
        "analytics.google.com",
        "hotjar.com",
        "segment.io",
        "mixpanel.com",
        "amplitude.com",
        "quantserve.com",
        "scorecardresearch.com",
        "statcounter.com",
        "facebook.net",
        "connect.facebook.net",
        "clarity.ms",
        "telemetry.mozilla.org",
        "metrics.icloud.com"
    };

    for (const auto& domain : defaultTrackerDomains) {
        addDomainRule(domain, BlockCategory::Tracker);
    }

    // Regex / URL keyword rules
    addRule("/ads/banner/*", BlockCategory::Advertisement);
    addRule("*/pagead/*", BlockCategory::Advertisement);
    addRule("*/adserver/*", BlockCategory::Advertisement);
    addRule("*/telemetry/*", BlockCategory::Tracker);
    addRule("*/analytics.js", BlockCategory::Analytics);
}

std::string AdBlocker::extractHost(const std::string& url) {
    std::string host = url;
    size_t protocolPos = host.find("://");
    if (protocolPos != std::string::npos) {
        host = host.substr(protocolPos + 3);
    }
    size_t slashPos = host.find('/');
    if (slashPos != std::string::npos) {
        host = host.substr(0, slashPos);
    }
    size_t colonPos = host.find(':');
    if (colonPos != std::string::npos) {
        host = host.substr(0, colonPos);
    }
    // to lower case
    std::transform(host.begin(), host.end(), host.begin(), [](unsigned char c){ return std::tolower(c); });
    return host;
}

void AdBlocker::addDomainRule(const std::string& domain, BlockCategory category) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string cleanDomain = domain;
    std::transform(cleanDomain.begin(), cleanDomain.end(), cleanDomain.begin(), [](unsigned char c){ return std::tolower(c); });
    m_blockedDomains[cleanDomain] = category;

    // Also add to rules list
    FilterRule rule;
    rule.pattern = "||" + cleanDomain + "^";
    rule.isWhitelist = false;
    rule.category = category;
    m_rules.push_back(std::move(rule));
}

void AdBlocker::addWhitelistDomain(const std::string& domain) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string clean = domain;
    std::transform(clean.begin(), clean.end(), clean.begin(), [](unsigned char c){ return std::tolower(c); });
    m_whitelistedDomains.insert(clean);
}

void AdBlocker::addRule(const std::string& pattern, BlockCategory category, bool isWhitelist) {
    std::lock_guard<std::mutex> lock(m_mutex);
    FilterRule rule;
    rule.pattern = pattern;
    rule.isWhitelist = isWhitelist;
    rule.category = category;

    // Convert simple wildcard to regex
    std::string regexStr = "^";
    for (char c : pattern) {
        if (c == '*') {
            regexStr += ".*";
        } else if (c == '.' || c == '?' || c == '+' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '^' || c == '$' || c == '|') {
            regexStr += '\\';
            regexStr += c;
        } else {
            regexStr += c;
        }
    }
    regexStr += "$";
    rule.regexPattern = regexStr;

    try {
        rule.compiledRegex = std::make_unique<std::regex>(regexStr, std::regex::optimize | std::regex::icase);
    } catch (...) {
        rule.compiledRegex = nullptr;
    }

    m_rules.push_back(std::move(rule));
}

size_t AdBlocker::loadRulesFromEasyList(const std::string& filePathOrContent, bool isContent) {
    std::istream* stream = nullptr;
    std::ifstream file;
    std::istringstream contentStream;

    if (isContent) {
        contentStream.str(filePathOrContent);
        stream = &contentStream;
    } else {
        file.open(filePathOrContent);
        if (!file.is_open()) return 0;
        stream = &file;
    }

    size_t count = 0;
    std::string line;
    while (std::getline(*stream, line)) {
        // Strip whitespace and carriage return
        while (!line.empty() && (line.back() == '\r' || line.back() == ' ' || line.back() == '\t')) {
            line.pop_back();
        }
        if (line.empty() || line[0] == '!' || line[0] == '[') {
            continue; // Comment or header
        }

        // Whitelist syntax: @@...
        bool isWhitelist = false;
        if (line.rfind("@@", 0) == 0) {
            isWhitelist = true;
            line = line.substr(2);
        }

        // Domain rule: ||domain.com^
        if (line.rfind("||", 0) == 0) {
            size_t endPos = line.find('^');
            std::string domain = (endPos != std::string::npos) ? line.substr(2, endPos - 2) : line.substr(2);
            if (isWhitelist) {
                addWhitelistDomain(domain);
            } else {
                addDomainRule(domain, BlockCategory::Advertisement);
            }
            count++;
        } else if (line.find("##") != std::string::npos || line.find("#?#") != std::string::npos) {
            // Cosmetic filter - ignore in network blocking
            continue;
        } else {
            // General pattern rule
            addRule(line, BlockCategory::Advertisement, isWhitelist);
            count++;
        }
    }

    return count;
}

bool AdBlocker::shouldBlock(const HttpRequest& request, BlockCategory* outCategory, std::string* outMatchedRule) {
    m_requestsChecked++;
    if (!m_enabled) {
        return false;
    }

    std::string host = extractHost(request.url);

    std::lock_guard<std::mutex> lock(m_mutex);

    // Whitelist check
    if (m_whitelistedDomains.find(host) != m_whitelistedDomains.end()) {
        return false;
    }

    // Direct domain match or subdomain match
    for (const auto& [blocked, cat] : m_blockedDomains) {
        if (host == blocked || (host.size() > blocked.size() && 
            host.rfind("." + blocked) == (host.size() - blocked.size() - 1))) {
            
            m_totalBlocked++;
            switch (cat) {
                case BlockCategory::Advertisement: m_adsBlocked++; break;
                case BlockCategory::Tracker: m_trackersBlocked++; break;
                case BlockCategory::Analytics: m_analyticsBlocked++; break;
                case BlockCategory::SocialWidget: m_socialBlocked++; break;
                case BlockCategory::Malware: m_malwareBlocked++; break;
                default: m_adsBlocked++; break;
            }
            if (outCategory) *outCategory = cat;
            if (outMatchedRule) *outMatchedRule = "||" + blocked + "^";
            return true;
        }
    }

    // Pattern / Regex rules check
    for (const auto& rule : m_rules) {
        if (rule.compiledRegex) {
            if (std::regex_search(request.url, *rule.compiledRegex)) {
                if (rule.isWhitelist) {
                    return false;
                }
                m_totalBlocked++;
                switch (rule.category) {
                    case BlockCategory::Advertisement: m_adsBlocked++; break;
                    case BlockCategory::Tracker: m_trackersBlocked++; break;
                    case BlockCategory::Analytics: m_analyticsBlocked++; break;
                    case BlockCategory::SocialWidget: m_socialBlocked++; break;
                    case BlockCategory::Malware: m_malwareBlocked++; break;
                    default: m_adsBlocked++; break;
                }
                if (outCategory) *outCategory = rule.category;
                if (outMatchedRule) *outMatchedRule = rule.pattern;
                return true;
            }
        }
    }

    return false;
}

void AdBlocker::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool AdBlocker::isEnabled() const {
    return m_enabled;
}

AdBlockStats AdBlocker::getStats() const {
    AdBlockStats stats;
    stats.totalBlocked = m_totalBlocked.load();
    stats.adsBlocked = m_adsBlocked.load();
    stats.trackersBlocked = m_trackersBlocked.load();
    stats.analyticsBlocked = m_analyticsBlocked.load();
    stats.socialBlocked = m_socialBlocked.load();
    stats.malwareBlocked = m_malwareBlocked.load();
    stats.requestsChecked = m_requestsChecked.load();
    return stats;
}

void AdBlocker::resetStats() {
    m_totalBlocked = 0;
    m_adsBlocked = 0;
    m_trackersBlocked = 0;
    m_analyticsBlocked = 0;
    m_socialBlocked = 0;
    m_malwareBlocked = 0;
    m_requestsChecked = 0;
}

size_t AdBlocker::getRuleCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_rules.size() + m_blockedDomains.size();
}

} // namespace BrowserCore
