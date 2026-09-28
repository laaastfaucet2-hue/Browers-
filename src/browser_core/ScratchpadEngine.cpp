#include "../../include/browser_core/ScratchpadEngine.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>

namespace BrowserCore {

static std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M");
    return ss.str();
}

ScratchpadEngine::ScratchpadEngine() {
    createNote("أفكار تطوير النواة", "1. استكمال معمارية الحاويات C++.\n2. تحسين حقن WebExtensions.\n3. ربط خوارزميات الذكاء الاصطناعي بنموذج محلي.", "https://github.com");
}

uint32_t ScratchpadEngine::createNote(const std::string& title, const std::string& content, const std::string& sourceUrl) {
    std::lock_guard<std::mutex> lock(m_mutex);
    NoteItem note;
    note.id = m_nextId++;
    note.title = title.empty() ? "ملاحظة بدون عنوان" : title;
    note.content = content;
    note.sourceUrl = sourceUrl;
    note.updatedAt = getTimestamp();
    m_notes.push_back(note);
    return note.id;
}

bool ScratchpadEngine::updateNote(uint32_t id, const std::string& title, const std::string& content) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& n : m_notes) {
        if (n.id == id) {
            n.title = title;
            n.content = content;
            n.updatedAt = getTimestamp();
            return true;
        }
    }
    return false;
}

bool ScratchpadEngine::deleteNote(uint32_t id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_notes.begin(); it != m_notes.end(); ++it) {
        if (it->id == id) {
            m_notes.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<NoteItem> ScratchpadEngine::getNotes() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_notes;
}

uint32_t ScratchpadEngine::clipWebSelection(const std::string& selectedText, const std::string& pageTitle, const std::string& pageUrl) {
    std::string title = "اقتباس من: " + (pageTitle.empty() ? pageUrl : pageTitle);
    std::string content = "> \"" + selectedText + "\"\n\nالمصدر: " + pageUrl;
    return createNote(title, content, pageUrl);
}

static std::string escapeJson(const std::string& str) {
    std::string out;
    for (char c : str) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

std::string ScratchpadEngine::exportNotesJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_notes.size(); ++i) {
        if (i > 0) ss << ",\n";
        const auto& n = m_notes[i];
        ss << "  {\"id\": " << n.id
           << ", \"title\": \"" << escapeJson(n.title) << "\""
           << ", \"content\": \"" << escapeJson(n.content) << "\""
           << ", \"sourceUrl\": \"" << escapeJson(n.sourceUrl) << "\""
           << ", \"updatedAt\": \"" << n.updatedAt << "\"}";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
