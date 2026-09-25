#include "../../include/browser_core/SchemeHandler.hpp"
#include <sstream>

namespace BrowserCore {

SchemeHandlerRegistry::SchemeHandlerRegistry() {
    registerScheme("mybrowser");

    registerHandler("mybrowser", "newtab", [](const std::string&, const std::string&) {
        BrowserConfig defaultConfig;
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = SchemeHandlerRegistry::renderNewTabHtml(defaultConfig);
        return resp;
    });

    registerHandler("mybrowser", "settings", [](const std::string&, const std::string&) {
        BrowserConfig defaultConfig;
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = SchemeHandlerRegistry::renderSettingsHtml(defaultConfig);
        return resp;
    });

    registerHandler("mybrowser", "about", [](const std::string&, const std::string&) {
        BrowserConfig defaultConfig;
        SchemeResponse resp;
        resp.statusCode = 200;
        resp.mimeType = "text/html; charset=utf-8";
        resp.content = SchemeHandlerRegistry::renderAboutHtml(defaultConfig);
        return resp;
    });
}

void SchemeHandlerRegistry::registerScheme(const std::string& scheme) {
    if (m_handlers.find(scheme) == m_handlers.end()) {
        m_handlers[scheme] = std::map<std::string, SchemeHandlerCallback>();
    }
}

bool SchemeHandlerRegistry::hasScheme(const std::string& scheme) const {
    return m_handlers.find(scheme) != m_handlers.end();
}

void SchemeHandlerRegistry::registerHandler(const std::string& scheme, const std::string& path, SchemeHandlerCallback callback) {
    registerScheme(scheme);
    m_handlers[scheme][path] = std::move(callback);
}

SchemeResponse SchemeHandlerRegistry::handleRequest(const std::string& url) {
    SchemeResponse notFound;
    notFound.statusCode = 404;
    notFound.content = "<html><body><h1>404 Not Found</h1><p>Internal URL not recognized: " + url + "</p></body></html>";

    size_t schemeDelim = url.find("://");
    if (schemeDelim == std::string::npos) {
        return notFound;
    }

    std::string scheme = url.substr(0, schemeDelim);
    std::string remainder = url.substr(schemeDelim + 3);

    std::string path = remainder;
    std::string query;
    size_t qPos = remainder.find('?');
    if (qPos != std::string::npos) {
        path = remainder.substr(0, qPos);
        query = remainder.substr(qPos + 1);
    }

    // Strip trailing slash
    if (!path.empty() && path.back() == '/') {
        path.pop_back();
    }

    auto schemeIt = m_handlers.find(scheme);
    if (schemeIt != m_handlers.end()) {
        auto pathIt = schemeIt->second.find(path);
        if (pathIt != schemeIt->second.end()) {
            return pathIt->second(path, query);
        }
    }

    return notFound;
}

std::string SchemeHandlerRegistry::renderNewTabHtml(const BrowserConfig& config) {
    std::ostringstream ss;
    ss << "<!DOCTYPE html>\n"
       << "<html lang=\"ar\" dir=\"rtl\">\n"
       << "<head>\n"
       << "  <meta charset=\"UTF-8\">\n"
       << "  <title>صفحة البداية - " << config.browserName << "</title>\n"
       << "  <style>\n"
       << "    body { font-family: 'Segoe UI', Tahoma, sans-serif; background: #0f172a; color: #f8fafc; margin: 0; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; }\n"
       << "    .hero { text-align: center; margin-bottom: 2rem; }\n"
       << "    .hero h1 { font-size: 2.8rem; margin: 0; background: linear-gradient(135deg, #38bdf8, #818cf8); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }\n"
       << "    .search-box { width: 600px; max-width: 90%; position: relative; }\n"
       << "    .search-box input { width: 100%; padding: 16px 24px; font-size: 1.1rem; border-radius: 30px; border: 2px solid #334155; background: #1e293b; color: #fff; outline: none; transition: border-color 0.2s; box-sizing: border-box; }\n"
       << "    .search-box input:focus { border-color: #38bdf8; box-shadow: 0 0 15px rgba(56, 189, 248, 0.3); }\n"
       << "    .shortcuts { display: flex; gap: 20px; margin-top: 2.5rem; }\n"
       << "    .shortcut-card { background: #1e293b; padding: 15px 20px; border-radius: 12px; border: 1px solid #334155; text-decoration: none; color: #cbd5e1; display: flex; align-items: center; gap: 10px; transition: transform 0.2s, background 0.2s; }\n"
       << "    .shortcut-card:hover { transform: translateY(-3px); background: #334155; color: #38bdf8; }\n"
       << "    .badges { margin-top: 3rem; display: flex; gap: 15px; }\n"
       << "    .badge { font-size: 0.85rem; padding: 6px 14px; border-radius: 20px; background: rgba(56, 189, 248, 0.1); color: #38bdf8; border: 1px solid rgba(56, 189, 248, 0.2); }\n"
       << "  </style>\n"
       << "</head>\n"
       << "<body>\n"
       << "  <div class=\"hero\">\n"
       << "    <h1>" << config.browserName << "</h1>\n"
       << "    <p style=\"color: #94a3b8;\">متصفحك المخصص فائق السرعة والأمان المبني بنواة C++</p>\n"
       << "  </div>\n"
       << "  <div class=\"search-box\">\n"
       << "    <form action=\"" << config.defaultSearchEngine << "\" method=\"GET\">\n"
       << "      <input type=\"text\" name=\"q\" placeholder=\"ابحث في الويب أو اكتب عنوان URL...\" autofocus autocomplete=\"off\" />\n"
       << "    </form>\n"
       << "  </div>\n"
       << "  <div class=\"shortcuts\">\n"
       << "    <a class=\"shortcut-card\" href=\"https://github.com\">GitHub</a>\n"
       << "    <a class=\"shortcut-card\" href=\"https://duckduckgo.com\">DuckDuckGo</a>\n"
       << "    <a class=\"shortcut-card\" href=\"mybrowser://settings\">الإعدادات</a>\n"
       << "    <a class=\"shortcut-card\" href=\"mybrowser://about\">حول المتصفح</a>\n"
       << "  </div>\n"
       << "  <div class=\"badges\">\n"
       << "    <span class=\"badge\">درع الخصوصية نشط</span>\n"
       << "    <span class=\"badge\">مانع الإعلانات مفعل</span>\n"
       << "    <span class=\"badge\">ترقية HTTPS تلقائية</span>\n"
       << "  </div>\n"
       << "</body>\n"
       << "</html>\n";
    return ss.str();
}

std::string SchemeHandlerRegistry::renderSettingsHtml(const BrowserConfig& config) {
    std::ostringstream ss;
    ss << "<!DOCTYPE html>\n"
       << "<html lang=\"ar\" dir=\"rtl\">\n"
       << "<head>\n"
       << "  <meta charset=\"UTF-8\">\n"
       << "  <title>الإعدادات - " << config.browserName << "</title>\n"
       << "  <style>\n"
       << "    body { font-family: sans-serif; background: #0f172a; color: #e2e8f0; margin: 0; padding: 40px; }\n"
       << "    .container { max-width: 800px; margin: 0 auto; }\n"
       << "    h1 { color: #38bdf8; border-bottom: 2px solid #334155; padding-bottom: 12px; }\n"
       << "    .setting-group { background: #1e293b; border-radius: 10px; padding: 20px; margin-bottom: 20px; border: 1px solid #334155; }\n"
       << "    .setting-row { display: flex; justify-content: space-between; align-items: center; padding: 12px 0; border-bottom: 1px solid #334155; }\n"
       << "    .setting-row:last-child { border-bottom: none; }\n"
       << "    .label-title { font-weight: bold; font-size: 1.05rem; }\n"
       << "    .label-desc { color: #94a3b8; font-size: 0.85rem; margin-top: 4px; }\n"
       << "    .val { color: #38bdf8; font-family: monospace; font-weight: bold; }\n"
       << "  </style>\n"
       << "</head>\n"
       << "<body>\n"
       << "  <div class=\"container\">\n"
       << "    <h1>إعدادات المتصفح المخصص (" << config.browserName << ")</h1>\n"
       << "    <div class=\"setting-group\">\n"
       << "      <h3>الحماية والخصوصية (C++ Subsystems)</h3>\n"
       << "      <div class=\"setting-row\">\n"
       << "        <div><div class=\"label-title\">مانع الإعلانات والتتبع (AdBlocker Engine)</div><div class=\"label-desc\">حجب سيل الإعلانات وشبكات التجسس التلقائي</div></div>\n"
       << "        <span class=\"val\">" << (config.enableAdBlocker ? "مفعل (Enabled)" : "معطل") << "</span>\n"
       << "      </div>\n"
       << "      <div class=\"setting-row\">\n"
       << "        <div><div class=\"label-title\">الترقية التلقائية لـ HTTPS</div><div class=\"label-desc\">تحويل الروابط غير الآمنة لتشفير SSL/TLS</div></div>\n"
       << "        <span class=\"val\">" << (config.enableHttpsUpgrade ? "مفعل (Enabled)" : "معطل") << "</span>\n"
       << "      </div>\n"
       << "      <div class=\"setting-row\">\n"
       << "        <div><div class=\"label-title\">حذف معلمات التتبع (URL Parameter Stripping)</div><div class=\"label-desc\">تنظيف روابط فيسبوك وجوجل وتويتر من utm_source و fbclid</div></div>\n"
       << "        <span class=\"val\">" << (config.stripTrackingParams ? "مفعل (Enabled)" : "معطل") << "</span>\n"
       << "      </div>\n"
       << "      <div class=\"setting-row\">\n"
       << "        <div><div class=\"label-title\">منع تسريب عنوان IP عبر WebRTC</div><div class=\"label-desc\">حجب كشف IP الحقيقي عبر بروتوكول STUN/ICE</div></div>\n"
       << "        <span class=\"val\">" << (config.preventWebRtcLeak ? "مفعل (Enabled)" : "معطل") << "</span>\n"
       << "      </div>\n"
       << "    </div>\n"
       << "  </div>\n"
       << "</body>\n"
       << "</html>\n";
    return ss.str();
}

std::string SchemeHandlerRegistry::renderStatsHtml(uint64_t totalBlocked, uint64_t requestsChecked) {
    std::ostringstream ss;
    ss << "<!DOCTYPE html>\n"
       << "<html lang=\"ar\" dir=\"rtl\">\n"
       << "<head>\n"
       << "  <meta charset=\"UTF-8\">\n"
       << "  <title>إحصائيات الحماية</title>\n"
       << "  <style>\n"
       << "    body { font-family: sans-serif; background: #0b1120; color: #fff; padding: 40px; }\n"
       << "    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 20px; }\n"
       << "    .card { background: #1e293b; padding: 25px; border-radius: 12px; border: 1px solid #334155; text-align: center; }\n"
       << "    .num { font-size: 3rem; color: #38bdf8; font-weight: bold; }\n"
       << "    .lbl { color: #94a3b8; font-size: 1rem; margin-top: 10px; }\n"
       << "  </style>\n"
       << "</head>\n"
       << "<body>\n"
       << "  <h1>لوحة إحصائيات المتصفح</h1>\n"
       << "  <div class=\"grid\">\n"
       << "    <div class=\"card\"><div class=\"num\">" << totalBlocked << "</div><div class=\"lbl\">إعلانات ومسارات تتبع محجوبة</div></div>\n"
       << "    <div class=\"card\"><div class=\"num\">" << requestsChecked << "</div><div class=\"lbl\">إجمالي الطلبات المفحوصة</div></div>\n"
       << "  </div>\n"
       << "</body>\n"
       << "</html>\n";
    return ss.str();
}

std::string SchemeHandlerRegistry::renderBlockedPageHtml(const std::string& blockedUrl, const std::string& reason) {
    std::ostringstream ss;
    ss << "<!DOCTYPE html>\n"
       << "<html lang=\"ar\" dir=\"rtl\">\n"
       << "<head>\n"
       << "  <meta charset=\"UTF-8\">\n"
       << "  <title>تم حظر الصفحة</title>\n"
       << "  <style>\n"
       << "    body { font-family: sans-serif; background: #1a1a2e; color: #fff; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }\n"
       << "    .box { background: #16213e; border: 2px solid #e94560; padding: 40px; border-radius: 12px; max-width: 600px; text-align: center; }\n"
       << "    h2 { color: #e94560; }\n"
       << "    code { background: #0f3460; padding: 4px 8px; border-radius: 4px; }\n"
       << "  </style>\n"
       << "</head>\n"
       << "<body>\n"
       << "  <div class=\"box\">\n"
       << "    <h2>🛡️ تم حظر الوصول إلى هذا الموقع</h2>\n"
       << "    <p>قام درع حماية المتصفح بحجب الطلب التالي لحمايتك:</p>\n"
       << "    <p><code>" << blockedUrl << "</code></p>\n"
       << "    <p>السبب: <strong>" << reason << "</strong></p>\n"
       << "  </div>\n"
       << "</body>\n"
       << "</html>\n";
    return ss.str();
}

std::string SchemeHandlerRegistry::renderAboutHtml(const BrowserConfig& config) {
    std::ostringstream ss;
    ss << "<!DOCTYPE html>\n"
       << "<html lang=\"ar\" dir=\"rtl\">\n"
       << "<head>\n"
       << "  <meta charset=\"UTF-8\">\n"
       << "  <title>حول " << config.browserName << "</title>\n"
       << "  <style>\n"
       << "    body { font-family: sans-serif; background: #0f172a; color: #fff; padding: 40px; line-height: 1.6; }\n"
       << "    .content { max-width: 700px; margin: 0 auto; background: #1e293b; padding: 30px; border-radius: 12px; }\n"
       << "    h1 { color: #38bdf8; }\n"
       << "  </style>\n"
       << "</head>\n"
       << "<body>\n"
       << "  <div class=\"content\">\n"
       << "    <h1>" << config.browserName << " v" << config.version << "</h1>\n"
       << "    <p>متصفح مصمم بتقنيات C++ الحديثة مع دعم ميزات الخصوصية والتحكم التام في دورة حياة الويب.</p>\n"
       << "    <ul>\n"
       << "      <li><strong>محرك النواة:</strong> Chromium / Blink Core Compatible</li>\n"
       << "      <li><strong>لغة التطوير:</strong> C++20</li>\n"
       << "      <li><strong>التحكم في الشبكة:</strong> Custom Interception Engine</li>\n"
       << "    </ul>\n"
       << "  </div>\n"
       << "</body>\n"
       << "</html>\n";
    return ss.str();
}

} // namespace BrowserCore
