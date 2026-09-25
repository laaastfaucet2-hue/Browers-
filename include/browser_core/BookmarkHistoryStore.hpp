#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <mutex>

namespace BrowserCore {

struct Bookmark {
    std::string id;
    std::string url;
    std::string title;
    std::string folder = "Bookmarks Bar";
    std::chrono::system_clock::time_point dateAdded;
};

struct HistoryEntry {
    std::string url;
    std::string title;
    uint32_t visitCount = 1;
    std::chrono::system_clock::time_point lastVisited;
};

class BookmarkHistoryStore {
public:
    BookmarkHistoryStore();
    ~BookmarkHistoryStore() = default;

    // Bookmarks
    void addBookmark(const std::string& url, const std::string& title, const std::string& folder = "Bookmarks Bar");
    bool removeBookmark(const std::string& url);
    std::vector<Bookmark> getBookmarks() const;
    std::vector<Bookmark> searchBookmarks(const std::string& query) const;

    // History
    void recordVisit(const std::string& url, const std::string& title);
    std::vector<HistoryEntry> getHistory(size_t limit = 100) const;
    std::vector<HistoryEntry> searchHistory(const std::string& query) const;
    void clearHistory();

    // Export JSON
    std::string exportBookmarksJson() const;
    std::string exportHistoryJson() const;

private:
    std::vector<Bookmark> m_bookmarks;
    std::vector<HistoryEntry> m_history;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
