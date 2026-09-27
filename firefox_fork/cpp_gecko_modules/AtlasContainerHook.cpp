#include "AtlasContainerHook.hpp"
#include <iostream>

extern "C" {
    // Exported C symbols that Gecko XPCOM loader can link with
    void AtlasGecko_InitializeContainers() {
        AtlasGecko::AtlasContainerManager::getInstance();
        std::cout << "[AtlasGecko C++] Native Container Subsystem Hook initialized.\n";
    }

    const char* AtlasGecko_GetSuffix(uint32_t userContextId) {
        static std::string lastSuffix;
        lastSuffix = AtlasGecko::AtlasContainerManager::getInstance().formatOriginSuffix(userContextId);
        return lastSuffix.c_str();
    }
}
