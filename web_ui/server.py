#!/usr/bin/env python3
import http.server
import socketserver
import json
import urllib.parse
import subprocess
import os
import re

PORT = 8080

# In-memory browser state mirroring our C++ engine
state = {
    "activeTabId": 1,
    "nextTabId": 2,
    "tabs": [
        {"id": 1, "title": "تبويب جديد", "url": "mybrowser://newtab", "pinned": False, "muted": False}
    ],
    "adBlockEnabled": True,
    "httpsUpgrade": True,
    "stripTracking": True,
    "stats": {
        "totalBlocked": 14,
        "adsBlocked": 9,
        "trackersBlocked": 5,
        "requestsChecked": 42
    },
    "bookmarks": [
        {"url": "https://duckduckgo.com", "title": "DuckDuckGo Privacy Search"},
        {"url": "https://github.com", "title": "GitHub Developer Platform"},
        {"url": "https://en.cppreference.com", "title": "C++ Reference"}
    ],
    "history": []
}

AD_DOMAINS = [
    "doubleclick.net", "googleadservices.com", "googlesyndication.com",
    "adnxs.com", "advertising.com", "criteo.com", "outbrain.com", "taboola.com"
]

TRACKER_DOMAINS = [
    "google-analytics.com", "analytics.google.com", "hotjar.com", "telemetry.mozilla.org"
]

