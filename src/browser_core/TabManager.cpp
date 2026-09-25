#include "../../include/browser_core/TabManager.hpp"
#include <algorithm>

namespace BrowserCore {

TabManager::TabManager() {}

uint32_t TabManager::createTab(const std::string& initialUrl) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto tab = std::make_shared<TabInfo>();
    tab->id = m_nextTabId++;
    tab->currentUrl = initialUrl;
    tab->title = "New Tab";
    tab->historyStack.push_back(initialUrl);
    tab->historyIndex = 0;
    tab->lastAccessed = std::chrono::system_clock::now();

    m_tabs.push_back(tab);
    m_activeTabId = tab->id;

    if (m_listener) {
        m_listener(tab->id, "created");
    }

    return tab->id;
}

bool TabManager::closeTab(uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find_if(m_tabs.begin(), m_tabs.end(), [tabId](const std::shared_ptr<TabInfo>& t) {
        return t->id == tabId;
    });

    if (it == m_tabs.end()) {
        return false;
    }

    m_tabs.erase(it);

    if (m_activeTabId == tabId) {
        if (!m_tabs.empty()) {
            m_activeTabId = m_tabs.back()->id;
        } else {
            m_activeTabId = 0;
        }
    }

    if (m_listener) {
        m_listener(tabId, "closed");
    }

    return true;
}

bool TabManager::switchTab(uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId) {
            m_activeTabId = tabId;
            tab->lastAccessed = std::chrono::system_clock::now();
            if (tab->isDiscarded) {
                tab->isDiscarded = false; // Restore on switch
            }
            if (m_listener) {
                m_listener(tabId, "activated");
            }
            return true;
        }
    }
    return false;
}

bool TabManager::navigateTab(uint32_t tabId, const std::string& url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId) {
            // Truncate forward history
            if (tab->historyIndex + 1 < tab->historyStack.size()) {
                tab->historyStack.resize(tab->historyIndex + 1);
            }
            tab->historyStack.push_back(url);
            tab->historyIndex = tab->historyStack.size() - 1;
            tab->currentUrl = url;
            tab->lastAccessed = std::chrono::system_clock::now();
            tab->isDiscarded = false;

            if (m_listener) {
                m_listener(tabId, "navigated");
            }
            return true;
        }
    }
    return false;
}

bool TabManager::goBack(uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId && tab->historyIndex > 0) {
            tab->historyIndex--;
            tab->currentUrl = tab->historyStack[tab->historyIndex];
            tab->lastAccessed = std::chrono::system_clock::now();
            if (m_listener) {
                m_listener(tabId, "history_back");
            }
            return true;
        }
    }
    return false;
}

bool TabManager::goForward(uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId && tab->historyIndex + 1 < tab->historyStack.size()) {
            tab->historyIndex++;
            tab->currentUrl = tab->historyStack[tab->historyIndex];
            tab->lastAccessed = std::chrono::system_clock::now();
            if (m_listener) {
                m_listener(tabId, "history_forward");
            }
            return true;
        }
    }
    return false;
}

bool TabManager::reloadTab(uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId) {
            tab->lastAccessed = std::chrono::system_clock::now();
            if (m_listener) {
                m_listener(tabId, "reloaded");
            }
            return true;
        }
    }
    return false;
}

bool TabManager::setPinned(uint32_t tabId, bool pinned) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId) {
            tab->isPinned = pinned;
            return true;
        }
    }
    return false;
}

bool TabManager::setMuted(uint32_t tabId, bool muted) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId) {
            tab->isMuted = muted;
            return true;
        }
    }
    return false;
}

bool TabManager::discardTab(uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId) {
            // Cannot discard active tab
            if (tab->id == m_activeTabId) return false;
            tab->isDiscarded = true;
            return true;
        }
    }
    return false;
}

bool TabManager::restoreDiscardedTab(uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId) {
            tab->isDiscarded = false;
            return true;
        }
    }
    return false;
}

TabInfo* TabManager::getActiveTab() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == m_activeTabId) return tab.get();
    }
    return nullptr;
}

const TabInfo* TabManager::getActiveTab() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& tab : m_tabs) {
        if (tab->id == m_activeTabId) return tab.get();
    }
    return nullptr;
}

TabInfo* TabManager::getTab(uint32_t tabId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& tab : m_tabs) {
        if (tab->id == tabId) return tab.get();
    }
    return nullptr;
}

const TabInfo* TabManager::getTab(uint32_t tabId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& tab : m_tabs) {
        if (tab->id == tabId) return tab.get();
    }
    return nullptr;
}

std::vector<TabInfo> TabManager::getAllTabs() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<TabInfo> list;
    for (const auto& tab : m_tabs) {
        list.push_back(*tab);
    }
    return list;
}

size_t TabManager::getTabCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_tabs.size();
}

uint32_t TabManager::getActiveTabId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_activeTabId;
}

void TabManager::setTabListener(TabCallback listener) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_listener = std::move(listener);
}

} // namespace BrowserCore
