#include "../../include/browser_core/TabGroupManager.hpp"
#include <sstream>
#include <algorithm>

namespace BrowserCore {

TabGroupManager::TabGroupManager() {
    uint32_t g1 = createGroup("مهام التطوير (Dev Tasks)", "#38bdf8");
    addTabToGroup(g1, 1);
    addTabToGroup(g1, 2);
}

uint32_t TabGroupManager::createGroup(const std::string& title, const std::string& color) {
    std::lock_guard<std::mutex> lock(m_mutex);
    TabGroup g;
    g.id = m_nextId++;
    g.title = title;
    g.color = color;
    g.isCollapsed = false;
    m_groups.push_back(g);
    return g.id;
}

bool TabGroupManager::addTabToGroup(uint32_t groupId, uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& g : m_groups) {
        if (g.id == groupId) {
            if (std::find(g.tabIds.begin(), g.tabIds.end(), tabId) == g.tabIds.end()) {
                g.tabIds.push_back(tabId);
            }
            return true;
        }
    }
    return false;
}

bool TabGroupManager::removeTabFromGroup(uint32_t groupId, uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& g : m_groups) {
        if (g.id == groupId) {
            auto it = std::find(g.tabIds.begin(), g.tabIds.end(), tabId);
            if (it != g.tabIds.end()) {
                g.tabIds.erase(it);
                return true;
            }
        }
    }
    return false;
}

bool TabGroupManager::toggleGroupCollapse(uint32_t groupId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& g : m_groups) {
        if (g.id == groupId) {
            g.isCollapsed = !g.isCollapsed;
            return true;
        }
    }
    return false;
}

bool TabGroupManager::deleteGroup(uint32_t groupId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_groups.begin(); it != m_groups.end(); ++it) {
        if (it->id == groupId) {
            m_groups.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<TabGroup> TabGroupManager::getAllGroups() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_groups;
}

const TabGroup* TabGroupManager::getGroup(uint32_t groupId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& g : m_groups) {
        if (g.id == groupId) return &g;
    }
    return nullptr;
}

void TabGroupManager::autoGroupByDomain(const std::vector<std::pair<uint32_t, std::string>>& tabUrls) {
    (void)tabUrls;
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

std::string TabGroupManager::exportGroupsJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_groups.size(); ++i) {
        if (i > 0) ss << ",\n";
        const auto& g = m_groups[i];
        ss << "  {\"id\": " << g.id
           << ", \"title\": \"" << escapeJson(g.title) << "\""
           << ", \"color\": \"" << g.color << "\""
           << ", \"isCollapsed\": " << (g.isCollapsed ? "true" : "false")
           << ", \"tabIds\": [";
        for (size_t j = 0; j < g.tabIds.size(); ++j) {
            if (j > 0) ss << ", ";
            ss << g.tabIds[j];
        }
        ss << "]}";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
