#include "../../include/browser_core/WorkspaceManager.hpp"
#include <sstream>
#include <algorithm>

namespace BrowserCore {

WorkspaceManager::WorkspaceManager() {
    initDefaultWorkspaces();
}

void WorkspaceManager::initDefaultWorkspaces() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_workspaces.clear();

    m_workspaces.push_back({1, "العمل والتطوير (Dev)", "💻", "#fb923c", 2});
    m_workspaces.push_back({2, "التصفح الشخصي (Personal)", "☕", "#38bdf8", 1});
    m_workspaces.push_back({3, "المالية والتداول (Finance)", "📈", "#4ade80", 3});
    m_workspaces.push_back({4, "البحث والدراسة (Research)", "🎓", "#a855f7", 0});

    m_activeWorkspaceId = 1;
    m_nextId = 5;
}

uint32_t WorkspaceManager::createWorkspace(const std::string& name, const std::string& icon, const std::string& themeColor, uint32_t defaultContainerId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    Workspace ws;
    ws.id = m_nextId++;
    ws.name = name;
    ws.icon = icon.empty() ? "📁" : icon;
    ws.themeColor = themeColor.empty() ? "#38bdf8" : themeColor;
    ws.defaultContainerId = defaultContainerId;
    m_workspaces.push_back(ws);
    return ws.id;
}

bool WorkspaceManager::removeWorkspace(uint32_t id) {
    if (id == 1) return false; // Cannot remove default primary workspace
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::remove_if(m_workspaces.begin(), m_workspaces.end(), [id](const Workspace& ws) {
        return ws.id == id;
    });
    if (it != m_workspaces.end()) {
        m_workspaces.erase(it, m_workspaces.end());
        if (m_activeWorkspaceId == id) {
            m_activeWorkspaceId = 1;
        }
        return true;
    }
    return false;
}

bool WorkspaceManager::switchWorkspace(uint32_t id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& ws : m_workspaces) {
        if (ws.id == id) {
            m_activeWorkspaceId = id;
            return true;
        }
    }
    return false;
}

uint32_t WorkspaceManager::getActiveWorkspaceId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_activeWorkspaceId;
}

const Workspace* WorkspaceManager::getWorkspace(uint32_t id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& ws : m_workspaces) {
        if (ws.id == id) return &ws;
    }
    return nullptr;
}

const Workspace* WorkspaceManager::getActiveWorkspace() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& ws : m_workspaces) {
        if (ws.id == m_activeWorkspaceId) return &ws;
    }
    return nullptr;
}

std::vector<Workspace> WorkspaceManager::getAllWorkspaces() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_workspaces;
}

std::string WorkspaceManager::exportWorkspacesJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_workspaces.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "  {\"id\": " << m_workspaces[i].id
           << ", \"name\": \"" << m_workspaces[i].name << "\""
           << ", \"icon\": \"" << m_workspaces[i].icon << "\""
           << ", \"color\": \"" << m_workspaces[i].themeColor << "\""
           << ", \"defaultContainer\": " << m_workspaces[i].defaultContainerId << "}";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
