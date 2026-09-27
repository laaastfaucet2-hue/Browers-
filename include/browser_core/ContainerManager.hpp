#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct ContainerIdentity {
    uint32_t id = 0;
    std::string name;
    std::string color;
    std::string icon; // e.g. "briefcase", "user", "dollar", "cart"
    std::map<std::string, std::map<std::string, std::string>> cookieJar; // domain -> (cookie_name -> value)
};

class ContainerManager {
public:
    ContainerManager();
    ~ContainerManager() = default;

    // Built-in containers setup (Firefox Multi-Account Containers model)
    void initDefaultContainers();

    // Container CRUD
    uint32_t createContainer(const std::string& name, const std::string& color, const std::string& icon = "circle");
    bool removeContainer(uint32_t id);
    const ContainerIdentity* getContainer(uint32_t id) const;
    std::vector<ContainerIdentity> getAllContainers() const;

    // Cookie isolation between containers
    void setCookie(uint32_t containerId, const std::string& domain, const std::string& name, const std::string& value);
    std::string getCookie(uint32_t containerId, const std::string& domain, const std::string& name) const;
    std::map<std::string, std::string> getAllCookies(uint32_t containerId, const std::string& domain) const;
    void clearContainerData(uint32_t containerId);

    // Serialization for UI / JS Bridge
    std::string exportContainersJson() const;

private:
    std::vector<ContainerIdentity> m_containers;
    uint32_t m_nextId = 1;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
