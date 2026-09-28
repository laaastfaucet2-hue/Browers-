#pragma once

#include <string>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct MediaPipState {
    bool isPipActive = false;
    std::string mediaTitle;
    std::string mediaUrl;
    bool isPlaying = true;
    double playbackRate = 1.0;
    bool isMuted = false;
};

class HardwareLimiter {
public:
    HardwareLimiter();
    ~HardwareLimiter() = default;

    // CPU Limiter
    void setCpuLimitPercent(int percent);
    int getCpuLimitPercent() const { return m_cpuLimitPercent; }

    // RAM Limiter
    void setRamLimitMb(int maxMb, bool hardLimit = false);
    int getRamLimitMb() const { return m_ramLimitMb; }
    bool isHardLimit() const { return m_hardLimit; }

    // PiP Controller
    void togglePip(bool active, const std::string& title = "", const std::string& url = "");
    void setPlaybackRate(double rate);
    void togglePlayPause();
    void toggleMute();
    MediaPipState getPipState() const;

    std::string exportHardwareJson() const;

private:
    int m_cpuLimitPercent = 100; // 100% = no limit
    int m_ramLimitMb = 4096;      // 4GB max default
    bool m_hardLimit = false;

    MediaPipState m_pipState;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
