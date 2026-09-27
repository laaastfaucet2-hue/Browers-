#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct TabPerformanceMetric {
    uint32_t tabId;
    std::string title;
    double memoryUsageMb;
    double cpuPercentage;
    bool isSleeping;
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;

    std::vector<TabPerformanceMetric> getTabMetrics(const std::vector<uint32_t>& activeTabIds) const;
    double getTotalMemoryUsageMb() const;
    double getMemorySavedMb() const;

    std::string generatePerformanceHtml(const std::vector<TabPerformanceMetric>& metrics) const;

private:
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
