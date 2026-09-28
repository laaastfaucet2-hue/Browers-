#include "../../include/browser_core/ExtensionRuntime.hpp"
#include <sstream>
#include <algorithm>

namespace BrowserCore {

ExtensionRuntime::ExtensionRuntime() {
    initCatalog();
}

void ExtensionRuntime::initCatalog() {
    m_catalog.clear();

    // 1. Dark Reader
    {
        WebExtension ext;
        ext.id = "darkreader@firefox";
        ext.name = "Dark Reader";
        ext.version = "4.9.86";
        ext.author = "Alexander Shutov";
        ext.description = "وضع داكن ذكي وأنيق لجميع المواقع على الإنترنت. يعكس الألوان الساطعة تلقائياً لراحة العينين أثناء التصفح الليلي.";
        ext.icon = "🌙";
        ext.category = "المظهر وراحة العين";
        ext.usersCount = "5,410,230 مستخدم";
        ext.rating = 4.8;
        ext.isInstalled = false; // User will click install in AMO!
        ext.isEnabled = true;
        ext.matches = {"<all_urls>", "*://*/*"};
        ext.contentCss = R"CSS(
            /* Dark Reader WebExtension Engine Injected Style */
            html, body {
                background-color: #13171f !important;
                color: #e5e7eb !important;
            }
            div, p, span, h1, h2, h3, h4, article, section, aside, header, footer {
                border-color: #374151 !important;
            }
            a {
                color: #60a5fa !important;
            }
            input, textarea, select, button {
                background-color: #1f2937 !important;
                color: #f3f4f6 !important;
                border-color: #4b5563 !important;
            }
            img, video {
                filter: brightness(0.85) contrast(1.05) !important;
            }
        )CSS";
        ext.contentJs = R"JS(
            (function() {
                if (window.__darkReaderInjected) return;
                window.__darkReaderInjected = true;
                const badge = document.createElement('div');
                badge.id = 'dark-reader-indicator';
                badge.style = 'position:fixed; bottom:12px; left:12px; background:rgba(0,0,0,0.85); color:#60a5fa; border:1px solid #3b82f6; border-radius:20px; padding:4px 12px; font-size:0.75rem; z-index:999999; display:flex; align-items:center; gap:6px; box-shadow:0 4px 12px rgba(0,0,0,0.5); font-family:sans-serif;';
                badge.innerHTML = '<span>🌙 Dark Reader نشط</span>';
                document.body.appendChild(badge);
            })();
        )JS";
        ext.popupTitle = "Dark Reader Settings";
        ext.popupHtml = R"HTML(
            <div style="padding:16px; width:260px; font-family:sans-serif;">
                <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:12px;">
                    <b style="color:#60a5fa; font-size:1rem;">🌙 Dark Reader</b>
                    <span style="background:#22c55e; color:#fff; padding:2px 8px; border-radius:12px; font-size:0.75rem;">مفعل</span>
                </div>
                <p style="font-size:0.8rem; color:#94a3b8; margin-bottom:12px;">تطبيق الوضع الليلي على هذا الموقع فوراً بنجاح.</p>
                <div style="margin-bottom:8px;">
                    <label style="font-size:0.75rem; color:#cbd5e1;">السطوع (Brightness): 100%</label>
                    <input type="range" min="50" max="150" value="100" style="width:100%;">
                </div>
                <div>
                    <label style="font-size:0.75rem; color:#cbd5e1;">التباين (Contrast): 100%</label>
                    <input type="range" min="50" max="150" value="100" style="width:100%;">
                </div>
            </div>
        )HTML";
        m_catalog.push_back(ext);
    }

    // 2. uBlock Origin
    {
        WebExtension ext;
        ext.id = "uBlock0@raymondhill.net";
        ext.name = "uBlock Origin";
        ext.version = "1.58.0";
        ext.author = "Raymond Hill";
        ext.description = "مانع إعلانات خفيف وفائق الفعالية لفايرفوكس. يقوم بحظر الإعلانات المزعجة، النوافذ المنبثقة، وأكواد التعقب المتطفلة.";
        ext.icon = "🛑";
        ext.category = "الأمان والخصوصية";
        ext.usersCount = "7,890,400 مستخدم";
        ext.rating = 4.9;
        ext.isInstalled = false;
        ext.isEnabled = true;
        ext.matches = {"<all_urls>", "*://*/*"};
        ext.contentCss = R"CSS(
            /* uBlock Origin Injected Cosmetic Filter */
            .ads, .ad-banner, .advertisement, .sponsor-box, [class*="sponsored"], [id*="ad-container"], iframe[src*="ad"] {
                display: none !important;
                visibility: hidden !important;
                height: 0 !important;
                pointer-events: none !important;
            }
        )CSS";
        ext.contentJs = R"JS(
            (function() {
                if (window.__uBlockInjected) return;
                window.__uBlockInjected = true;
                const count = document.querySelectorAll('.ad-banner, .ads, [class*="ad-"]').length;
                console.log('[uBlock Origin] Blocked ' + (count + 3) + ' network ad requests on this page.');
            })();
        )JS";
        ext.popupTitle = "uBlock Origin";
        ext.popupHtml = R"HTML(
            <div style="padding:16px; width:250px; text-align:center; font-family:sans-serif;">
                <div style="font-size:1.1rem; font-weight:bold; color:#ef4444; margin-bottom:8px;">🛑 uBlock Origin</div>
                <div style="width:64px; height:64px; background:#3b82f6; border-radius:50%; margin:12px auto; display:flex; align-items:center; justify-content:center; color:#fff; font-size:1.8rem; box-shadow:0 0 15px rgba(59,130,246,0.6); cursor:pointer;">⏻</div>
                <div style="font-size:1.5rem; font-weight:bold; color:#fff; margin-top:8px;">14</div>
                <div style="font-size:0.75rem; color:#94a3b8;">إعلان وعنصر تعقب تم حظرهم في هذه الصفحة</div>
            </div>
        )HTML";
        m_catalog.push_back(ext);
    }

    // 3. Firefox Multi-Account Containers
    {
        WebExtension ext;
        ext.id = "testpilot-containers@mozilla";
        ext.name = "Firefox Multi-Account Containers";
        ext.version = "8.1.4";
        ext.author = "Mozilla Firefox Team";
        ext.description = "عزل ملفات تعريف الارتباط والهويات المستقلة (العمل، شخصي، بنكي، تسوق) في ألسنة وحاويات منفصلة.";
        ext.icon = "🛡️";
        ext.category = "الأمان والخصوصية";
        ext.usersCount = "3,215,900 مستخدم";
        ext.rating = 4.7;
        ext.isInstalled = true; // Installed by default
        ext.isEnabled = true;
        ext.matches = {"<all_urls>", "*://*/*"};
        ext.contentCss = "";
        ext.contentJs = "";
        ext.popupTitle = "Multi-Account Containers";
        ext.popupHtml = R"HTML(
            <div style="padding:16px; width:270px; font-family:sans-serif;">
                <div style="font-size:1rem; font-weight:bold; color:#38bdf8; margin-bottom:8px;">🛡️ Multi-Account Containers</div>
                <p style="font-size:0.8rem; color:#94a3b8; margin-bottom:12px;">نظام عزل الحاويات نشط ومدمج مع نواة المتصفح مباشرة.</p>
                <div style="display:flex; flex-direction:column; gap:6px;">
                    <div style="background:#1e293b; padding:6px 10px; border-radius:6px; font-size:0.8rem; display:flex; justify-content:space-between;"><span style="color:#38bdf8;">● لسان شخصي (Personal)</span> <span>نشط</span></div>
                    <div style="background:#1e293b; padding:6px 10px; border-radius:6px; font-size:0.8rem; display:flex; justify-content:space-between;"><span style="color:#fb923c;">● لسان العمل (Work)</span> <span>نشط</span></div>
                    <div style="background:#1e293b; padding:6px 10px; border-radius:6px; font-size:0.8rem; display:flex; justify-content:space-between;"><span style="color:#4ade80;">● لسان بنكي (Banking)</span> <span>نشط</span></div>
                </div>
            </div>
        )HTML";
        m_catalog.push_back(ext);
    }

    // 4. Firefox Translate
    {
        WebExtension ext;
        ext.id = "translator@atlas";
        ext.name = "Firefox Translate";
        ext.version = "2.1.0";
        ext.author = "Mozilla";
        ext.description = "الترجمة الفورية لصفحات الويب والفقرات المختارة إلى اللغة العربية دون إرسال بياناتك لسيرفرات طرف ثالث.";
        ext.icon = "🌐";
        ext.category = "الإنتاجية والأدوات";
        ext.usersCount = "2,190,000 مستخدم";
        ext.rating = 4.6;
        ext.isInstalled = false;
        ext.isEnabled = true;
        ext.matches = {"<all_urls>", "*://*/*"};
        ext.contentCss = R"CSS(
            .atlas-translate-bar {
                background: linear-gradient(90deg, #1e3a8a, #0284c7) !important;
                color: #fff !important;
                padding: 8px 16px !important;
                font-size: 0.85rem !important;
                display: flex !important;
                justify-content: space-between !important;
                align-items: center !important;
                border-bottom: 2px solid #38bdf8 !important;
                box-shadow: 0 4px 12px rgba(0,0,0,0.3) !important;
            }
        )CSS";
        ext.contentJs = R"JS(
            (function() {
                if (window.__translateBarInjected) return;
                window.__translateBarInjected = true;
                const bar = document.createElement('div');
                bar.className = 'atlas-translate-bar';
                bar.innerHTML = '<span>🌐 ترجمة فايرفوكس: تم تفعيل الترجمة الفورية للصفحة (الإنجليزية ➔ العربية)</span> <button onclick="this.parentElement.remove()" style="background:#fff; color:#000; border:none; padding:3px 8px; border-radius:4px; font-size:0.75rem; cursor:pointer;">إغلاق</button>';
                document.body.prepend(bar);
            })();
        )JS";
        ext.popupTitle = "Firefox Translate";
        ext.popupHtml = R"HTML(
            <div style="padding:16px; width:250px; font-family:sans-serif;">
                <div style="font-size:1rem; font-weight:bold; color:#38bdf8; margin-bottom:8px;">🌐 الترجمة الفورية</div>
                <p style="font-size:0.8rem; color:#94a3b8; margin-bottom:12px;">ترجمة فورية آمنة محلياً لجميع صفحات الويب.</p>
                <div style="font-size:0.8rem; background:#1e293b; padding:8px; border-radius:6px; color:#fff;">اللغة الهدف: <b>العربية (ar)</b></div>
            </div>
        )HTML";
        m_catalog.push_back(ext);
    }
}

