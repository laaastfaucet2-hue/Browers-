#include "../../include/browser_core/HardwareLimiter.hpp"
#include <sstream>

namespace BrowserCore {

HardwareLimiter::HardwareLimiter() {
    m_pipState.mediaTitle = "Mozilla Firefox Gecko Engine Overview";
    m_pipState.mediaUrl = "https://youtube.com/watch?v=firefox_gecko_demo";
    m_pipState.isPlaying = true;
    m_pipState.playbackRate = 1.0;
    m_pipState.isMuted = false;
    m_pipState.isPipActive = false;
}

void HardwareLimiter::setCpuLimitPercent(int percent) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (percent < 10) percent = 10;
    if (percent > 100) percent = 100;
    m_cpuLimitPercent = percent;
}

void HardwareLimiter::setRamLimitMb(int maxMb, bool hardLimit) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (maxMb < 512) maxMb = 512;
    m_ramLimitMb = maxMb;
    m_hardLimit = hardLimit;
}

void HardwareLimiter::togglePip(bool active, const std::string& title, const std::string& url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pipState.isPipActive = active;
    if (!title.empty()) m_pipState.mediaTitle = title;
    if (!url.empty()) m_pipState.mediaUrl = url;
}

void HardwareLimiter::setPlaybackRate(double rate) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pipState.playbackRate = rate;
}

void HardwareLimiter::togglePlayPause() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pipState.isPlaying = !m_pipState.isPlaying;
}

void HardwareLimiter::toggleMute() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pipState.isMuted = !m_pipState.isMuted;
}

MediaPipState HardwareLimiter::getPipState() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pipState;
}

std::string HardwareLimiter::exportHardwareJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "{\n"
       << "  \"cpuLimitPercent\": " << m_cpuLimitPercent << ",\n"
       << "  \"ramLimitMb\": " << m_ramLimitMb << ",\n"
       << "  \"hardLimit\": " << (m_hardLimit ? "true" : "false") << ",\n"
       << "  \"pip\": {\n"
       << "    \"active\": " << (m_pipState.isPipActive ? "true" : "false") << ",\n"
       << "    \"title\": \"" << m_pipState.mediaTitle << "\",\n"
       << "    \"url\": \"" << m_pipState.mediaUrl << "\",\n"
       << "    \"isPlaying\": " << (m_pipState.isPlaying ? "true" : "false") << ",\n"
       << "    \"playbackRate\": " << m_pipState.playbackRate << ",\n"
       << "    \"isMuted\": " << (m_pipState.isMuted ? "true" : "false") << "\n"
       << "  }\n"
       << "}\n";
    return ss.str();
}

} // namespace BrowserCore
