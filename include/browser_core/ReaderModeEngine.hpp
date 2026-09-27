#pragma once

#include <string>

namespace BrowserCore {

class ReaderModeEngine {
public:
    ReaderModeEngine() = default;
    ~ReaderModeEngine() = default;

    // Converts messy webpage text/article into distraction-free reading mode
    static std::string formatReaderArticle(const std::string& title, const std::string& author, const std::string& content);
};

} // namespace BrowserCore