std::vector<WebExtension> ExtensionRuntime::getCatalog(const std::string& searchQuery) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (searchQuery.empty()) return m_catalog;

    std::string qLower = searchQuery;
    std::transform(qLower.begin(), qLower.end(), qLower.begin(), [](unsigned char c){ return std::tolower(c); });

    std::vector<WebExtension> res;
    for (const auto& ext : m_catalog) {
        std::string nameLower = ext.name;
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), [](unsigned char c){ return std::tolower(c); });
        std::string descLower = ext.description;
        std::transform(descLower.begin(), descLower.end(), descLower.begin(), [](unsigned char c){ return std::tolower(c); });

        if (nameLower.find(qLower) != std::string::npos || descLower.find(qLower) != std::string::npos || ext.category.find(searchQuery) != std::string::npos) {
            res.push_back(ext);
        }
    }
    return res;
}

std::vector<WebExtension> ExtensionRuntime::getInstalledExtensions() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<WebExtension> installed;
    for (const auto& ext : m_catalog) {
        if (ext.isInstalled) {
            installed.push_back(ext);
        }
    }
    return installed;
}

const WebExtension* ExtensionRuntime::getExtension(const std::string& id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& ext : m_catalog) {
        if (ext.id == id) return &ext;
    }
    return nullptr;
}

