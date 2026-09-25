#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace BrowserCore {

class PrivacyShield {
public:
    explicit PrivacyShield(const BrowserConfig& config);
    ~PrivacyShield() = default;

    // Generates tamper-proof JS scripts injected before page loads (Content Script / Preload Script)
    std::string generateAntiFingerprintingScript() const;

    // WebRTC IP leak protection flags for Chromium/CEF/Gecko
    std::vector<std::string> getChromiumWebRtcArgs() const;

    // Cookie & Referrer policy enforcement
    std::string sanitizeReferrer(const std::string& currentUrl, const std::string& targetUrl) const;
    bool shouldBlockThirdPartyCookie(const std::string& requestUrl, const std::string& tabOrigin) const;

    void updateConfig(const BrowserConfig& config);

private:
    BrowserConfig m_config;
};

} // namespace BrowserCore
