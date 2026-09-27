#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>

/**
 * AtlasContainerHook - C++ Native Module for Gecko Engine
 * Hooks directly into Gecko's OriginAttributes and Necko (nsIChannel)
 * to provide fine-grained cookie and network partition per container.
 */
namespace AtlasGecko {

struct ContainerProfile {
    uint32_t userContextId;
    std::string name;
    std::string color;
    std::string icon;
    bool enableStrictIsolation;
};

class AtlasContainerManager {
public:
    static AtlasContainerManager& getInstance() {
        static AtlasContainerManager instance;
        return instance;
    }

    void registerContainer(uint32_t id, const std::string& name, const std::string& color, const std::string& icon) {
        ContainerProfile p{id, name, color, icon, true};
        m_profiles[id] = p;
    }

    // Called when Gecko creates an OriginAttributes suffix (^userContextId=X)
    std::string formatOriginSuffix(uint32_t userContextId) {
        if (userContextId == 0) return "";
        return "^userContextId=" + std::to_string(userContextId);
    }

    bool isContainerRegistered(uint32_t id) const {
        return m_profiles.find(id) != m_profiles.end();
    }

private:
    AtlasContainerManager() {
        registerContainer(1, "Personal", "#38bdf8", "fingerprint");
        registerContainer(2, "Work", "#fb923c", "briefcase");
        registerContainer(3, "Banking", "#4ade80", "dollar");
        registerContainer(4, "Shopping", "#f472b6", "cart");
    }
    std::map<uint32_t, ContainerProfile> m_profiles;
};

} // namespace AtlasGecko
