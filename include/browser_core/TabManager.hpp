#pragma once

#include "Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <mutex>

namespace BrowserCore {

class TabManager {
public:
    TabManager();
    ~TabManager() = default;

    // Tab operations
    uint32_t createTab(const std::string& initialUrl = "mybrowser://newtab", uint32_t containerId = 0, const std::string& containerName = "Default", const std::string& containerColor = "#94a3b8", uint32_t workspaceId = 1);
    bool closeTab(uint32_t tabId);
    bool switchTab(uint32_t tabId);
    bool navigateTab(uint32_t tabId, const std::string& url);
    bool goBack(uint32_t tabId);
    bool goForward(uint32_t tabId);
    bool reloadTab(uint32_t tabId);

    // Tab state & container & workspace
    bool setTabContainer(uint32_t tabId, uint32_t containerId, const std::string& containerName, const std::string& containerColor);
    bool setTabWorkspace(uint32_t tabId, uint32_t workspaceId);
    bool setPinned(uint32_t tabId, bool pinned);
    bool setMuted(uint32_t tabId, bool muted);
    bool discardTab(uint32_t tabId); // Memory Saver
    bool restoreDiscardedTab(uint32_t tabId);

    // Split View
    SplitViewState getSplitView() const;
    void setSplitView(bool enabled, uint32_t secondaryTabId = 0);

    // Getters
    TabInfo* getActiveTab();
    const TabInfo* getActiveTab() const;
    TabInfo* getTab(uint32_t tabId);
    const TabInfo* getTab(uint32_t tabId) const;
    std::vector<TabInfo> getAllTabs() const;
    std::vector<TabInfo> getTabsInWorkspace(uint32_t workspaceId) const;
    size_t getTabCount() const;
    uint32_t getActiveTabId() const;

    // Callbacks
    using TabCallback = std::function<void(uint32_t tabId, const std::string& event)>;
    void setTabListener(TabCallback listener);

private:
    std::vector<std::shared_ptr<TabInfo>> m_tabs;
    uint32_t m_activeTabId = 0;
    uint32_t m_nextTabId = 1;
    SplitViewState m_splitView;
    mutable std::mutex m_mutex;
    TabCallback m_listener;
};

} // namespace BrowserCore