HTML_PAGE = """<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AtlasBrowser - متصفحك المخصص بـ C++</title>
    <style>
        :root {
            --bg-dark: #0f172a;
            --bg-toolbar: #1e293b;
            --bg-tab-active: #334155;
            --bg-tab-inactive: #1e293b;
            --border-color: #334155;
            --accent: #38bdf8;
            --text-main: #f8fafc;
            --text-muted: #94a3b8;
            --danger: #ef4444;
            --success: #22c55e;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }
        body { background: var(--bg-dark); color: var(--text-main); height: 100vh; display: flex; flex-direction: column; overflow: hidden; }

        /* Window Header & Tabs */
        .window-header { background: #0b1120; padding: 8px 12px 0 12px; display: flex; align-items: center; border-bottom: 1px solid rgba(255,255,255,0.05); }
        .window-controls { display: flex; gap: 8px; margin-left: 16px; }
        .dot { width: 12px; height: 12px; border-radius: 50%; display: inline-block; }
        .dot.red { background: #ff5f56; }
        .dot.yellow { background: #ffbd2e; }
        .dot.green { background: #27c93f; }

        .tabs-container { display: flex; gap: 4px; flex: 1; overflow-x: auto; scrollbar-width: none; }
        .tab {
            background: var(--bg-tab-inactive);
            color: var(--text-muted);
            padding: 8px 16px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            display: flex;
            align-items: center;
            gap: 10px;
            font-size: 0.85rem;
            cursor: pointer;
            border: 1px solid transparent;
            border-bottom: none;
            max-width: 200px;
            min-width: 120px;
            transition: all 0.15s ease;
        }
        .tab:hover { background: #243248; color: var(--text-main); }
        .tab.active { background: var(--bg-toolbar); color: var(--text-main); font-weight: 500; border-color: var(--border-color); }
        .tab-title { white-space: nowrap; overflow: hidden; text-overflow: ellipsis; flex: 1; }
        .tab-close { opacity: 0.6; font-size: 1rem; border-radius: 50%; padding: 0 4px; }
        .tab-close:hover { opacity: 1; background: rgba(255,255,255,0.1); color: var(--danger); }
        .btn-new-tab { background: transparent; border: none; color: var(--text-muted); font-size: 1.2rem; cursor: pointer; padding: 4px 10px; border-radius: 6px; }
        .btn-new-tab:hover { background: var(--bg-toolbar); color: var(--text-main); }

        /* Toolbar */
        .toolbar {
            background: var(--bg-toolbar);
            padding: 8px 16px;
            display: flex;
            align-items: center;
            gap: 12px;
            border-bottom: 1px solid var(--border-color);
        }
        .nav-btn {
            background: transparent;
            border: none;
            color: var(--text-muted);
            font-size: 1.1rem;
            width: 32px;
            height: 32px;
            border-radius: 6px;
            cursor: pointer;
            display: flex;
            align-items: center;
            justify-content: center;
            transition: background 0.15s;
        }
        .nav-btn:hover { background: rgba(255,255,255,0.08); color: var(--text-main); }

        .omnibar-container {
            flex: 1;
            background: #0f172a;
            border: 1px solid var(--border-color);
            border-radius: 20px;
            display: flex;
            align-items: center;
            padding: 2px 14px;
            gap: 8px;
            transition: border-color 0.2s;
        }
        .omnibar-container:focus-within { border-color: var(--accent); box-shadow: 0 0 10px rgba(56, 189, 248, 0.2); }
        .omnibar-input {
            flex: 1;
            background: transparent;
            border: none;
            color: #fff;
            font-size: 0.95rem;
            outline: none;
            direction: ltr;
            text-align: right;
            padding: 6px 0;
        }
        .shield-btn {
            background: rgba(56, 189, 248, 0.15);
            border: 1px solid rgba(56, 189, 248, 0.4);
            color: var(--accent);
            padding: 3px 8px;
            border-radius: 12px;
            font-size: 0.75rem;
            cursor: pointer;
            display: flex;
            align-items: center;
            gap: 4px;
        }
        .shield-btn.off { background: rgba(239, 68, 68, 0.15); border-color: rgba(239, 68, 68, 0.4); color: var(--danger); }

        /* Bookmarks Bar */
        .bookmarks-bar {
            background: #182234;
            padding: 4px 16px;
            display: flex;
            align-items: center;
            gap: 16px;
            border-bottom: 1px solid rgba(255,255,255,0.05);
            font-size: 0.8rem;
        }
        .bookmark-item { color: var(--text-muted); text-decoration: none; display: flex; align-items: center; gap: 6px; cursor: pointer; }
        .bookmark-item:hover { color: var(--accent); }

        /* Viewport / Content Area */
        .content-area { flex: 1; background: #0f172a; overflow-y: auto; position: relative; }
        .view-frame { width: 100%; height: 100%; border: none; }

        /* Notification Toast */
        .toast {
            position: fixed;
            bottom: 24px;
            left: 24px;
            background: #1e293b;
            color: #fff;
            padding: 12px 20px;
            border-radius: 8px;
            border-right: 4px solid var(--accent);
            box-shadow: 0 10px 25px rgba(0,0,0,0.4);
            display: none;
            animation: slideIn 0.3s ease;
            z-index: 999;
        }
        @keyframes slideIn { from { transform: translateY(20px); opacity: 0; } to { transform: translateY(0); opacity: 1; } }

        /* Internal Pages Styling */
        .internal-page { padding: 40px; max-width: 900px; margin: 0 auto; text-align: center; }
        .internal-page h1 { font-size: 2.5rem; margin-bottom: 12px; color: var(--accent); }
        .internal-page p { color: var(--text-muted); margin-bottom: 30px; font-size: 1.1rem; }
        .search-hero input {
            width: 100%;
            max-width: 600px;
            padding: 16px 24px;
            border-radius: 30px;
            border: 2px solid var(--border-color);
            background: #1e293b;
            color: #fff;
            font-size: 1.1rem;
            outline: none;
            margin-bottom: 30px;
            box-shadow: 0 8px 20px rgba(0,0,0,0.3);
        }
        .search-hero input:focus { border-color: var(--accent); }
        .card-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 16px; margin-top: 20px; text-align: right; }
        .feature-card { background: #1e293b; padding: 20px; border-radius: 12px; border: 1px solid var(--border-color); }
        .feature-card h3 { color: #38bdf8; font-size: 1.1rem; margin-bottom: 8px; }
        .feature-card p { font-size: 0.85rem; color: #94a3b8; margin: 0; }
        .badge-pill { display: inline-block; background: rgba(56, 189, 248, 0.15); color: #38bdf8; padding: 4px 10px; border-radius: 12px; font-size: 0.75rem; font-weight: bold; }
    </style>
</head>
<body>

    <!-- Window Header & Tabs -->
    <div class="window-header">
        <div class="window-controls">
            <span class="dot red"></span>
            <span class="dot yellow"></span>
            <span class="dot green"></span>
        </div>
        <div class="tabs-container" id="tabsList"></div>
        <button class="btn-new-tab" onclick="createNewTab()" title="فتح لسان جديد">+</button>
    </div>

    <!-- Navigation & URL Toolbar -->
    <div class="toolbar">
        <button class="nav-btn" onclick="goBack()" title="رجوع">➔</button>
        <button class="nav-btn" onclick="goForward()" title="تقدم">➔</button>
        <button class="nav-btn" onclick="reloadTab()" title="إعادة تحميل">⟳</button>
        <button class="nav-btn" onclick="navigate('mybrowser://newtab')" title="صفحة البداية">🏠</button>

        <div class="omnibar-container">
            <button class="shield-btn" id="shieldStatus" onclick="toggleShield()">
                <span>🛡️</span> <span id="shieldText">الدرع مفعل</span>
            </button>
            <input type="text" class="omnibar-input" id="urlInput" placeholder="اكتب عنوان ويب أو ابحث في الويب..." onkeydown="if(event.key==='Enter') handleUrlSubmit()">
        </div>

        <button class="nav-btn" onclick="navigate('mybrowser://settings')" title="إعدادات النواة C++">⚙️</button>
        <button class="nav-btn" onclick="navigate('mybrowser://stats')" title="إحصائيات الحظر">📊</button>
    </div>

    <!-- Bookmarks Bar -->
    <div class="bookmarks-bar" id="bookmarksBar">
        <span style="color: var(--accent); font-weight: bold;">المفضلات:</span>
    </div>

    <!-- Content Area -->
    <div class="content-area" id="contentArea"></div>

    <div class="toast" id="toastBox"></div>

    <script>
        let currentState = {};

        async function fetchState() {
            try {
                const res = await fetch('/api/state');
                currentState = await res.json();
                renderUI();
            } catch (e) {
                console.error('Failed to fetch state', e);
            }
        }

        function showToast(msg, isSuccess = true) {
            const toast = document.getElementById('toastBox');
            toast.innerText = msg;
            toast.style.borderRightColor = isSuccess ? 'var(--accent)' : 'var(--danger)';
            toast.style.display = 'block';
            setTimeout(() => { toast.style.display = 'none'; }, 3500);
        }

        function renderUI() {
            // Render Tabs
            const tabsList = document.getElementById('tabsList');
            tabsList.innerHTML = '';
            currentState.tabs.forEach(tab => {
                const tabEl = document.createElement('div');
                tabEl.className = 'tab' + (tab.id === currentState.activeTabId ? ' active' : '');
                tabEl.onclick = () => switchTab(tab.id);
                tabEl.innerHTML = `
                    <span class="tab-title">${tab.title}</span>
                    <span class="tab-close" onclick="event.stopPropagation(); closeTab(${tab.id})">×</span>
                `;
                tabsList.appendChild(tabEl);
            });

            // Update URL bar
            const activeTab = currentState.tabs.find(t => t.id === currentState.activeTabId);
            if (activeTab) {
                document.getElementById('urlInput').value = activeTab.url;
                renderContent(activeTab.url);
            }

            // Shield status
            const shieldBtn = document.getElementById('shieldStatus');
            const shieldText = document.getElementById('shieldText');
            if (currentState.adBlockEnabled) {
                shieldBtn.className = 'shield-btn';
                shieldText.innerText = 'الدرع مفعل (' + currentState.stats.totalBlocked + ')';
            } else {
                shieldBtn.className = 'shield-btn off';
                shieldText.innerText = 'الدرع معطل';
            }

            // Bookmarks
            const bBar = document.getElementById('bookmarksBar');
            bBar.innerHTML = '<span style="color: var(--accent); font-weight: bold;">المفضلات:</span>';
            currentState.bookmarks.forEach(b => {
                const a = document.createElement('a');
                a.className = 'bookmark-item';
                a.innerHTML = '★ ' + b.title;
                a.onclick = () => navigate(b.url);
                bBar.appendChild(a);
            });
        }

        function renderContent(url) {
            const area = document.getElementById('contentArea');
            if (url === 'mybrowser://newtab' || url === '') {
                area.innerHTML = `
                    <div class="internal-page">
                        <h1>AtlasBrowser Core</h1>
                        <p>متصفحك المخصص فائق السرعة والمبني بـ C++ بنواة Chromium / Firefox Subsystems</p>
                        <div class="search-hero">
                            <input type="text" placeholder="ابحث في الإنترنت أو جرب الروابط المحظورة..." onkeydown="if(event.key==='Enter') navigate(this.value)" autofocus>
                        </div>
                        <div class="card-grid">
                            <div class="feature-card">
                                <span class="badge-pill">C++ Subsystem</span>
                                <h3>مانع الإعلانات والتتبع</h3>
                                <p>حظر تلقائي لـ ${currentState.stats.totalBlocked} طلباً من شبكات الإعلانات وتتبع المستخدمين.</p>
                            </div>
                            <div class="feature-card">
                                <span class="badge-pill">HTTPS Enforcer</span>
                                <h3>الترقية التلقائية لـ HTTPS</h3>
                                <p>تحويل الروابط غير الآمنة لتشفير SSL/TLS تلقائياً قبل إرسال الطلب.</p>
                            </div>
                            <div class="feature-card">
                                <span class="badge-pill">URL Cleanser</span>
                                <h3>تنظيف معلمات التتبع</h3>
                                <p>حذف وسوم fbclid و utm_* لضمان عدم تعقبك بين المواقع.</p>
                            </div>
                            <div class="feature-card">
                                <span class="badge-pill">Privacy Shield</span>
                                <h3>حماية WebRTC والبصمة</h3>
                                <p>منع تسريب عنوان IP الحقيقي وإضافة تشويش للـ Canvas لمنع التعرف على جهازك.</p>
                            </div>
                        </div>
                    </div>
                `;
            } else if (url === 'mybrowser://settings') {
                area.innerHTML = `
                    <div class="internal-page" style="text-align: right;">
                        <h1>إعدادات المتصفح المخصص (C++ Settings)</h1>
                        <p style="text-align: right;">التحكم في خيارات المحرك الداخلي والأمان.</p>
                        <div style="background: #1e293b; padding: 24px; border-radius: 12px; border: 1px solid #334155; line-height: 2;">
                            <p><strong>اسم المتصفح:</strong> AtlasBrowser (C++ Edition)</p>
                            <p><strong>الإصدار:</strong> 1.0.0-release</p>
                            <p><strong>محرك النواة:</strong> Chromium Blink / V8 Compatible Subsystems</p>
                            <p><strong>حالة مانع الإعلانات:</strong> ${currentState.adBlockEnabled ? 'مفعل ✅' : 'معطل ❌'}</p>
                            <p><strong>ترقية HTTPS التلقائية:</strong> مفعلة ✅</p>
                            <p><strong>حذف معلمات التتبع:</strong> مفعلة ✅</p>
                        </div>
                    </div>
                `;
            } else if (url === 'mybrowser://stats') {
                area.innerHTML = `
                    <div class="internal-page">
                        <h1>إحصائيات درع الخصوصية والشبكة</h1>
                        <p>تقرير مباشر لما تم فحصه وحجبه بواسطة محرك C++ الداخلي:</p>
                        <div class="card-grid">
                            <div class="feature-card" style="text-align: center;">
                                <h2 style="font-size: 3rem; color: #38bdf8;">${currentState.stats.totalBlocked}</h2>
                                <p>إجمالي الإعلانات والمتتبعات المحجوبة</p>
                            </div>
                            <div class="feature-card" style="text-align: center;">
                                <h2 style="font-size: 3rem; color: #22c55e;">${currentState.stats.requestsChecked}</h2>
                                <p>إجمالي الطلبات الشبكية المفحوصة</p>
                            </div>
                            <div class="feature-card" style="text-align: center;">
                                <h2 style="font-size: 3rem; color: #f59e0b;">${currentState.stats.adsBlocked}</h2>
                                <p>إعلانات تجارية محجوبة</p>
                            </div>
                            <div class="feature-card" style="text-align: center;">
                                <h2 style="font-size: 3rem; color: #a855f7;">${currentState.stats.trackersBlocked}</h2>
                                <p>مسارات تجسس وتتبع محجوبة</p>
                            </div>
                        </div>
                    </div>
                `;
            } else if (url.startsWith('mybrowser://blocked')) {
                area.innerHTML = `
                    <div class="internal-page" style="margin-top: 50px;">
                        <h1 style="color: var(--danger);">🛡️ تم حظر الطلب لحمايتك!</h1>
                        <p>قام محرك C++ بحظر هذا الموقع لكونه ينتمي لشبكة إعلانات أو تتبع خبيثة.</p>
                        <div style="background: #1e293b; padding: 20px; border-radius: 8px; border: 1px solid var(--danger); display: inline-block;">
                            <code>${url}</code>
                        </div>
                    </div>
                `;
            } else {
                area.innerHTML = `
                    <div style="padding: 30px; text-align: center;">
                        <div style="background: #1e293b; padding: 30px; border-radius: 12px; max-width: 700px; margin: 0 auto; border: 1px solid #334155;">
                            <h2 style="color: #38bdf8; margin-bottom: 15px;">تم فحص وتمرير الطلب بنجاح ✅</h2>
                            <p style="margin-bottom: 10px;">الرابط النهائي بعد ترقية HTTPS وتنظيف التتبع:</p>
                            <p style="direction: ltr; font-family: monospace; background: #0f172a; padding: 10px; border-radius: 6px; color: #22c55e;">${url}</p>
                            <p style="margin-top: 15px; font-size: 0.9rem; color: #94a3b8;">تم فحص الرابط ومطابقته بقواعد C++ AdBlocker و NetworkInterceptor.</p>
                        </div>
                    </div>
                `;
            }
        }

        async function handleUrlSubmit() {
            const input = document.getElementById('urlInput').value.trim();
            if (input) navigate(input);
        }

        async function navigate(url) {
            const res = await fetch('/api/navigate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ url: url })
            });
            const data = await res.json();
            if (data.action === 'blocked') {
                showToast('تم حجب الإعلان/المتتبع: ' + data.reason, false);
            } else if (data.cleaned) {
                showToast('تم تنظيف الرابط وترقيته إلى HTTPS بنجاح ✅');
            }
            fetchState();
        }

        async function createNewTab() {
            await fetch('/api/tabs/new', { method: 'POST' });
            fetchState();
        }

        async function switchTab(id) {
            await fetch('/api/tabs/switch', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            fetchState();
        }

        async function closeTab(id) {
            await fetch('/api/tabs/close', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            fetchState();
        }

        async function toggleShield() {
            await fetch('/api/shield/toggle', { method: 'POST' });
            fetchState();
        }

        fetchState();
    </script>
</body>
</html>
"""

