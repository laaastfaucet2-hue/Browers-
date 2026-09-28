#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct NoteItem {
    uint32_t id;
    std::string title;
    std::string content;
    std::string sourceUrl;
    std::string updatedAt;
};

class ScratchpadEngine {
public:
    ScratchpadEngine();
    ~ScratchpadEngine() = default;

    uint32_t createNote(const std::string& title, const std::string& content, const std::string& sourceUrl = "");
    bool updateNote(uint32_t id, const std::string& title, const std::string& content);
    bool deleteNote(uint32_t id);
    std::vector<NoteItem> getNotes() const;

    // Web Clipper
    uint32_t clipWebSelection(const std::string& selectedText, const std::string& pageTitle, const std::string& pageUrl);

    std::string exportNotesJson() const;

private:
    std::vector<NoteItem> m_notes;
    uint32_t m_nextId = 1;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
