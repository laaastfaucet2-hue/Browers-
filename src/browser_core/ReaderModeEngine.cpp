#include "../../include/browser_core/ReaderModeEngine.hpp"
#include <sstream>

namespace BrowserCore {

std::string ReaderModeEngine::formatReaderArticle(const std::string& title, const std::string& author, const std::string& content) {
    std::ostringstream ss;
    ss << "<div style=\"max-width: 720px; margin: 40px auto; padding: 0 20px; font-family: 'Georgia', serif; line-height: 1.8; color: #f8fafc;\">\n"
       << "  <div style=\"display:flex; justify-content:space-between; align-items:center; border-bottom: 1px solid #334155; padding-bottom: 12px; margin-bottom: 24px; font-family: sans-serif;\">\n"
       << "    <span style=\"color: #38bdf8; font-weight: bold;\">📖 وضع القراءة والتركيز (Reader Mode)</span>\n"
       << "    <div style=\"display: flex; gap: 8px;\">\n"
       << "      <button onclick=\"document.body.style.background='#0f172a'\" style=\"background:#0f172a; color:#fff; border:1px solid #334155; padding:4px 8px; border-radius:4px; cursor:pointer;\">داكن</button>\n"
       << "      <button onclick=\"document.body.style.background='#2d241e'\" style=\"background:#2d241e; color:#f5e6d3; border:1px solid #4a3b32; padding:4px 8px; border-radius:4px; cursor:pointer;\">سيبيا</button>\n"
       << "    </div>\n"
       << "  </div>\n"
       << "  <h1 style=\"font-size: 2.4rem; line-height: 1.3; margin-bottom: 8px; color: #fff;\">" << title << "</h1>\n"
       << "  <p style=\"color: #94a3b8; font-size: 0.95rem; margin-bottom: 30px; font-style: italic;\">بواسطة: " << (author.empty() ? "الناشر الأصلي" : author) << " • تم تنظيف المقال من الإعلانات</p>\n"
       << "  <div style=\"font-size: 1.15rem; color: #cbd5e1;\">\n"
       << content
       << "\n  </div>\n"
       << "</div>\n";
    return ss.str();
}

} // namespace BrowserCore
