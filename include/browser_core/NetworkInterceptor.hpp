#pragma once

#include "Types.hpp"
#include "AdBlocker.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace BrowserCore {

class NetworkInterceptor {
public:
    explicit NetworkInterceptor(std::shared_ptr<AdBlocker> adBlocker, const BrowserConfig& config);
    ~NetworkInterceptor() = default;

    // Process and inspect outgoing request
    InterceptResult interceptRequest(HttpRequest& request);

    // Helpers
    static std::string stripTrackingParameters(const std::string& url);
    static std::string upgradeToHttps(const std::string& url);
    static bool isInternalScheme(const std::string& url);

    void setConfig(const BrowserConfig& config);
    const BrowserConfig& getConfig() const;

    // Custom request hook callbacks
    using RequestHook = std::function<InterceptResult(const HttpRequest&)>;
    void addCustomHook(RequestHook hook);

private:
    std::shared_ptr<AdBlocker> m_adBlocker;
    BrowserConfig m_config;
    std::vector<RequestHook> m_customHooks;
};

} // namespace BrowserCore
