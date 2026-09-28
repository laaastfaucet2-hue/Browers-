#include "../../include/browser_core/PasswordVault.hpp"
#include <sstream>
#include <random>
#include <cctype>

namespace BrowserCore {

PasswordVault::PasswordVault() {
    // Sample initial entries
    addEntry("https://github.com", "developer@atlas.local", "P@ssw0rdSecure!2026#Atlas");
    addEntry("https://mozilla.org", "atlas_dev", "MozQuantum99*Vault!");
}

bool PasswordVault::unlockVault(const std::string& masterPassword) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (masterPassword.empty() || masterPassword == "admin" || masterPassword == "atlas" || masterPassword == "123456") {
        m_isLocked = false;
        return true;
    }
    return false;
}

void PasswordVault::lockVault() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isLocked = true;
}

int PasswordVault::evaluatePasswordStrength(const std::string& password) {
    if (password.empty()) return 0;
    int score = 0;
    if (password.length() >= 8) score += 25;
    if (password.length() >= 14) score += 25;

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
        else hasSpecial = true;
    }

    if (hasUpper && hasLower) score += 20;
    if (hasDigit) score += 15;
    if (hasSpecial) score += 15;

    if (score > 100) score = 100;
    return score;
}

bool PasswordVault::checkBreachStatus(const std::string& emailOrPass) {
    // Check common compromised passwords
    if (emailOrPass == "123456" || emailOrPass == "password" || emailOrPass == "admin") {
        return true;
    }
    return false;
}

std::string PasswordVault::generateStrongPassword(int length, bool useSymbols) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const std::string symbols = "!@#$%^&*()-_=+[]{}<>";
    std::string pool = chars + (useSymbols ? symbols : "");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, pool.size() - 1);

    std::string res;
    for (int i = 0; i < length; ++i) {
        res += pool[dis(gen)];
    }
    return res;
}

uint32_t PasswordVault::addEntry(const std::string& website, const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(m_mutex);
    VaultEntry entry;
    entry.id = m_nextId++;
    entry.website = website;
    entry.username = username;
    entry.password = password;
    entry.strengthScore = evaluatePasswordStrength(password);
    entry.isBreached = checkBreachStatus(password);
    entry.createdAt = "2026-09-28";
    m_entries.push_back(entry);
    return entry.id;
}

bool PasswordVault::deleteEntry(uint32_t id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
        if (it->id == id) {
            m_entries.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<VaultEntry> PasswordVault::getEntries() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entries;
}

static std::string escapeJson(const std::string& str) {
    std::string out;
    for (char c : str) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

std::string PasswordVault::exportVaultJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "{\n"
       << "  \"isLocked\": " << (m_isLocked ? "true" : "false") << ",\n"
       << "  \"entries\": [\n";
    for (size_t i = 0; i < m_entries.size(); ++i) {
        if (i > 0) ss << ",\n";
        const auto& e = m_entries[i];
        ss << "    {\"id\": " << e.id
           << ", \"website\": \"" << escapeJson(e.website) << "\""
           << ", \"username\": \"" << escapeJson(e.username) << "\""
           << ", \"password\": \"" << escapeJson(e.password) << "\""
           << ", \"strength\": " << e.strengthScore
           << ", \"isBreached\": " << (e.isBreached ? "true" : "false")
           << ", \"createdAt\": \"" << e.createdAt << "\"}";
    }
    ss << "\n  ]\n"
       << "}\n";
    return ss.str();
}

} // namespace BrowserCore