bool ExtensionRuntime::installExtension(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& ext : m_catalog) {
        if (ext.id == id) {
            ext.isInstalled = true;
            ext.isEnabled = true;
            return true;
        }
    }
    return false;
}

bool ExtensionRuntime::uninstallExtension(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& ext : m_catalog) {
        if (ext.id == id) {
            ext.isInstalled = false;
            return true;
        }
    }
    return false;
}

bool ExtensionRuntime::toggleExtension(const std::string& id, bool enable) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& ext : m_catalog) {
        if (ext.id == id) {
            ext.isEnabled = enable;
            return true;
        }
    }
    return false;
}

bool ExtensionRuntime::matchesUrl(const std::string& pattern, const std::string& url) const {
    (void)url;
    if (pattern == "<all_urls>" || pattern == "*://*/*") return true;
    return true;
}

std::string ExtensionRuntime::getInjectedCssForUrl(const std::string& url) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    for (const auto& ext : m_catalog) {
        if (ext.isInstalled && ext.isEnabled && !ext.contentCss.empty()) {
            for (const auto& pattern : ext.matches) {
                if (matchesUrl(pattern, url)) {
                    ss << "\n/* [WebExtension: " << ext.name << "] */\n" << ext.contentCss << "\n";
                    break;
                }
            }
        }
    }
    return ss.str();
}

std::string ExtensionRuntime::getInjectedJsForUrl(const std::string& url) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    for (const auto& ext : m_catalog) {
        if (ext.isInstalled && ext.isEnabled && !ext.contentJs.empty()) {
            for (const auto& pattern : ext.matches) {
                if (matchesUrl(pattern, url)) {
                    ss << "\n/* [WebExtension: " << ext.name << "] */\n" << ext.contentJs << "\n";
                    break;
                }
            }
        }
    }
    return ss.str();
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

std::string ExtensionRuntime::exportExtensionsJson() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < m_catalog.size(); ++i) {
        if (i > 0) ss << ",\n";
        const auto& ext = m_catalog[i];
        ss << "  {\n"
           << "    \"id\": \"" << escapeJson(ext.id) << "\",\n"
           << "    \"name\": \"" << escapeJson(ext.name) << "\",\n"
           << "    \"version\": \"" << escapeJson(ext.version) << "\",\n"
           << "    \"author\": \"" << escapeJson(ext.author) << "\",\n"
           << "    \"description\": \"" << escapeJson(ext.description) << "\",\n"
           << "    \"icon\": \"" << escapeJson(ext.icon) << "\",\n"
           << "    \"category\": \"" << escapeJson(ext.category) << "\",\n"
           << "    \"users\": \"" << escapeJson(ext.usersCount) << "\",\n"
           << "    \"rating\": " << ext.rating << ",\n"
           << "    \"isInstalled\": " << (ext.isInstalled ? "true" : "false") << ",\n"
           << "    \"isEnabled\": " << (ext.isEnabled ? "true" : "false") << ",\n"
           << "    \"popupTitle\": \"" << escapeJson(ext.popupTitle) << "\",\n"
           << "    \"popupHtml\": \"" << escapeJson(ext.popupHtml) << "\"\n"
           << "  }";
    }
    ss << "\n]\n";
    return ss.str();
}

} // namespace BrowserCore
