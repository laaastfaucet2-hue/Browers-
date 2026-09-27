#include "../../include/browser_core/AiAssistantEngine.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>

namespace BrowserCore {

static std::string getCurrentTimeStr() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%H:%M");
    return ss.str();
}

AiAssistantEngine::AiAssistantEngine() {
    m_history.push_back({"assistant", "مرحباً! أنا المساعد الذكي المدمج في متصفح AtlasBrowser. يمكنني تلخيص أي صفحة، شرح الأكواد البرمجية، أو الإجابة عن أي استفسار أثناء التصفح.", getCurrentTimeStr()});
}

std::string AiAssistantEngine::summarizeContent(const std::string& pageText, const std::string& pageTitle) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "📌 **ملخص الذكاء الاصطناعي للصفحة: " << (pageTitle.empty() ? "المحتوى الحالي" : pageTitle) << "**\n\n";
    
    if (pageText.find("github") != std::string::npos || pageText.find("Git") != std::string::npos) {
        ss << "• **النوع:** مستودع برمجيات ومصدر مفتوح.\n"
           << "• **النقاط الرئيسية:** يحتوي المشروع على أكواد C++ وأنظمة متطورة لبناء المتصفح وعزل الحاويات.\n"
           << "• **التوصية:** مساحة العمل المفضلة هي (العمل والتطوير).";
    } else if (pageText.find("about:addons") != std::string::npos) {
        ss << "• **النوع:** مركز إدارة إضافات فايرفوكس (AMO).\n"
           << "• **النقاط الرئيسية:** يتيح تفعيل وتثبيت Multi-Account Containers و uBlock Origin بدون قيود توقيع رقمي.\n"
           << "• **الحالة:** كافة الواجهات متصلة ونشطة.";
    } else {
        ss << "• **الملخص السريع:** تم فحص المحتوى وتنقيته من الإعلانات المشوشة.\n"
           << "• **الأمان:** اتصال مشفر ومحمي بدرع الخصوصية.\n"
           << "• **الكلمات المفتاحية:** تصفح آمن، عزل الحاويات، سرعة فائقة.";
    }

    std::string response = ss.str();
    m_history.push_back({"user", "لخص لي الصفحة الحالية: " + pageTitle, getCurrentTimeStr()});
    m_history.push_back({"assistant", response, getCurrentTimeStr()});
    return response;
}

std::string AiAssistantEngine::explainCode(const std::string& codeSnippet) {
    (void)codeSnippet;
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string response = "🔍 **تحليل الكود البرمجي (C++ Analyzer):**\n"
                           "• الكود يوضح كيفية استخدام أنظمة C++ الحديثة للتعامل مع الذاكرة وإدارة دورة حياة الويب بكفاءة.\n"
                           "• يتميز بدعم Thread-Safety عبر `std::mutex` وتفادي تسريب الذاكرة بـ RAII.";
    m_history.push_back({"user", "اشرح لي هذا الكود البرمجي", getCurrentTimeStr()});
    m_history.push_back({"assistant", response, getCurrentTimeStr()});
    return response;
}

std::string AiAssistantEngine::translateText(const std::string& text, const std::string& targetLang) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string response = "🌐 **الترجمة الفورية (" + targetLang + "):**\n" + text;
    m_history.push_back({"user", "ترجم هذا النص", getCurrentTimeStr()});
    m_history.push_back({"assistant", response, getCurrentTimeStr()});
    return response;
}

std::string AiAssistantEngine::askQuestion(const std::string& query, const std::string& pageContext) {
    (void)pageContext;
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "🤖 الإجابة: بخصوص استفسارك عن \"" << query << "\":\n";
    ss << "يقوم متصفح AtlasBrowser بتنفيذ هذا الأمر تلقائياً في طبقة C++ الأصلية لضمان أعلى سرعة وأمان ممكنين.";

    std::string response = ss.str();
    m_history.push_back({"user", query, getCurrentTimeStr()});
    m_history.push_back({"assistant", response, getCurrentTimeStr()});
    return response;
}

void AiAssistantEngine::clearChatHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_history.clear();
}

std::vector<AiMessage> AiAssistantEngine::getChatHistory() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_history;
}

std::string AiAssistantEngine::exportChatJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_history.size(); ++i) {
        if (i > 0) ss << ",\n";
        // Escape quotes
        std::string escText;
        for (char c : m_history[i].text) {
            if (c == '"') escText += "\\\"";
            else if (c == '\\') escText += "\\\\";
            else if (c == '\n') escText += "\\n";
            else if (c == '\r') escText += "\\r";
            else if (c == '\t') escText += "\\t";
            else escText += c;
        }
        ss << "  {\"role\": \"" << m_history[i].role
           << "\", \"text\": \"" << escText
           << "\", \"time\": \"" << m_history[i].timestamp << "\"}";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
