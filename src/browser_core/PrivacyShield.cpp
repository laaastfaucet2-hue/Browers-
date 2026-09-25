#include "../../include/browser_core/PrivacyShield.hpp"
#include <sstream>

namespace BrowserCore {

PrivacyShield::PrivacyShield(const BrowserConfig& config) : m_config(config) {}

void PrivacyShield::updateConfig(const BrowserConfig& config) {
    m_config = config;
}

std::string PrivacyShield::generateAntiFingerprintingScript() const {
    if (!m_config.antiFingerprinting) {
        return "";
    }

    // Injected into every frame before DOM is loaded
    return R"JS(
(function() {
    'use strict';

    // 1. Canvas Fingerprint Protection (Add subtle noise to readback)
    const originalToDataURL = HTMLCanvasElement.prototype.toDataURL;
    const originalGetImageData = CanvasRenderingContext2D.prototype.getImageData;

    CanvasRenderingContext2D.prototype.getImageData = function(x, y, w, h) {
        const imageData = originalGetImageData.apply(this, arguments);
        // Inject slight pseudo-random non-destructive noise
        for (let i = 0; i < imageData.data.length; i += 64) {
            imageData.data[i] = imageData.data[i] ^ 1;
        }
        return imageData;
    };

    HTMLCanvasElement.prototype.toDataURL = function() {
        const ctx = this.getContext('2d');
        if (ctx) {
            // Apply jitter
            ctx.fillStyle = 'rgba(255,255,255,0.001)';
            ctx.fillRect(0, 0, 1, 1);
        }
        return originalToDataURL.apply(this, arguments);
    };

    // 2. AudioContext Fingerprint Protection
    if (window.AudioBuffer) {
        const originalGetChannelData = AudioBuffer.prototype.getChannelData;
        AudioBuffer.prototype.getChannelData = function() {
            const data = originalGetChannelData.apply(this, arguments);
            for (let i = 0; i < data.length; i += 100) {
                data[i] += 0.0000001;
            }
            return data;
        };
    }

    // 3. Spoof Hardware Concurrency & Device Memory to standard values
    Object.defineProperty(navigator, 'hardwareConcurrency', { get: () => 4 });
    Object.defineProperty(navigator, 'deviceMemory', { get: () => 8 });

    console.log('[AtlasShield] Anti-fingerprinting protections active.');
})();
)JS";
}

std::vector<std::string> PrivacyShield::getChromiumWebRtcArgs() const {
    std::vector<std::string> args;
    if (m_config.preventWebRtcLeak) {
        // Essential Chromium / CEF command line flags to prevent WebRTC IP leakage
        args.push_back("--webrtc-ip-handling-policy=disable_non_proxied_udp");
        args.push_back("--enforce-webrtc-ip-permission-check");
        args.push_back("--force-webrtc-ip-handling-policy");
    }
    return args;
}

std::string PrivacyShield::sanitizeReferrer(const std::string& currentUrl, const std::string& targetUrl) const {
    // If different origin, strip path/query and send only origin
    size_t curProtocol = currentUrl.find("://");
    size_t tgtProtocol = targetUrl.find("://");

    if (curProtocol != std::string::npos && tgtProtocol != std::string::npos) {
        size_t curEnd = currentUrl.find('/', curProtocol + 3);
        size_t tgtEnd = targetUrl.find('/', tgtProtocol + 3);

        std::string curOrigin = (curEnd != std::string::npos) ? currentUrl.substr(0, curEnd) : currentUrl;
        std::string tgtOrigin = (tgtEnd != std::string::npos) ? targetUrl.substr(0, tgtEnd) : targetUrl;

        if (curOrigin != tgtOrigin) {
            return curOrigin + "/"; // Strict origin when cross-origin
        }
    }
    return currentUrl;
}

bool PrivacyShield::shouldBlockThirdPartyCookie(const std::string& requestUrl, const std::string& tabOrigin) const {
    if (!m_config.blockThirdPartyCookies) return false;
    if (tabOrigin.empty()) return false;

    // Compare domains
    size_t reqSlash = requestUrl.find("://");
    size_t tabSlash = tabOrigin.find("://");

    if (reqSlash != std::string::npos && tabSlash != std::string::npos) {
        std::string reqHost = requestUrl.substr(reqSlash + 3);
        size_t rSlash = reqHost.find('/');
        if (rSlash != std::string::npos) reqHost = reqHost.substr(0, rSlash);

        std::string tabHost = tabOrigin.substr(tabSlash + 3);
        size_t tSlash = tabHost.find('/');
        if (tSlash != std::string::npos) tabHost = tabHost.substr(0, tSlash);

        return reqHost != tabHost;
    }
    return false;
}

} // namespace BrowserCore
