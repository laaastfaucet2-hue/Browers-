#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct WebExtension {
    std::string id;
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string icon;
    std::string category;
    std::string usersCount;
    double rating = 5.0;
    bool isInstalled = false;
    bool isEnabled = true;
    std::vector<std::string> matches;
    std::string contentCss;
    std::string contentJs;
    std::string popupTitle;
    std::string popupHtml;
};

class ExtensionRuntime {
public:
    ExtensionRuntime();
    ~ExtensionRuntime() = default;

    void initCatalog();

    // Store & Catalog operations
    std::vector<WebExtension> getCatalog(const std::string& searchQuery = "") const;
    std::vector<WebExtension> getInstalledExtensions() const;
    const WebExtension* getExtension(const std::string& id) const;

    // Lifecycle
    bool installExtension(const std::string& id);
    bool uninstallExtension(const std::string& id);
    bool toggleExtension(const std::string& id, bool enable);

    // Injection engine for Web Pages
    std::string getInjectedCssForUrl(const std::string& url) const;
    std::string getInjectedJsForUrl(const std::string& url) const;
    bool matchesUrl(const std::string& pattern, const std::string& url) const;

    // Export state
    std::string exportExtensionsJson() const;

private:
    std::vector<WebExtension> m_catalog;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
