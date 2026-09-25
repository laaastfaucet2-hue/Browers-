#include "../../include/browser_core/NetworkInterceptor.hpp"
#include <sstream>
#include <vector>
#include <set>

namespace BrowserCore {

NetworkInterceptor::NetworkInterceptor(std::shared_ptr<AdBlocker> adBlocker, const BrowserConfig& config)
    : m_adBlocker(std::move(adBlocker)), m_config(config) {}

bool NetworkInterceptor::isInternalScheme(const std::string& url) {
    return (url.rfind("mybrowser://", 0) == 0 ||
            url.rfind("chrome://", 0) == 0 ||
            url.rfind("about:", 0) == 0 ||
            url.rfind("file://", 0) == 0 ||
            url.rfind("data:", 0) == 0);
}

std::string NetworkInterceptor::upgradeToHttps(const std::string& url) {
    if (url.rfind("http://", 0) == 0) {
        return "https://" + url.substr(7);
    }
    return url;
}

std::string NetworkInterceptor::stripTrackingParameters(const std::string& url) {
    static const std::set<std::string> trackingParams = {
        "utm_source", "utm_medium", "utm_campaign", "utm_term", "utm_content",
        "fbclid", "gclid", "msclkid", "mc_eid", "igshid", "yclid", "_ga", "_gl",
        "vero_id", "wickedid", "twclid"
    };

    size_t queryPos = url.find('?');
    if (queryPos == std::string::npos) {
        return url;
    }

    std::string baseUrl = url.substr(0, queryPos);
    std::string queryString = url.substr(queryPos + 1);

    // Split query string by &
    std::vector<std::string> cleanParams;
    std::istringstream iss(queryString);
    std::string pair;

    while (std::getline(iss, pair, '&')) {
        size_t eqPos = pair.find('=');
        std::string key = (eqPos != std::string::npos) ? pair.substr(0, eqPos) : pair;
        if (trackingParams.find(key) == trackingParams.end()) {
            cleanParams.push_back(pair);
        }
    }

    if (cleanParams.empty()) {
        return baseUrl;
    }

    std::ostringstream oss;
    oss << baseUrl << "?";
    for (size_t i = 0; i < cleanParams.size(); ++i) {
        if (i > 0) oss << "&";
        oss << cleanParams[i];
    }
    return oss.str();
}

InterceptResult NetworkInterceptor::interceptRequest(HttpRequest& request) {
    InterceptResult result;
    result.action = InterceptAction::Allow;

    // 1. Internal schemes bypass network inspection
    if (isInternalScheme(request.url)) {
        return result;
    }

    // 2. HTTPS Upgrade
    if (m_config.enableHttpsUpgrade && request.url.rfind("http://", 0) == 0) {
        result.action = InterceptAction::Redirect;
        result.redirectedUrl = upgradeToHttps(request.url);
        result.reason = "Automatic HTTPS Upgrade";
        request.url = result.redirectedUrl;
        return result;
    }

    // 3. Strip tracking parameters
    if (m_config.stripTrackingParams) {
        std::string stripped = stripTrackingParameters(request.url);
        if (stripped != request.url) {
            request.url = stripped;
            result.action = InterceptAction::Redirect;
            result.redirectedUrl = stripped;
            result.reason = "Tracking Parameters Stripped";
        }
    }

    // 4. AdBlocker Check
    if (m_config.enableAdBlocker && m_adBlocker) {
        BlockCategory category;
        std::string matchedRule;
        if (m_adBlocker->shouldBlock(request, &category, &matchedRule)) {
            result.action = InterceptAction::Block;
            result.reason = "Blocked by Filter Rule: " + matchedRule;
            return result;
        }
    }

    // 5. Custom Hooks
    for (const auto& hook : m_customHooks) {
        auto hookResult = hook(request);
        if (hookResult.action != InterceptAction::Allow) {
            return hookResult;
        }
    }

    // 6. Inject Privacy Headers
    request.headers["User-Agent"] = m_config.userAgent;
    if (m_config.antiFingerprinting) {
        request.headers["DNT"] = "1";
        request.headers["Sec-GPC"] = "1";
    }

    return result;
}

void NetworkInterceptor::setConfig(const BrowserConfig& config) {
    m_config = config;
}

const BrowserConfig& NetworkInterceptor::getConfig() const {
    return m_config;
}

void NetworkInterceptor::addCustomHook(RequestHook hook) {
    m_customHooks.push_back(std::move(hook));
}

} // namespace BrowserCore
