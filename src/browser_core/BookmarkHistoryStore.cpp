#include "../../include/browser_core/BookmarkHistoryStore.hpp"
#include <sstream>
#include <algorithm>

namespace BrowserCore {

BookmarkHistoryStore::BookmarkHistoryStore() {
    // Add default bookmarks
    addBookmark("https://duckduckgo.com", "DuckDuckGo Privacy Search");
    addBookmark("https://github.com", "GitHub Developer Platform");
    addBookmark("https://cppreference.com", "C++ Reference Documentation");
}

void BookmarkHistoryStore::addBookmark(const std::string& url, const std::string& title, const std::string& folder) {
    std::lock_guard<std::mutex> lock(m_mutex);
    // Check if already exists
    for (auto& bm : m_bookmarks) {
        if (bm.url == url) {
            bm.title = title;
            bm.folder = folder;
            return;
        }
    }
    Bookmark bm;
    bm.id = std::to_string(m_bookmarks.size() + 1);
    bm.url = url;
    bm.title = title;
    bm.folder = folder;
    bm.dateAdded = std::chrono::system_clock::now();
    m_bookmarks.push_back(bm);
}

bool BookmarkHistoryStore::removeBookmark(const std::string& url) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::remove_if(m_bookmarks.begin(), m_bookmarks.end(),
        [&url](const Bookmark& b) { return b.url == url; });
    if (it != m_bookmarks.end()) {
        m_bookmarks.erase(it, m_bookmarks.end());
        return true;
    }
    return false;
}

std::vector<Bookmark> BookmarkHistoryStore::getBookmarks() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_bookmarks;
}

std::vector<Bookmark> BookmarkHistoryStore::searchBookmarks(const std::string& query) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Bookmark> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), [](unsigned char c){ return std::tolower(c); });

    for (const auto& b : m_bookmarks) {
        std::string lowerUrl = b.url;
        std::string lowerTitle = b.title;
        std::transform(lowerUrl.begin(), lowerUrl.end(), lowerUrl.begin(), [](unsigned char c){ return std::tolower(c); });
        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), [](unsigned char c){ return std::tolower(c); });

        if (lowerUrl.find(lowerQuery) != std::string::npos || lowerTitle.find(lowerQuery) != std::string::npos) {
            results.push_back(b);
        }
    }
    return results;
}

void BookmarkHistoryStore::recordVisit(const std::string& url, const std::string& title) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& h : m_history) {
        if (h.url == url) {
            h.visitCount++;
            h.title = title.empty() ? h.title : title;
            h.lastVisited = std::chrono::system_clock::now();
            return;
        }
    }
    HistoryEntry entry;
    entry.url = url;
    entry.title = title.empty() ? url : title;
    entry.visitCount = 1;
    entry.lastVisited = std::chrono::system_clock::now();
    m_history.push_back(entry);
}

std::vector<HistoryEntry> BookmarkHistoryStore::getHistory(size_t limit) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<HistoryEntry> sorted = m_history;
    std::sort(sorted.begin(), sorted.end(), [](const HistoryEntry& a, const HistoryEntry& b) {
        return a.lastVisited > b.lastVisited;
    });
    if (sorted.size() > limit) {
        sorted.resize(limit);
    }
    return sorted;
}

std::vector<HistoryEntry> BookmarkHistoryStore::searchHistory(const std::string& query) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<HistoryEntry> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), [](unsigned char c){ return std::tolower(c); });

    for (const auto& h : m_history) {
        std::string lowerUrl = h.url;
        std::string lowerTitle = h.title;
        std::transform(lowerUrl.begin(), lowerUrl.end(), lowerUrl.begin(), [](unsigned char c){ return std::tolower(c); });
        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), [](unsigned char c){ return std::tolower(c); });

        if (lowerUrl.find(lowerQuery) != std::string::npos || lowerTitle.find(lowerQuery) != std::string::npos) {
            results.push_back(h);
        }
    }
    return results;
}

void BookmarkHistoryStore::clearHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_history.clear();
}

std::string BookmarkHistoryStore::exportBookmarksJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_bookmarks.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "  {\"url\": \"" << m_bookmarks[i].url << "\", \"title\": \"" << m_bookmarks[i].title << "\"}";
    }
    ss << "\n]\n";
    return ss.str();
}

std::string BookmarkHistoryStore::exportHistoryJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_history.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "  {\"url\": \"" << m_history[i].url << "\", \"title\": \"" << m_history[i].title
           << "\", \"visits\": " << m_history[i].visitCount << "}";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
