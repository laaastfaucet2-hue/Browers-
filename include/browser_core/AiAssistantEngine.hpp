#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <cstdint>

namespace BrowserCore {

struct AiMessage {
    std::string role; // "user" or "assistant"
    std::string text;
    std::string timestamp;
};

class AiAssistantEngine {
public:
    AiAssistantEngine();
    ~AiAssistantEngine() = default;

    // Core AI browser features
    std::string summarizeContent(const std::string& pageText, const std::string& pageTitle = "");
    std::string explainCode(const std::string& codeSnippet);
    std::string translateText(const std::string& text, const std::string& targetLang = "ar");
    std::string askQuestion(const std::string& query, const std::string& pageContext = "");

    void clearChatHistory();
    std::vector<AiMessage> getChatHistory() const;
    std::string exportChatJson() const;

private:
    std::vector<AiMessage> m_history;
    mutable std::mutex m_mutex;
};

} // namespace BrowserCore
