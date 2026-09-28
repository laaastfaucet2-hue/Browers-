#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <cstdint>

namespace BrowserCore {

struct ProfileCookie {
    std::string domain;
    std::string name;
    std::string value;
    std::string path = "/";
    bool httpOnly = false;
    bool secure = true;
    int64_t expiresAt = 0; // 0 = session cookie
    bool isModified = false;
};

struct ProfileStorageItem {
    std::string domain;
    std::string key;
    std::string value;
    int64_t updatedAt = 0;
    size_t sizeBytes = 0;
};

enum class ProfileStorageState {
    ACTIVE,
    IDLE,
    HIBERNATED
};

struct ProfileStorageStats {
    uint32_t profileId = 0;
    ProfileStorageState state = ProfileStorageState::ACTIVE;
    size_t totalCookies = 0;
    size_t totalStorageKeys = 0;
    size_t activeRamBytes = 0;
    size_t diskDeltaBytes = 0;
    size_t standardBrowserBytes = 262144000; // ~250 MB
    double compressionRatio = 4.6;
    size_t savedBytes = 0;
    uint64_t readsCount = 0;
    uint64_t writesCount = 0;
    double lastAccessAgoSec = 0.0;
};

struct GlobalStorageMetrics {
    size_t totalProfilesCount = 200;
    size_t activeProfilesCount = 0;
    size_t hibernatedProfilesCount = 0;
    size_t totalActiveRamBytes = 0;
    size_t totalDiskDeltaBytes = 0;
    size_t theoreticalStandardDiskBytes = 0;
    size_t theoreticalStandardRamBytes = 0;
    size_t totalDiskSavedBytes = 0;
    size_t totalRamSavedBytes = 0;
    double averageCompressionRatio = 4.8;
    uint64_t totalIopsOperations = 0;
};

class UltraStorageBox {
public:
    explicit UltraStorageBox(uint32_t profileId);

    uint32_t getId() const { return m_profileId; }
    ProfileStorageState getState() const { return m_state; }

    // Cookies
    void setCookie(const ProfileCookie& cookie);
    std::vector<ProfileCookie> getCookies(const std::string& domain);
    bool deleteCookie(const std::string& domain, const std::string& name);
    void clearCookies();

    // LocalStorage / Key-Value
    void setItem(const std::string& domain, const std::string& key, const std::string& value);
    std::string getItem(const std::string& domain, const std::string& key);
    bool removeItem(const std::string& domain, const std::string& key);
    std::vector<ProfileStorageItem> getAllItems(const std::string& domain);
    void clearStorage();

    // Lifecycle: Hibernation, Wakeup, Compaction
    bool hibernate();
    bool wake();
    void compact();

    // Stats
    ProfileStorageStats getStats() const;
    std::string exportJson() const;

private:
    void touchAccess();
    std::string encryptPayload(const std::string& plain) const;
    std::string decryptPayload(const std::string& cipher) const;

    uint32_t m_profileId;
    ProfileStorageState m_state = ProfileStorageState::ACTIVE;
    std::string m_encryptionKey;
    std::chrono::steady_clock::time_point m_lastAccess;

    // Hot In-Memory Data (Map: domain -> (name -> Cookie))
    std::unordered_map<std::string, std::unordered_map<std::string, ProfileCookie>> m_cookies;
    // Map: domain -> (key -> Item)
    std::unordered_map<std::string, std::unordered_map<std::string, ProfileStorageItem>> m_storage;

    // Hibernation Compressed Payload (Zero-RAM snapshot)
    std::string m_hibernatedBlob;

    uint64_t m_reads = 0;
    uint64_t m_writes = 0;
    mutable std::mutex m_mutex;
};

class UltraStorageEngine {
public:
    explicit UltraStorageEngine(size_t maxProfiles = 200);
    ~UltraStorageEngine() = default;

    // Profile Management
    std::shared_ptr<UltraStorageBox> getBox(uint32_t profileId);
    bool hasBox(uint32_t profileId) const;

    // Direct Operations
    void setCookie(uint32_t profileId, const std::string& domain, const std::string& name,
                   const std::string& value, const std::string& path = "/", bool httpOnly = false,
                   bool secure = true, int64_t maxAgeSeconds = 86400 * 30);
    std::vector<ProfileCookie> getCookies(uint32_t profileId, const std::string& domain);
    bool deleteCookie(uint32_t profileId, const std::string& domain, const std::string& name);

    void setItem(uint32_t profileId, const std::string& domain, const std::string& key, const std::string& value);
    std::string getItem(uint32_t profileId, const std::string& domain, const std::string& key);
    bool removeItem(uint32_t profileId, const std::string& domain, const std::string& key);

    // Ultra Efficiency Lifecycle Controls
    bool hibernateProfile(uint32_t profileId);
    bool wakeProfile(uint32_t profileId);
    size_t hibernateAllInactive(uint32_t exceptProfileId);
    void compactProfile(uint32_t profileId);
    void compactAll();

    // Global Metrics
    GlobalStorageMetrics calculateGlobalMetrics() const;
    std::string exportGlobalMetricsJson() const;
    std::string exportProfileStorageJson(uint32_t profileId);
    std::string generateStorageDiagnosticsHtml();

private:
    std::unordered_map<uint32_t, std::shared_ptr<UltraStorageBox>> m_boxes;
    size_t m_maxProfiles;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
