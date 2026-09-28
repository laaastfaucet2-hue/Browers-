#pragma once

#include "Types.hpp"
#include "AdBlocker.hpp"
#include "NetworkInterceptor.hpp"
#include "SchemeHandler.hpp"
#include "PrivacyShield.hpp"
#include "JsBridge.hpp"
#include "BookmarkHistoryStore.hpp"
#include "TabManager.hpp"
#include "ContainerManager.hpp"
#include "AutoContainerRouter.hpp"
#include "WorkspaceManager.hpp"
#include "AiAssistantEngine.hpp"
#include "DownloadManager.hpp"
#include "PerformanceMonitor.hpp"
#include "ReaderModeEngine.hpp"
#include "ExtensionRuntime.hpp"
#include <memory>
#include <string>

namespace BrowserCore {

struct NavigationResult {
    bool success = false;
    std::string finalUrl;
    std::string pageTitle;
    std::string contentPreview;
    int statusCode = 200;
    std::string errorString;
    bool wasBlocked = false;
    std::string blockedReason;
    bool containerSwitched = false;
    std::string newContainerName;
    std::string newContainerColor;
};

class BrowserEngine {
public:
    explicit BrowserEngine(const BrowserConfig& config = BrowserConfig{});
    ~BrowserEngine() = default;

    // Initialization & Lifecycle
    void initialize();
    void shutdown();

    // High level navigation
    NavigationResult navigate(uint32_t tabId, const std::string& inputUrl);
    NavigationResult navigateActiveTab(const std::string& inputUrl);

    // Subsystems access
    std::shared_ptr<TabManager> tabs() const { return m_tabManager; }
    std::shared_ptr<ContainerManager> containers() const { return m_containerManager; }
    std::shared_ptr<AutoContainerRouter> containerRouter() const { return m_containerRouter; }
    std::shared_ptr<WorkspaceManager> workspaces() const { return m_workspaceManager; }
    std::shared_ptr<AiAssistantEngine> ai() const { return m_aiEngine; }
    std::shared_ptr<DownloadManager> downloads() const { return m_downloadManager; }
    std::shared_ptr<PerformanceMonitor> performance() const { return m_perfMonitor; }
    std::shared_ptr<ExtensionRuntime> extensions() const { return m_extensionRuntime; }
    std::shared_ptr<AdBlocker> adBlocker() const { return m_adBlocker; }
    std::shared_ptr<NetworkInterceptor> network() const { return m_networkInterceptor; }
    std::shared_ptr<SchemeHandlerRegistry> schemes() const { return m_schemeRegistry; }
    std::shared_ptr<PrivacyShield> privacy() const { return m_privacyShield; }
    std::shared_ptr<JsBridge> jsBridge() const { return m_jsBridge; }
    std::shared_ptr<BookmarkHistoryStore> storage() const { return m_storage; }

    const BrowserConfig& getConfig() const { return m_config; }
    void updateConfig(const BrowserConfig& config);

    // Helpers
    std::string resolveInputToUrl(const std::string& input) const;

private:
    void setupInternalSchemes();
    void setupJsBridgeApis();

    BrowserConfig m_config;
    std::shared_ptr<AdBlocker> m_adBlocker;
    std::shared_ptr<NetworkInterceptor> m_networkInterceptor;
    std::shared_ptr<SchemeHandlerRegistry> m_schemeRegistry;
    std::shared_ptr<PrivacyShield> m_privacyShield;
    std::shared_ptr<JsBridge> m_jsBridge;
    std::shared_ptr<BookmarkHistoryStore> m_storage;
    std::shared_ptr<TabManager> m_tabManager;
    std::shared_ptr<ContainerManager> m_containerManager;
    std::shared_ptr<AutoContainerRouter> m_containerRouter;
    std::shared_ptr<WorkspaceManager> m_workspaceManager;
    std::shared_ptr<AiAssistantEngine> m_aiEngine;
    std::shared_ptr<DownloadManager> m_downloadManager;
    std::shared_ptr<PerformanceMonitor> m_perfMonitor;
    std::shared_ptr<ExtensionRuntime> m_extensionRuntime;
    bool m_initialized = false;
};

} // namespace BrowserCore
