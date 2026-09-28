#pragma once

#include "VirtualDeviceProfile.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <string>

namespace BrowserCore {

class ProfileManager {
public:
    explicit ProfileManager(size_t initialCount = 200);
    ~ProfileManager() = default;

    void initPool(size_t count);

    size_t getProfileCount() const;
    const VirtualDeviceProfile* getProfile(uint32_t id) const;
    VirtualDeviceProfile* getActiveProfile();
    bool switchActiveProfile(uint32_t id);

    uint32_t createCustomProfile(const std::string& name, const std::string& osType, int cpuCores, int ramGb);

    std::vector<VirtualDeviceProfile> getProfiles(size_t offset = 0, size_t limit = 20, const std::string& query = "") const;

    std::string getActiveAntiDetectScript() const;
    std::string exportProfilesJson(size_t page = 1, size_t limit = 20, const std::string& query = "") const;

    // Generates a CreepJS / BrowserLeaks style live diagnostic page
    std::string renderFingerprintTestHtml() const;

private:
    static VirtualDeviceProfile generateDeterministicProfile(uint32_t id);

    std::vector<VirtualDeviceProfile> m_profiles;
    uint32_t m_activeProfileId = 1;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
