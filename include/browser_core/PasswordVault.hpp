#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct VaultEntry {
    uint32_t id;
    std::string website;
    std::string username;
    std::string password; // Stored encrypted in memory / disk
    int strengthScore;    // 0 to 100
    bool isBreached;
    std::string createdAt;
};

class PasswordVault {
public:
    PasswordVault();
    ~PasswordVault() = default;

    bool isLocked() const { return m_isLocked; }
    bool unlockVault(const std::string& masterPassword);
    void lockVault();

    uint32_t addEntry(const std::string& website, const std::string& username, const std::string& password);
    bool deleteEntry(uint32_t id);
    std::vector<VaultEntry> getEntries() const;

    // Generators & Analyzers
    static std::string generateStrongPassword(int length = 16, bool useSymbols = true);
    static int evaluatePasswordStrength(const std::string& password);
    static bool checkBreachStatus(const std::string& emailOrPass);

    std::string exportVaultJson() const;

private:
    std::vector<VaultEntry> m_entries;
    bool m_isLocked = false;
    std::string m_masterPasswordHash;
    uint32_t m_nextId = 1;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