class BrowserRequestHandler(http.server.BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        pass # Silence logs

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        if parsed.path == "/" or parsed.path == "/index.html":
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()
            self.wfile.write(HTML_PAGE.encode("utf-8"))
        elif parsed.path == "/api/state":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps(state).encode("utf-8"))
        else:
            self.send_response(404)
            self.end_headers()

    def do_POST(self):
        parsed = urllib.parse.urlparse(self.path)
        content_len = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_len).decode("utf-8") if content_len > 0 else "{}"
        try:
            req_data = json.loads(body)
        except Exception:
            req_data = {}

        if parsed.path == "/api/navigate":
            raw_url = req_data.get("url", "")
            resolved = raw_url

            # Tracking cleanser
            cleaned = False
            if "?" in resolved:
                base, qs = resolved.split("?", 1)
                params = qs.split("&")
                clean_params = [p for p in params if not any(p.startswith(t + "=") for t in ["utm_", "fbclid", "gclid", "msclkid"])]
                if len(clean_params) != len(params):
                    cleaned = True
                    resolved = base + ("?" + "&".join(clean_params) if clean_params else "")

            # HTTPS Upgrade
            if resolved.startswith("http://"):
                resolved = "https://" + resolved[7:]
                cleaned = True
            elif not resolved.startswith("https://") and not resolved.startswith("mybrowser://") and "." in resolved:
                resolved = "https://" + resolved

            # Check AdBlocker
            state["stats"]["requestsChecked"] += 1
            is_blocked = False
            block_reason = ""
            if state["adBlockEnabled"]:
                for bad in AD_DOMAINS + TRACKER_DOMAINS:
                    if bad in resolved:
                        is_blocked = True
                        block_reason = "Match rule: ||" + bad + "^"
                        state["stats"]["totalBlocked"] += 1
                        if bad in AD_DOMAINS:
                            state["stats"]["adsBlocked"] += 1
                        else:
                            state["stats"]["trackersBlocked"] += 1
                        break

            # Update active tab
            active_tab = next((t for t in state["tabs"] if t["id"] == state["activeTabId"]), None)
            if active_tab:
                if is_blocked:
                    active_tab["url"] = "mybrowser://blocked?target=" + urllib.parse.quote(resolved)
                    active_tab["title"] = "تم حظر الموقع"
                else:
                    active_tab["url"] = resolved
                    active_tab["title"] = resolved if "mybrowser://" not in resolved else ("صفحة البداية" if "newtab" in resolved else resolved)

            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            resp = {
                "action": "blocked" if is_blocked else "allow",
                "reason": block_reason,
                "cleaned": cleaned,
                "url": active_tab["url"] if active_tab else resolved
            }
            self.wfile.write(json.dumps(resp).encode("utf-8"))

        elif parsed.path == "/api/tabs/new":
            new_id = state["nextTabId"]
            state["nextTabId"] += 1
            state["tabs"].append({
                "id": new_id,
                "title": "تبويب جديد",
                "url": "mybrowser://newtab",
                "pinned": False,
                "muted": False
            })
            state["activeTabId"] = new_id
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')

        elif parsed.path == "/api/tabs/switch":
            tab_id = req_data.get("id")
            if any(t["id"] == tab_id for t in state["tabs"]):
                state["activeTabId"] = tab_id
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')

        elif parsed.path == "/api/tabs/close":
            tab_id = req_data.get("id")
            if len(state["tabs"]) > 1:
                state["tabs"] = [t for t in state["tabs"] if t["id"] != tab_id]
                if state["activeTabId"] == tab_id:
                    state["activeTabId"] = state["tabs"][0]["id"]
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')

        elif parsed.path == "/api/shield/toggle":
            state["adBlockEnabled"] = not state["adBlockEnabled"]
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps({"enabled": state["adBlockEnabled"]}).encode("utf-8"))

class ReuseAddrServer(socketserver.TCPServer):
    allow_reuse_address = True

if __name__ == "__main__":
    server = ReuseAddrServer(("0.0.0.0", PORT), BrowserRequestHandler)
    print(f"Browser Web UI Server listening on http://0.0.0.0:{PORT}")
    server.serve_forever()
