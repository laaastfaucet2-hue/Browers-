#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct ContainerRule {
    std::string domainPattern;
    uint32_t targetContainerId;
    std::string containerName;
    std::string containerColor;
};

class AutoContainerRouter {
public:
    AutoContainerRouter();
    ~AutoContainerRouter() = default;

    void initDefaultRules();

    // Add or remove rules
    void addRule(const std::string& domainPattern, uint32_t containerId, const std::string& name, const std::string& color);
    bool removeRule(const std::string& domainPattern);

    // Evaluate URL and return target container ID (0 if no rule matches)
    uint32_t matchContainer(const std::string& url, std::string* outName = nullptr, std::string* outColor = nullptr) const;

    std::vector<ContainerRule> getAllRules() const;
    std::string exportRulesJson() const;

private:
    static std::string extractHost(const std::string& url);

    std::vector<ContainerRule> m_rules;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
