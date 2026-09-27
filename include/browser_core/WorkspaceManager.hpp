#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct Workspace {
    uint32_t id = 1;
    std::string name;
    std::string icon;
    std::string themeColor;
    uint32_t defaultContainerId = 0;
};

class WorkspaceManager {
public:
    WorkspaceManager();
    ~WorkspaceManager() = default;

    void initDefaultWorkspaces();

    uint32_t createWorkspace(const std::string& name, const std::string& icon, const std::string& themeColor, uint32_t defaultContainerId = 0);
    bool removeWorkspace(uint32_t id);
    bool switchWorkspace(uint32_t id);
    uint32_t getActiveWorkspaceId() const;
    const Workspace* getWorkspace(uint32_t id) const;
    const Workspace* getActiveWorkspace() const;
    std::vector<Workspace> getAllWorkspaces() const;

    std::string exportWorkspacesJson() const;

private:
    std::vector<Workspace> m_workspaces;
    uint32_t m_activeWorkspaceId = 1;
    uint32_t m_nextId = 5;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
