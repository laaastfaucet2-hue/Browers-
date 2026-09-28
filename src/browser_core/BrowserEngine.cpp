#include "../../include/browser_core/BrowserEngine.hpp"
#include <iostream>
#include <algorithm>

namespace BrowserCore {

BrowserEngine::BrowserEngine(const BrowserConfig& config)
    : m_config(config) {
    initialize();
}

void BrowserEngine::initialize() {
    if (m_initialized) return;

    m_adBlocker = std::make_shared<AdBlocker>();
    m_networkInterceptor = std::make_shared<NetworkInterceptor>(m_adBlocker, m_config);
    m_schemeRegistry = std::make_shared<SchemeHandlerRegistry>();
    m_privacyShield = std::make_shared<PrivacyShield>(m_config);
    m_jsBridge = std::make_shared<JsBridge>();
    m_storage = std::make_shared<BookmarkHistoryStore>();
    m_tabManager = std::make_shared<TabManager>();
    m_containerManager = std::make_shared<ContainerManager>();
    m_containerRouter = std::make_shared<AutoContainerRouter>();
    m_workspaceManager = std::make_shared<WorkspaceManager>();
    m_aiEngine = std::make_shared<AiAssistantEngine>();
    m_downloadManager = std::make_shared<DownloadManager>();
    m_perfMonitor = std::make_shared<PerformanceMonitor>();
    m_extensionRuntime = std::make_shared<ExtensionRuntime>();
    m_devToolsEngine = std::make_shared<DevToolsEngine>();
    m_passwordVault = std::make_shared<PasswordVault>();
    m_tabGroupManager = std::make_shared<TabGroupManager>();
    m_scratchpadEngine = std::make_shared<ScratchpadEngine>();
    m_hardwareLimiter = std::make_shared<HardwareLimiter>();
    m_profileManager = std::make_shared<ProfileManager>(200);

    setupInternalSchemes();
    setupJsBridgeApis();

    // Create default first tab
    m_tabManager->createTab(m_config.newTabPageUrl);

    m_initialized = true;
}

void BrowserEngine::shutdown() {
    m_initialized = false;
}

void BrowserEngine::setupInternalSchemes() {
    m_schemeRegistry->registerHandler("mybrowser", "stats", [this](const std::string&, const std::string&) {
        auto stats = m_adBlocker->getStats();
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = SchemeHandlerRegistry::renderStatsHtml(stats.totalBlocked, stats.requestsChecked);
        return resp;
    });

    m_schemeRegistry->registerHandler("mybrowser", "settings", [this](const std::string&, const std::string&) {
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = SchemeHandlerRegistry::renderSettingsHtml(m_config);
        return resp;
    });

    m_schemeRegistry->registerHandler("mybrowser", "newtab", [this](const std::string&, const std::string&) {
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = SchemeHandlerRegistry::renderNewTabHtml(m_config);
        return resp;
    });

    m_schemeRegistry->registerHandler("mybrowser", "about", [this](const std::string&, const std::string&) {
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = SchemeHandlerRegistry::renderAboutHtml(m_config);
        return resp;
    });

    m_schemeRegistry->registerHandler("mybrowser", "performance", [this](const std::string&, const std::string&) {
        auto allTabs = m_tabManager->getAllTabs();
        std::vector<uint32_t> ids;
        for (const auto& t : allTabs) ids.push_back(t.id);
        auto metrics = m_perfMonitor->getTabMetrics(ids);

        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = m_perfMonitor->generatePerformanceHtml(metrics);
        return resp;
    });

    m_schemeRegistry->registerHandler("mybrowser", "devices", [this](const std::string&, const std::string&) {
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = m_profileManager->renderFingerprintTestHtml();
        return resp;
    });

    m_schemeRegistry->registerHandler("mybrowser", "fingerprint", [this](const std::string&, const std::string&) {
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = m_profileManager->renderFingerprintTestHtml();
        return resp;
    });
}

void BrowserEngine::setupJsBridgeApis() {
    // 1. System info
    m_jsBridge->registerFunction("system.getInfo", [this](const std::vector<JsValue>&) {
        JsValue val = JsValue::makeObject();
        val.objVal["browserName"] = JsValue::makeString(m_config.browserName);
        val.objVal["version"] = JsValue::makeString(m_config.version);
        val.objVal["engine"] = JsValue::makeString("Chromium/CEF C++ Subsystem");
        val.objVal["activeTabs"] = JsValue::makeNumber(static_cast<double>(m_tabManager->getTabCount()));
        return val;
    });

    // 2. AdBlock stats
    m_jsBridge->registerFunction("adblock.getStats", [this](const std::vector<JsValue>&) {
        auto stats = m_adBlocker->getStats();
        JsValue val = JsValue::makeObject();
        val.objVal["totalBlocked"] = JsValue::makeNumber(static_cast<double>(stats.totalBlocked));
        val.objVal["adsBlocked"] = JsValue::makeNumber(static_cast<double>(stats.adsBlocked));
        val.objVal["trackersBlocked"] = JsValue::makeNumber(static_cast<double>(stats.trackersBlocked));
        val.objVal["requestsChecked"] = JsValue::makeNumber(static_cast<double>(stats.requestsChecked));
        return val;
    });

    // 3. AdBlock toggle
    m_jsBridge->registerFunction("adblock.toggle", [this](const std::vector<JsValue>& args) {
        bool newState = !m_adBlocker->isEnabled();
        if (!args.empty() && args[0].type == JsValue::Type::Boolean) {
            newState = args[0].boolVal;
        }
        m_adBlocker->setEnabled(newState);
        JsValue val = JsValue::makeObject();
        val.objVal["enabled"] = JsValue::makeBool(newState);
        return val;
    });

    // 4. Bookmarks list
    m_jsBridge->registerFunction("bookmarks.list", [this](const std::vector<JsValue>&) {
        auto bms = m_storage->getBookmarks();
        JsValue val;
        val.type = JsValue::Type::Array;
        for (const auto& b : bms) {
            JsValue item = JsValue::makeObject();
            item.objVal["url"] = JsValue::makeString(b.url);
            item.objVal["title"] = JsValue::makeString(b.title);
            val.arrVal.push_back(item);
        }
        return val;
    });

    // 5. Bookmarks add
    m_jsBridge->registerFunction("bookmarks.add", [this](const std::vector<JsValue>& args) {
        if (args.size() >= 2) {
            m_storage->addBookmark(args[0].strVal, args[1].strVal);
            JsValue res = JsValue::makeObject();
            res.objVal["success"] = JsValue::makeBool(true);
            return res;
        }
        JsValue res = JsValue::makeObject();
        res.objVal["success"] = JsValue::makeBool(false);
        res.objVal["error"] = JsValue::makeString("Missing url or title argument");
        return res;
    });
}

std::string BrowserEngine::resolveInputToUrl(const std::string& input) const {
    std::string trimmed = input;
    while (!trimmed.empty() && trimmed.front() == ' ') trimmed.erase(0, 1);
    while (!trimmed.empty() && trimmed.back() == ' ') trimmed.pop_back();

    if (trimmed.empty()) return m_config.newTabPageUrl;

    if (trimmed.find("://") != std::string::npos || trimmed.rfind("about:", 0) == 0) {
        return trimmed;
    }

    // Check if it's an internal shortcut like "settings" or "newtab"
    if (trimmed == "settings" || trimmed == "about" || trimmed == "newtab" || trimmed == "stats") {
        return "mybrowser://" + trimmed;
    }

    // Check if it looks like a domain name (contains . and no spaces)
    if (trimmed.find(' ') == std::string::npos && trimmed.find('.') != std::string::npos) {
        return "https://" + trimmed;
    }

    // Otherwise, treated as search query
    return m_config.defaultSearchEngine + trimmed;
}

NavigationResult BrowserEngine::navigate(uint32_t tabId, const std::string& inputUrl) {
    NavigationResult result;
    std::string resolved = resolveInputToUrl(inputUrl);

    // 1. Internal scheme
    if (resolved.rfind("mybrowser://", 0) == 0) {
        auto schemeResp = m_schemeRegistry->handleRequest(resolved);
        result.success = (schemeResp.statusCode == 200);
        result.statusCode = schemeResp.statusCode;
        result.finalUrl = resolved;
        result.pageTitle = (resolved == "mybrowser://newtab") ? "New Tab" : resolved;
        result.contentPreview = schemeResp.content;

        m_tabManager->navigateTab(tabId, resolved);
        auto tab = m_tabManager->getTab(tabId);
        if (tab) tab->title = result.pageTitle;
        return result;
    }

    // 2. Outgoing web request
    HttpRequest req;
    req.url = resolved;
    req.method = "GET";
    req.resourceType = ResourceType::MainFrame;

    // Check Auto-Container Router
    std::string targetContainerName, targetContainerColor;
    uint32_t targetCId = m_containerRouter->matchContainer(resolved, &targetContainerName, &targetContainerColor);
    if (targetCId > 0) {
        auto tab = m_tabManager->getTab(tabId);
        if (tab && tab->containerId != targetCId) {
            m_tabManager->setTabContainer(tabId, targetCId, targetContainerName, targetContainerColor);
            result.containerSwitched = true;
            result.newContainerName = targetContainerName;
            result.newContainerColor = targetContainerColor;
        }
    }

    auto interceptRes = m_networkInterceptor->interceptRequest(req);

    if (interceptRes.action == InterceptAction::Block) {
        result.success = false;
        result.wasBlocked = true;
        result.blockedReason = interceptRes.reason;
        result.finalUrl = "mybrowser://blocked?target=" + req.url;
        result.pageTitle = "Blocked Page";
        result.contentPreview = SchemeHandlerRegistry::renderBlockedPageHtml(req.url, interceptRes.reason);
        m_tabManager->navigateTab(tabId, result.finalUrl);
        return result;
    }

    result.finalUrl = req.url;
    result.success = true;
    result.statusCode = 200;
    result.pageTitle = req.url;
    result.contentPreview = "<html><head><title>" + req.url + "</title></head><body>Loaded content from " + req.url + "</body></html>";

    m_tabManager->navigateTab(tabId, result.finalUrl);
    auto tab = m_tabManager->getTab(tabId);
    if (tab) tab->title = result.pageTitle;

    m_storage->recordVisit(result.finalUrl, result.pageTitle);

    return result;
}

NavigationResult BrowserEngine::navigateActiveTab(const std::string& inputUrl) {
    uint32_t activeId = m_tabManager->getActiveTabId();
    if (activeId == 0) {
        activeId = m_tabManager->createTab();
    }
    return navigate(activeId, inputUrl);
}

void BrowserEngine::updateConfig(const BrowserConfig& config) {
    m_config = config;
    if (m_networkInterceptor) m_networkInterceptor->setConfig(config);
    if (m_privacyShield) m_privacyShield->updateConfig(config);
}

} // namespace BrowserCore
