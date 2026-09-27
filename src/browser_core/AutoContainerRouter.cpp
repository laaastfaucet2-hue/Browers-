#include "../../include/browser_core/AutoContainerRouter.hpp"
#include <algorithm>
#include <sstream>

namespace BrowserCore {

AutoContainerRouter::AutoContainerRouter() {
    initDefaultRules();
}

std::string AutoContainerRouter::extractHost(const std::string& url) {
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
    std::transform(host.begin(), host.end(), host.begin(), [](unsigned char c){ return std::tolower(c); });
    return host;
}

void AutoContainerRouter::initDefaultRules() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_rules.clear();

    // Work Container (ID 2, #fb923c)
    m_rules.push_back({"github.com", 2, "العمل (Work)", "#fb923c"});
    m_rules.push_back({"gitlab.com", 2, "العمل (Work)", "#fb923c"});
    m_rules.push_back({"jira.com", 2, "العمل (Work)", "#fb923c"});
    m_rules.push_back({"atlassian.net", 2, "العمل (Work)", "#fb923c"});
    m_rules.push_back({"slack.com", 2, "العمل (Work)", "#fb923c"});
    m_rules.push_back({"trello.com", 2, "العمل (Work)", "#fb923c"});
    m_rules.push_back({"stackoverflow.com", 2, "العمل (Work)", "#fb923c"});
    m_rules.push_back({"linkedin.com", 2, "العمل (Work)", "#fb923c"});

    // Banking Container (ID 3, #4ade80)
    m_rules.push_back({"paypal.com", 3, "البنوك والمعاملات (Banking)", "#4ade80"});
    m_rules.push_back({"stripe.com", 3, "البنوك والمعاملات (Banking)", "#4ade80"});
    m_rules.push_back({"binance.com", 3, "البنوك والمعاملات (Banking)", "#4ade80"});
    m_rules.push_back({"wise.com", 3, "البنوك والمعاملات (Banking)", "#4ade80"});
    m_rules.push_back({"chase.com", 3, "البنوك والمعاملات (Banking)", "#4ade80"});
    m_rules.push_back({"hsbc.com", 3, "البنوك والمعاملات (Banking)", "#4ade80"});

    // Shopping Container (ID 4, #f472b6)
    m_rules.push_back({"amazon.com", 4, "التسوق (Shopping)", "#f472b6"});
    m_rules.push_back({"ebay.com", 4, "التسوق (Shopping)", "#f472b6"});
    m_rules.push_back({"aliexpress.com", 4, "التسوق (Shopping)", "#f472b6"});
    m_rules.push_back({"noon.com", 4, "التسوق (Shopping)", "#f472b6"});
    m_rules.push_back({"jumia.com", 4, "التسوق (Shopping)", "#f472b6"});
    m_rules.push_back({"shopify.com", 4, "التسوق (Shopping)", "#f472b6"});

    // Personal Container (ID 1, #38bdf8)
    m_rules.push_back({"facebook.com", 1, "شخصي (Personal)", "#38bdf8"});
    m_rules.push_back({"twitter.com", 1, "شخصي (Personal)", "#38bdf8"});
    m_rules.push_back({"x.com", 1, "شخصي (Personal)", "#38bdf8"});
    m_rules.push_back({"instagram.com", 1, "شخصي (Personal)", "#38bdf8"});
    m_rules.push_back({"reddit.com", 1, "شخصي (Personal)", "#38bdf8"});
}

void AutoContainerRouter::addRule(const std::string& domainPattern, uint32_t containerId, const std::string& name, const std::string& color) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string clean = domainPattern;
    std::transform(clean.begin(), clean.end(), clean.begin(), [](unsigned char c){ return std::tolower(c); });
    m_rules.push_back({clean, containerId, name, color});
}

bool AutoContainerRouter::removeRule(const std::string& domainPattern) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::remove_if(m_rules.begin(), m_rules.end(), [&](const ContainerRule& r){
        return r.domainPattern == domainPattern;
    });
    if (it != m_rules.end()) {
        m_rules.erase(it, m_rules.end());
        return true;
    }
    return false;
}

uint32_t AutoContainerRouter::matchContainer(const std::string& url, std::string* outName, std::string* outColor) const {
    std::string host = extractHost(url);
    if (host.empty()) return 0;

    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& rule : m_rules) {
        if (host == rule.domainPattern ||
            (host.size() > rule.domainPattern.size() &&
             host.rfind("." + rule.domainPattern) == (host.size() - rule.domainPattern.size() - 1))) {
            
            if (outName) *outName = rule.containerName;
            if (outColor) *outColor = rule.containerColor;
            return rule.targetContainerId;
        }
    }
    return 0;
}

std::vector<ContainerRule> AutoContainerRouter::getAllRules() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_rules;
}

std::string AutoContainerRouter::exportRulesJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_rules.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "  {\"domain\": \"" << m_rules[i].domainPattern
           << "\", \"containerId\": " << m_rules[i].targetContainerId
           << ", \"containerName\": \"" << m_rules[i].containerName << "\""
           << ", \"containerColor\": \"" << m_rules[i].containerColor << "\"}";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
