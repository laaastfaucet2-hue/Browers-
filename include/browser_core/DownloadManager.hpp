#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

enum class DownloadStatus {
    Downloading,
    Paused,
    Completed,
    Cancelled,
    BlockedDangerous
};

inline std::string downloadStatusToString(DownloadStatus s) {
    switch (s) {
        case DownloadStatus::Downloading: return "Downloading";
        case DownloadStatus::Paused: return "Paused";
        case DownloadStatus::Completed: return "Completed";
        case DownloadStatus::Cancelled: return "Cancelled";
        case DownloadStatus::BlockedDangerous: return "Blocked (Malware Risk)";
        default: return "Unknown";
    }
}

struct DownloadItem {
    uint32_t id;
    std::string url;
    std::string fileName;
    uint64_t totalBytes;
    uint64_t receivedBytes;
    int progressPercent;
    std::string speedStr;
    DownloadStatus status;
    std::string securityNotice;
};

class DownloadManager {
public:
    DownloadManager();
    ~DownloadManager() = default;

    uint32_t startDownload(const std::string& url, const std::string& fileName = "", uint64_t expectedSize = 10485760);
    bool pauseDownload(uint32_t id);
    bool resumeDownload(uint32_t id);
    bool cancelDownload(uint32_t id);

    std::vector<DownloadItem> getAllDownloads() const;
    std::string exportDownloadsJson() const;

private:
    static bool isDangerousFileExtension(const std::string& fileName);
    static std::string extractFileNameFromUrl(const std::string& url);

    std::vector<DownloadItem> m_items;
    uint32_t m_nextId = 1;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
