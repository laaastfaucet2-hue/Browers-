#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct TabGroup {
    uint32_t id;
    std::string title;
    std::string color;
    bool isCollapsed;
    std::vector<uint32_t> tabIds;
};

class TabGroupManager {
public:
    TabGroupManager();
    ~TabGroupManager() = default;

    uint32_t createGroup(const std::string& title, const std::string& color = "#38bdf8");
    bool addTabToGroup(uint32_t groupId, uint32_t tabId);
    bool removeTabFromGroup(uint32_t groupId, uint32_t tabId);
    bool toggleGroupCollapse(uint32_t groupId);
    bool deleteGroup(uint32_t groupId);

    std::vector<TabGroup> getAllGroups() const;
    const TabGroup* getGroup(uint32_t groupId) const;

    // Auto-grouping by domain
    void autoGroupByDomain(const std::vector<std::pair<uint32_t, std::string>>& tabUrls);

    std::string exportGroupsJson() const;

private:
    std::vector<TabGroup> m_groups;
    uint32_t m_nextId = 1;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
