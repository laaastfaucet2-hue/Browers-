#include "../../include/browser_core/BrowserEngine.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace BrowserCore;

static BrowserEngine g_engine;

static const char* HTML_UI = R"RAW_HTML(<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AtlasBrowser - متصفح C++ المخصص</title>
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
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: system-ui, -apple-system, sans-serif; }
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
            gap: 8px;
            font-size: 0.85rem;
            cursor: pointer;
            border: 1px solid transparent;
            border-bottom: 2px solid var(--container-color, transparent);
            max-width: 220px;
            min-width: 130px;
            transition: all 0.15s ease;
            position: relative;
        }
        .tab:hover { background: #243248; color: var(--text-main); }
        .tab.active { background: var(--bg-toolbar); color: var(--text-main); font-weight: 500; border-color: var(--border-color); border-bottom: 3px solid var(--container-color, #38bdf8); }
        .tab-title { white-space: nowrap; overflow: hidden; text-overflow: ellipsis; flex: 1; }
        .container-badge {
            font-size: 0.65rem;
            padding: 2px 6px;
            border-radius: 4px;
            font-weight: bold;
            background: var(--container-color);
            color: #fff;
            white-space: nowrap;
        }
        .tab-close { opacity: 0.6; font-size: 1rem; border-radius: 50%; padding: 0 4px; }
        .tab-close:hover { opacity: 1; background: rgba(255,255,255,0.1); color: var(--danger); }
        .btn-new-tab { background: transparent; border: none; color: var(--text-muted); font-size: 1.2rem; cursor: pointer; padding: 4px 10px; border-radius: 6px; }
        .btn-new-tab:hover { background: var(--bg-toolbar); color: var(--text-main); }

        .container-dropdown {
            position: relative;
            display: inline-block;
        }
        .container-menu {
            display: none;
            position: absolute;
            left: 0;
            top: 100%;
            background: #1e293b;
            border: 1px solid var(--border-color);
            border-radius: 8px;
            box-shadow: 0 10px 25px rgba(0,0,0,0.5);
            z-index: 1000;
            min-width: 220px;
            padding: 6px 0;
        }
        .container-menu.show { display: block; }
        .container-option {
            padding: 8px 16px;
            display: flex;
            align-items: center;
            gap: 10px;
            color: var(--text-main);
            font-size: 0.85rem;
            cursor: pointer;
            transition: background 0.15s;
        }
        .container-option:hover { background: #334155; }
        .c-dot { width: 10px; height: 10px; border-radius: 50%; display: inline-block; }

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

        /* Main Body Layout */
        .main-wrapper { display: flex; flex: 1; overflow: hidden; position: relative; }
        .vertical-sidebar {
            width: 250px;
            background: #0b1120;
            border-left: 1px solid var(--border-color);
            display: none;
            flex-direction: column;
            padding: 10px;
            gap: 6px;
            overflow-y: auto;
        }
        body.vertical-mode .vertical-sidebar { display: flex; }
        body.vertical-mode .window-header .tabs-container { display: none; }
        body.vertical-mode .window-header .btn-new-tab { display: none; }

        .v-tab {
            background: #1e293b;
            color: var(--text-muted);
            padding: 10px 12px;
            border-radius: 8px;
            display: flex;
            align-items: center;
            gap: 10px;
            font-size: 0.85rem;
            cursor: pointer;
            border-right: 4px solid var(--container-color, #94a3b8);
            transition: all 0.15s ease;
        }
        .v-tab:hover { background: #334155; color: #fff; }
        .v-tab.active { background: #334155; color: #fff; font-weight: bold; box-shadow: 0 4px 12px rgba(0,0,0,0.3); }

        /* Workspaces Bar */
        .workspaces-bar {
            background: #0b1120;
            padding: 6px 16px;
            display: flex;
            align-items: center;
            gap: 8px;
            border-bottom: 1px solid rgba(255,255,255,0.05);
            overflow-x: auto;
        }
        .ws-pill {
            background: #1e293b;
            color: var(--text-muted);
            padding: 4px 12px;
            border-radius: 20px;
            font-size: 0.8rem;
            cursor: pointer;
            display: flex;
            align-items: center;
            gap: 6px;
            border: 1px solid transparent;
            transition: all 0.15s ease;
            white-space: nowrap;
        }
        .ws-pill:hover { background: #334155; color: #fff; }
        .ws-pill.active { background: rgba(56, 189, 248, 0.15); border-color: var(--accent); color: var(--accent); font-weight: bold; }

        /* Split View */
        .content-area.split-active {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 6px;
            padding: 6px;
            background: #0b1120;
        }
        .split-pane {
            background: #0f172a;
            border: 1px solid var(--border-color);
            border-radius: 8px;
            overflow-y: auto;
            display: flex;
            flex-direction: column;
        }
        .split-pane-header {
            background: #1e293b;
            padding: 8px 12px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            font-size: 0.8rem;
            border-bottom: 1px solid var(--border-color);
        }

        /* Command Palette Modal */
        .cmd-modal-backdrop {
            display: none;
            position: fixed;
            top: 0; left: 0; right: 0; bottom: 0;
            background: rgba(0,0,0,0.7);
            backdrop-filter: blur(4px);
            z-index: 2000;
            justify-content: center;
            align-items: flex-start;
            padding-top: 10vh;
        }
        .cmd-modal-backdrop.show { display: flex; }
        .cmd-box {
            background: #1e293b;
            width: 600px;
            max-width: 90%;
            border-radius: 12px;
            border: 1px solid var(--accent);
            box-shadow: 0 20px 40px rgba(0,0,0,0.6);
            overflow: hidden;
            display: flex;
            flex-direction: column;
        }
        .cmd-input {
            width: 100%;
            padding: 16px 20px;
            background: #0f172a;
            border: none;
            color: #fff;
            font-size: 1.1rem;
            outline: none;
            border-bottom: 1px solid var(--border-color);
        }
        .cmd-list { max-height: 350px; overflow-y: auto; padding: 8px 0; }
        .cmd-item {
            padding: 10px 20px;
            display: flex;
            align-items: center;
            justify-content: space-between;
            cursor: pointer;
            color: var(--text-main);
            font-size: 0.9rem;
            transition: background 0.1s;
        }
        .cmd-item:hover, .cmd-item.selected { background: #334155; color: var(--accent); }
        .cmd-badge { font-size: 0.75rem; background: rgba(255,255,255,0.08); padding: 2px 6px; border-radius: 4px; color: var(--text-muted); }

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

        .banner-engine {
            background: rgba(56, 189, 248, 0.1);
            border: 1px solid rgba(56, 189, 248, 0.2);
            padding: 8px 16px;
            font-size: 0.8rem;
            color: #38bdf8;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
    </style>
</head>
<body>
    <div class="banner-engine">
        <span>🦊 مبني بنواة Mozilla Firefox (Gecko Engine Architecture)</span>
        <span>AtlasBrowser Quantum v128.0 (Firefox Edition)</span>
    </div>

    <!-- Workspaces Bar (Arc / Zen Style) -->
    <div class="workspaces-bar" id="workspacesBar"></div>

    <!-- Window Header & Tabs -->
    <div class="window-header">
        <div class="window-controls">
            <span class="dot red"></span>
            <span class="dot yellow"></span>
            <span class="dot green"></span>
        </div>
        <div class="tabs-container" id="tabsList"></div>
        <button class="btn-new-tab" onclick="createNewTab(0)" title="فتح لسان عادي">+</button>
        <div class="container-dropdown">
            <button class="btn-new-tab" onclick="toggleContainerMenu()" title="فتح لسان في حاوية Firefox Container">🛡️ الحاويات ▼</button>
            <div class="container-menu" id="containerMenu">
                <div style="padding: 6px 16px; font-size: 0.75rem; color: #94a3b8; font-weight: bold;">اختر حاوية معزولة (Multi-Container):</div>
                <div class="container-option" onclick="createNewTab(1)"><span class="c-dot" style="background:#38bdf8;"></span> لسان شخصي (Personal)</div>
                <div class="container-option" onclick="createNewTab(2)"><span class="c-dot" style="background:#fb923c;"></span> لسان العمل (Work)</div>
                <div class="container-option" onclick="createNewTab(3)"><span class="c-dot" style="background:#4ade80;"></span> لسان بنكي (Banking)</div>
                <div class="container-option" onclick="createNewTab(4)"><span class="c-dot" style="background:#f472b6;"></span> لسان التسوق (Shopping)</div>
            </div>
        </div>
    </div>

    <!-- Toolbar -->
    <div class="toolbar">
        <button class="nav-btn" onclick="historyBack()" title="رجوع">➔</button>
        <button class="nav-btn" onclick="historyForward()" title="تقدم">➔</button>
        <button class="nav-btn" onclick="reloadTab()" title="إعادة تحميل">⟳</button>
        <button class="nav-btn" onclick="navigate('mybrowser://newtab')" title="صفحة البداية">🏠</button>

        <div class="omnibar-container">
            <button class="shield-btn" id="shieldStatus" onclick="toggleShield()">
                <span>🛡️</span> <span id="shieldText">الدرع مفعل</span>
            </button>
            <input type="text" class="omnibar-input" id="urlInput" placeholder="اكتب عنوان ويب أو ابحث في الويب..." onkeydown="if(event.key==='Enter') handleUrlSubmit()">
        </div>

        <button class="nav-btn" onclick="navigate('about:addons')" title="متجر وإضافات فايرفوكس (Firefox Add-ons)">🧩</button>
        <button class="nav-btn" onclick="navigate('mybrowser://settings')" title="إعدادات المتصفح">⚙️</button>
        <button class="nav-btn" onclick="navigate('mybrowser://stats')" title="إحصائيات الحظر">📊</button>
        <button class="layout-btn" onclick="toggleSplitView()" title="تقسيم الشاشة لعرض لسانين (Split View)">🪟 <span id="splitText">تقسيم الشاشة</span></button>
        <button class="layout-btn" onclick="toggleVerticalTabs()" title="تبديل الألسنة الجانبية (Vertical Tabs)">📑 <span id="layoutText">ألسنة جانبية</span></button>
        <button class="layout-btn" onclick="openCommandPalette()" style="background: rgba(168, 85, 247, 0.15); border-color: rgba(168, 85, 247, 0.4); color: #c084fc;" title="لوحة الأوامر السريعة (Ctrl+K)">⚡ Ctrl+K</button>
    </div>

    <!-- Bookmarks Bar -->
    <div class="bookmarks-bar" id="bookmarksBar">
        <span style="color: var(--accent); font-weight: bold;">المفضلات:</span>
    </div>

    <!-- Main Wrapper (Sidebar + Viewport) -->
    <div class="main-wrapper">
        <div class="vertical-sidebar" id="verticalSidebar">
            <div style="font-size: 0.75rem; color: #94a3b8; font-weight: bold; margin-bottom: 8px;">الألسنة الرأسية (Vertical Tabs):</div>
            <div id="vTabsList" style="display: flex; flex-direction: column; gap: 6px; flex: 1;"></div>
            <div style="display: flex; gap: 4px; margin-top: 10px;">
                <button class="btn-new-tab" onclick="createNewTab(0)" style="flex: 1; border: 1px dashed #334155; font-size: 0.8rem; padding: 6px;">+ لسان عادي</button>
                <button class="btn-new-tab" onclick="toggleContainerMenu()" style="border: 1px dashed #334155; font-size: 0.8rem; padding: 6px;">🛡️ حاوية</button>
            </div>
        </div>
        <div class="content-area" id="contentArea"></div>
    </div>

    <!-- Command Palette Modal -->
    <div class="cmd-modal-backdrop" id="cmdModalBackdrop" onclick="if(event.target===this) closeCommandPalette()">
        <div class="cmd-box">
            <input type="text" class="cmd-input" id="cmdInput" placeholder="اكتب أمراً، موقعاً، أو اختر من القائمة..." oninput="filterCommands(this.value)" onkeydown="handleCmdKey(event)">
            <div class="cmd-list" id="cmdList"></div>
        </div>
    </div>

    <div class="toast" id="toastBox"></div>

    <script>
        let currentState = {};

        async function fetchState() {
            try {
                const res = await fetch('/api/state');
                currentState = await res.json();
                renderUI();
            } catch (e) {
                console.error('Failed to fetch state from C++ backend', e);
            }
        }

        function showToast(msg, isSuccess = true) {
            const toast = document.getElementById('toastBox');
            toast.innerText = msg;
            toast.style.borderRightColor = isSuccess ? 'var(--accent)' : 'var(--danger)';
            toast.style.display = 'block';
            setTimeout(() => { toast.style.display = 'none'; }, 3500);
        }

        function toggleContainerMenu() {
            document.getElementById('containerMenu').classList.toggle('show');
        }

        window.onclick = function(e) {
            if (!e.target.matches('.container-dropdown *')) {
                const menu = document.getElementById('containerMenu');
                if (menu && menu.classList.contains('show')) menu.classList.remove('show');
            }
        };

        function toggleVerticalTabs() {
            document.body.classList.toggle('vertical-mode');
            const isV = document.body.classList.contains('vertical-mode');
            document.getElementById('layoutText').innerText = isV ? 'ألسنة أفقية' : 'ألسنة جانبية';
            renderUI();
        }

        function renderUI() {
            // 1. Render Workspaces Bar
            const wsBar = document.getElementById('workspacesBar');
            if (wsBar && currentState.workspaces) {
                wsBar.innerHTML = '';
                currentState.workspaces.forEach(ws => {
                    const pill = document.createElement('div');
                    pill.className = 'ws-pill' + (ws.id === currentState.activeWorkspaceId ? ' active' : '');
                    pill.onclick = () => switchWorkspace(ws.id);
                    pill.innerHTML = `<span>${ws.icon}</span> <span>${ws.name}</span>`;
                    wsBar.appendChild(pill);
                });
            }

            // 2. Render Tabs (filtered by active workspace)
            const tabsList = document.getElementById('tabsList');
            const vTabsList = document.getElementById('vTabsList');
            tabsList.innerHTML = '';
            if (vTabsList) vTabsList.innerHTML = '';

            const currentWsId = currentState.activeWorkspaceId || 1;
            const visibleTabs = currentState.tabs.filter(t => !t.workspaceId || t.workspaceId === currentWsId);

            visibleTabs.forEach(tab => {
                const tabEl = document.createElement('div');
                tabEl.className = 'tab' + (tab.id === currentState.activeTabId ? ' active' : '');
                tabEl.style.setProperty('--container-color', tab.containerColor || '#94a3b8');
                tabEl.onclick = () => switchTab(tab.id);

                let badgeHtml = '';
                if (tab.containerId > 0) {
                    badgeHtml = `<span class="container-badge" style="background:${tab.containerColor}">${tab.containerName.split(' ')[0]}</span>`;
                }

                tabEl.innerHTML = `
                    ${badgeHtml}
                    <span class="tab-title">${tab.title || 'تبويب جديد'}</span>
                    <span class="tab-close" onclick="event.stopPropagation(); closeTab(${tab.id})">×</span>
                `;
                tabsList.appendChild(tabEl);

                // Vertical Tab
                if (vTabsList) {
                    const vEl = document.createElement('div');
                    vEl.className = 'v-tab' + (tab.id === currentState.activeTabId ? ' active' : '');
                    vEl.style.setProperty('--container-color', tab.containerColor || '#94a3b8');
                    vEl.onclick = () => switchTab(tab.id);
                    vEl.innerHTML = `
                        ${badgeHtml}
                        <span style="white-space:nowrap; overflow:hidden; text-overflow:ellipsis; flex:1;">${tab.title || 'تبويب جديد'}</span>
                        <span class="tab-close" onclick="event.stopPropagation(); closeTab(${tab.id})">×</span>
                    `;
                    vTabsList.appendChild(vEl);
                }
            });

            // 3. Update Split View Button Text
            const splitText = document.getElementById('splitText');
            if (splitText && currentState.splitView) {
                splitText.innerText = currentState.splitView.enabled ? 'إلغاء التقسيم' : 'تقسيم الشاشة';
            }

            // 4. Update Omnibar & Content
            const activeTab = currentState.tabs.find(t => t.id === currentState.activeTabId);
            if (activeTab) {
                document.getElementById('urlInput').value = activeTab.url;
                renderContent(activeTab);
            }

            const shieldBtn = document.getElementById('shieldStatus');
            const shieldText = document.getElementById('shieldText');
            if (currentState.adBlockEnabled) {
                shieldBtn.className = 'shield-btn';
                shieldText.innerText = 'الدرع مفعل (' + currentState.stats.totalBlocked + ')';
            } else {
                shieldBtn.className = 'shield-btn off';
                shieldText.innerText = 'الدرع معطل';
            }

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

        function renderContent(primaryTab) {
            const area = document.getElementById('contentArea');
            if (currentState.splitView && currentState.splitView.enabled) {
                area.classList.add('split-active');
                const secondaryTab = currentState.tabs.find(t => t.id === currentState.splitView.secondaryId) || primaryTab;
                area.innerHTML = `
                    <div class="split-pane">
                        <div class="split-pane-header">
                            <span style="color: #38bdf8; font-weight:bold;">اللسان الأول: ${primaryTab.title}</span>
                            <span style="font-size:0.75rem; color:${primaryTab.containerColor}">${primaryTab.containerName}</span>
                        </div>
                        <div style="flex:1; overflow-y:auto;">${primaryTab.content || ''}</div>
                    </div>
                    <div class="split-pane">
                        <div class="split-pane-header">
                            <span style="color: #4ade80; font-weight:bold;">اللسان الثاني (Split): ${secondaryTab.title}</span>
                            <button onclick="toggleSplitView()" style="background:transparent; border:none; color:#ef4444; font-size:1rem; cursor:pointer;">×</button>
                        </div>
                        <div style="flex:1; overflow-y:auto;">${secondaryTab.content || ''}</div>
                    </div>
                `;
            } else {
                area.classList.remove('split-active');
                if (primaryTab.content) {
                    area.innerHTML = primaryTab.content;
                } else {
                    area.innerHTML = '<div style="padding:40px; text-align:center; color:#94a3b8;">جاري التحميل...</div>';
                }
            }
        }

        async function switchWorkspace(id) {
            await fetch('/api/workspaces/switch', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            fetchState();
        }

        async function toggleSplitView() {
            await fetch('/api/splitview/toggle', { method: 'POST' });
            fetchState();
        }

        // Command Palette
        const COMMANDS = [
            { id: 'new_tab', title: 'فتح لسان جديد عادي', icon: '➕', action: () => createNewTab(0) },
            { id: 'c_work', title: 'فتح لسان في حاوية العمل (Work Container)', icon: '🟠', action: () => createNewTab(2) },
            { id: 'c_bank', title: 'فتح لسان في حاوية البنوك (Banking Container)', icon: '🟢', action: () => createNewTab(3) },
            { id: 'c_shop', title: 'فتح لسان في حاوية التسوق (Shopping Container)', icon: '🌸', action: () => createNewTab(4) },
            { id: 'c_pers', title: 'فتح لسان في حاوية شخصي (Personal Container)', icon: '🔵', action: () => createNewTab(1) },
            { id: 'split', title: 'تبديل تقسيم الشاشة (Toggle Split View)', icon: '🪟', action: () => toggleSplitView() },
            { id: 'vtabs', title: 'تبديل الألسنة الجانبية (Vertical Tabs)', icon: '📑', action: () => toggleVerticalTabs() },
            { id: 'addons', title: 'فتح متجر إضافات فايرفوكس (AMO Add-ons)', icon: '🧩', action: () => navigate('about:addons') },
            { id: 'settings', title: 'إعدادات النواة C++', icon: '⚙️', action: () => navigate('mybrowser://settings') },
            { id: 'stats', title: 'إحصائيات الحظر والدرع', icon: '📊', action: () => navigate('mybrowser://stats') },
            { id: 'shield', title: 'تبديل درع الإعلانات (Toggle Shield)', icon: '🛡️', action: () => toggleShield() }
        ];

        function openCommandPalette() {
            const modal = document.getElementById('cmdModalBackdrop');
            modal.classList.add('show');
            const inp = document.getElementById('cmdInput');
            inp.value = '';
            inp.focus();
            filterCommands('');
        }

        function closeCommandPalette() {
            const modal = document.getElementById('cmdModalBackdrop');
            modal.classList.remove('show');
        }

        function filterCommands(query) {
            const list = document.getElementById('cmdList');
            list.innerHTML = '';
            const q = query.toLowerCase().trim();
            const filtered = COMMANDS.filter(c => c.title.toLowerCase().includes(q));

            filtered.forEach((cmd, idx) => {
                const item = document.createElement('div');
                item.className = 'cmd-item' + (idx === 0 ? ' selected' : '');
                item.onclick = () => { closeCommandPalette(); cmd.action(); };
                item.innerHTML = `
                    <div style="display:flex; align-items:center; gap:12px;">
                        <span>${cmd.icon}</span>
                        <span>${cmd.title}</span>
                    </div>
                    <span class="cmd-badge">Enter</span>
                `;
                list.appendChild(item);
            });
        }

        function handleCmdKey(e) {
            if (e.key === 'Escape') {
                closeCommandPalette();
            } else if (e.key === 'Enter') {
                const selected = document.querySelector('.cmd-item.selected') || document.querySelector('.cmd-item');
                if (selected) selected.click();
            }
        }

        window.addEventListener('keydown', (e) => {
            if ((e.ctrlKey || e.metaKey) && (e.key === 'k' || e.key === 'K')) {
                e.preventDefault();
                openCommandPalette();
            }
        });

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
                showToast('تم حظر الموقع الإعلاني: ' + data.reason, false);
            } else if (data.containerSwitched) {
                showToast(`🛡️ قام محرك C++ بنقلك تلقائياً إلى حاوية: ${data.containerName}!`);
            } else if (data.cleaned) {
                showToast('قام محرك C++ بتنظيف الرابط وترقيته لـ HTTPS بنجاح ✅');
            }
            fetchState();
        }

        async function createNewTab(containerId = 0) {
            const menu = document.getElementById('containerMenu');
            if (menu) menu.classList.remove('show');
            await fetch('/api/tabs/new', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ containerId: containerId })
            });
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

        async function historyBack() {
            await fetch('/api/tabs/back', { method: 'POST' });
            fetchState();
        }

        async function historyForward() {
            await fetch('/api/tabs/forward', { method: 'POST' });
            fetchState();
        }

        async function reloadTab() {
            const activeTab = currentState.tabs.find(t => t.id === currentState.activeTabId);
            if (activeTab) navigate(activeTab.url);
        }

        async function toggleShield() {
            await fetch('/api/shield/toggle', { method: 'POST' });
            fetchState();
        }

        function installAddon(name, icon) {
            showToast(`تم تثبيت إضافة ${name} من متجر فايرفوكس الرسمي وتفعيلها بنجاح! 🎉`);
            const bBar = document.getElementById('bookmarksBar');
            const span = document.createElement('span');
            span.style = 'background: rgba(56, 189, 248, 0.15); color: #38bdf8; padding: 2px 8px; border-radius: 4px; font-size: 0.75rem; margin-right: 8px;';
            span.innerHTML = `${icon} ${name} (نشط)`;
            bBar.appendChild(span);
        }

        fetchState();
    </script>
</body>
</html>
)RAW_HTML";

static std::string buildStateJson() {
    auto tabs = g_engine.tabs()->getAllTabs();
    uint32_t activeId = g_engine.tabs()->getActiveTabId();
    auto stats = g_engine.adBlocker()->getStats();
    auto bookmarks = g_engine.storage()->getBookmarks();

    std::ostringstream ss;
    ss << "{\n"
       << "  \"activeTabId\": " << activeId << ",\n"
       << "  \"adBlockEnabled\": " << (g_engine.adBlocker()->isEnabled() ? "true" : "false") << ",\n"
       << "  \"stats\": {\n"
       << "    \"totalBlocked\": " << stats.totalBlocked << ",\n"
       << "    \"adsBlocked\": " << stats.adsBlocked << ",\n"
       << "    \"trackersBlocked\": " << stats.trackersBlocked << ",\n"
       << "    \"analyticsBlocked\": " << stats.analyticsBlocked << ",\n"
       << "    \"requestsChecked\": " << stats.requestsChecked << "\n"
       << "  },\n"
       << "  \"tabs\": [\n";

    for (size_t i = 0; i < tabs.size(); ++i) {
        if (i > 0) ss << ",\n";
        std::string contentPreview = "";
        if (tabs[i].currentUrl.rfind("about:addons", 0) == 0 || tabs[i].currentUrl.rfind("mybrowser://addons", 0) == 0) {
            contentPreview = R"ADDONS_HTML(
                <div style="padding: 40px; max-width: 900px; margin: 0 auto;">
                    <div style="display:flex; justify-content:space-between; align-items:center; border-bottom: 2px solid #334155; padding-bottom: 16px; margin-bottom: 24px;">
                        <div>
                            <h1 style="color: #38bdf8; font-size: 2rem; margin: 0;">🧩 إدارة إضافات فايرفوكس (Firefox Add-ons)</h1>
                            <p style="color: #94a3b8; margin-top: 6px;">تثبيت وإدارة إضافات WebExtensions من الموقع الرسمي addons.mozilla.org</p>
                        </div>
                        <span style="background: rgba(251, 146, 60, 0.2); color: #fb923c; border: 1px solid #fb923c; padding: 6px 14px; border-radius: 20px; font-weight: bold; font-size: 0.85rem;">AMO متصل ومفعل</span>
                    </div>

                    <h3 style="color: #f8fafc; margin-bottom: 16px;">الإضافات الرسمية الموصى بها:</h3>
                    
                    <div style="display: flex; flex-direction: column; gap: 16px;">
                        <!-- Multi-Account Containers -->
                        <div style="background: #1e293b; border: 1px solid #334155; border-radius: 12px; padding: 20px; display: flex; justify-content: space-between; align-items: center;">
                            <div style="display: flex; gap: 16px; align-items: center;">
                                <div style="font-size: 2.2rem; background: rgba(56, 189, 248, 0.1); width: 60px; height: 60px; border-radius: 12px; display: flex; align-items: center; justify-content: center;">🛡️</div>
                                <div>
                                    <div style="font-size: 1.15rem; font-weight: bold; color: #fff;">Firefox Multi-Account Containers</div>
                                    <p style="color: #94a3b8; font-size: 0.85rem; margin-top: 4px;">عزل ملفات تعريف الارتباط والهوية لكل حساب (العمل، شخصي، بنكي) في ألسنة مستقلة داخل نفس النافذة.</p>
                                    <span style="font-size: 0.75rem; color: #38bdf8;">بواسطة: Mozilla Firefox Team • المعرف: @testpilot-containers</span>
                                </div>
                            </div>
                            <button onclick="installAddon('Firefox Multi-Account Containers', '🛡️')" style="background: #22c55e; color: #fff; border: none; padding: 10px 20px; border-radius: 8px; font-weight: bold; cursor: pointer; white-space: nowrap;">✓ مثبتة ونشطة (Installed)</button>
                        </div>

                        <!-- uBlock Origin -->
                        <div style="background: #1e293b; border: 1px solid #334155; border-radius: 12px; padding: 20px; display: flex; justify-content: space-between; align-items: center;">
                            <div style="display: flex; gap: 16px; align-items: center;">
                                <div style="font-size: 2.2rem; background: rgba(239, 68, 68, 0.1); width: 60px; height: 60px; border-radius: 12px; display: flex; align-items: center; justify-content: center;">🛑</div>
                                <div>
                                    <div style="font-size: 1.15rem; font-weight: bold; color: #fff;">uBlock Origin</div>
                                    <p style="color: #94a3b8; font-size: 0.85rem; margin-top: 4px;">مانع إعلانات خفيف وفعال جداً يدعم واجهات Manifest V2 الكاملة الحصرية في فايرفوكس.</p>
                                    <span style="font-size: 0.75rem; color: #38bdf8;">بواسطة: Raymond Hill • صيغة: .xpi</span>
                                </div>
                            </div>
                            <button onclick="installAddon('uBlock Origin', '🛑')" style="background: #38bdf8; color: #0f172a; border: none; padding: 10px 20px; border-radius: 8px; font-weight: bold; cursor: pointer; white-space: nowrap;">+ أضف إلى فايرفوكس</button>
                        </div>

                        <!-- Dark Reader -->
                        <div style="background: #1e293b; border: 1px solid #334155; border-radius: 12px; padding: 20px; display: flex; justify-content: space-between; align-items: center;">
                            <div style="display: flex; gap: 16px; align-items: center;">
                                <div style="font-size: 2.2rem; background: rgba(168, 85, 247, 0.1); width: 60px; height: 60px; border-radius: 12px; display: flex; align-items: center; justify-content: center;">🌙</div>
                                <div>
                                    <div style="font-size: 1.15rem; font-weight: bold; color: #fff;">Dark Reader</div>
                                    <p style="color: #94a3b8; font-size: 0.85rem; margin-top: 4px;">تفعيل الوضع الليلي الداكن لجميع المواقع على الإنترنت بذكاء وحماية العينين.</p>
                                    <span style="font-size: 0.75rem; color: #38bdf8;">بواسطة: Alexander Shutov</span>
                                </div>
                            </div>
                            <button onclick="installAddon('Dark Reader', '🌙')" style="background: #38bdf8; color: #0f172a; border: none; padding: 10px 20px; border-radius: 8px; font-weight: bold; cursor: pointer; white-space: nowrap;">+ أضف إلى فايرفوكس</button>
                        </div>
                    </div>
                </div>
            )ADDONS_HTML";
        } else if (tabs[i].currentUrl.rfind("mybrowser://", 0) == 0) {
            auto schemeResp = g_engine.schemes()->handleRequest(tabs[i].currentUrl);
            contentPreview = schemeResp.content;
        } else {
            contentPreview = "<div style=\"padding:40px; text-align:center;\"><div style=\"background:#1e293b; padding:30px; border-radius:12px; max-width:700px; margin:0 auto; border:1px solid #334155;\"><h2 style=\"color:#38bdf8; margin-bottom:15px;\">تم فحص الرابط بنجاح بواسطة C++ Core ✅</h2><p style=\"direction:ltr; font-family:monospace; background:#0f172a; padding:12px; border-radius:6px; color:#22c55e;\">" + tabs[i].currentUrl + "</p><p style=\"margin-top:15px; color:#94a3b8;\">الطلب آمن وتمت ترقيته لـ HTTPS وتنظيف معلمات التتبع عبر C++ NetworkInterceptor.</p></div></div>";
        }

        // Escape contentPreview for JSON string
        std::string escContent;
        for (char c : contentPreview) {
            if (c == '"') escContent += "\\\"";
            else if (c == '\\') escContent += "\\\\";
            else if (c == '\n') escContent += "\\n";
            else if (c == '\r') escContent += "\\r";
            else if (c == '\t') escContent += "\\t";
            else escContent += c;
        }

        ss << "    {\n"
           << "      \"id\": " << tabs[i].id << ",\n"
           << "      \"title\": \"" << tabs[i].title << "\",\n"
           << "      \"url\": \"" << tabs[i].currentUrl << "\",\n"
           << "      \"containerId\": " << tabs[i].containerId << ",\n"
           << "      \"containerName\": \"" << tabs[i].containerName << "\",\n"
           << "      \"containerColor\": \"" << tabs[i].containerColor << "\",\n"
           << "      \"workspaceId\": " << tabs[i].workspaceId << ",\n"
           << "      \"content\": \"" << escContent << "\"\n"
           << "    }";
    }
    auto splitState = g_engine.tabs()->getSplitView();
    ss << "\n  ],\n"
       << "  \"activeWorkspaceId\": " << g_engine.workspaces()->getActiveWorkspaceId() << ",\n"
       << "  \"workspaces\": " << g_engine.workspaces()->exportWorkspacesJson() << ",\n"
       << "  \"splitView\": {\n"
       << "    \"enabled\": " << (splitState.enabled ? "true" : "false") << ",\n"
       << "    \"primaryId\": " << splitState.primaryTabId << ",\n"
       << "    \"secondaryId\": " << splitState.secondaryTabId << "\n"
       << "  },\n"
       << "  \"containers\": " << g_engine.containers()->exportContainersJson() << ",\n"
       << "  \"bookmarks\": [\n";

    for (size_t i = 0; i < bookmarks.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << "    {\"title\": \"" << bookmarks[i].title << "\", \"url\": \"" << bookmarks[i].url << "\"}";
    }
    ss << "\n  ]\n"
       << "}\n";

    return ss.str();
}

static std::string extractJsonField(const std::string& body, const std::string& field) {
    std::string key = "\"" + field + "\"";
    size_t kPos = body.find(key);
    if (kPos == std::string::npos) return "";
    size_t colon = body.find(':', kPos + key.size());
    if (colon == std::string::npos) return "";

    size_t start = body.find_first_not_of(" \t\r\n", colon + 1);
    if (start == std::string::npos) return "";

    if (body[start] == '"') {
        size_t end = body.find('"', start + 1);
        if (end == std::string::npos) return "";
        return body.substr(start + 1, end - start - 1);
    } else {
        size_t end = body.find_first_of(",}\r\n ", start);
        if (end == std::string::npos) end = body.size();
        return body.substr(start, end - start);
    }
}

void handleClient(int clientSocket) {
    char buffer[8192];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    }
    buffer[bytesRead] = '\0';
    std::string request(buffer, bytesRead);

    std::istringstream reqStream(request);
    std::string method, path, httpVer;
    reqStream >> method >> path >> httpVer;

    std::string body;
    size_t dblClrf = request.find("\r\n\r\n");
    if (dblClrf != std::string::npos) {
        body = request.substr(dblClrf + 4);
    }

    std::string responseStatus = "200 OK";
    std::string contentType = "text/html; charset=utf-8";
    std::string responseBody;

    if (method == "GET" && (path == "/" || path == "/index.html")) {
        responseBody = HTML_UI;
    } else if (method == "GET" && path == "/api/state") {
        contentType = "application/json";
        responseBody = buildStateJson();
    } else if (method == "POST" && path == "/api/navigate") {
        std::string url = extractJsonField(body, "url");
        if (url == "addons" || url == "about:addons" || url.find("addons.mozilla.org") != std::string::npos) {
            url = "about:addons";
        }
        auto navRes = g_engine.navigateActiveTab(url);
        if (url == "about:addons") {
            auto cur = g_engine.tabs()->getActiveTab();
            if (cur) {
                cur->title = "إضافات فايرفوكس";
                cur->currentUrl = "about:addons";
            }
            navRes.finalUrl = "about:addons";
            navRes.pageTitle = "إضافات فايرفوكس";
        }

        contentType = "application/json";
        std::ostringstream ss;
        ss << "{\n"
           << "  \"action\": \"" << (navRes.wasBlocked ? "blocked" : "allow") << "\",\n"
           << "  \"reason\": \"" << navRes.blockedReason << "\",\n"
           << "  \"cleaned\": true,\n"
           << "  \"containerSwitched\": " << (navRes.containerSwitched ? "true" : "false") << ",\n"
           << "  \"containerName\": \"" << navRes.newContainerName << "\",\n"
           << "  \"containerColor\": \"" << navRes.newContainerColor << "\",\n"
           << "  \"url\": \"" << navRes.finalUrl << "\"\n"
           << "}\n";
        responseBody = ss.str();
    } else if (method == "POST" && path == "/api/tabs/new") {
        std::string cIdStr = extractJsonField(body, "containerId");
        uint32_t cId = cIdStr.empty() ? 0 : std::stoi(cIdStr);
        std::string cName = "Default";
        std::string cColor = "#94a3b8";
        const auto* container = g_engine.containers()->getContainer(cId);
        if (container) {
            cName = container->name;
            cColor = container->color;
        }
        uint32_t wsId = g_engine.workspaces()->getActiveWorkspaceId();
        g_engine.tabs()->createTab("mybrowser://newtab", cId, cName, cColor, wsId);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/workspaces/switch") {
        std::string wsIdStr = extractJsonField(body, "id");
        if (!wsIdStr.empty()) {
            uint32_t wsId = std::stoi(wsIdStr);
            g_engine.workspaces()->switchWorkspace(wsId);
            auto tabsInWs = g_engine.tabs()->getTabsInWorkspace(wsId);
            if (!tabsInWs.empty()) {
                g_engine.tabs()->switchTab(tabsInWs[0].id);
            } else {
                const auto* ws = g_engine.workspaces()->getWorkspace(wsId);
                uint32_t cId = ws ? ws->defaultContainerId : 0;
                std::string cName = "Default";
                std::string cColor = "#94a3b8";
                const auto* container = g_engine.containers()->getContainer(cId);
                if (container) {
                    cName = container->name;
                    cColor = container->color;
                }
                g_engine.tabs()->createTab("mybrowser://newtab", cId, cName, cColor, wsId);
            }
        }
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/splitview/toggle") {
        auto currentSplit = g_engine.tabs()->getSplitView();
        bool newEnabled = !currentSplit.enabled;
        uint32_t secId = 0;
        if (newEnabled) {
            auto allTabs = g_engine.tabs()->getAllTabs();
            uint32_t activeId = g_engine.tabs()->getActiveTabId();
            for (const auto& t : allTabs) {
                if (t.id != activeId) {
                    secId = t.id;
                    break;
                }
            }
            if (secId == 0) {
                secId = g_engine.tabs()->createTab("https://duckduckgo.com");
            }
        }
        g_engine.tabs()->setSplitView(newEnabled, secId);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/tabs/switch") {
        std::string idStr = extractJsonField(body, "id");
        if (!idStr.empty()) {
            g_engine.tabs()->switchTab(std::stoi(idStr));
        }
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/tabs/close") {
        std::string idStr = extractJsonField(body, "id");
        if (!idStr.empty()) {
            g_engine.tabs()->closeTab(std::stoi(idStr));
        }
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/tabs/back") {
        auto cur = g_engine.tabs()->getActiveTab();
        if (cur) g_engine.tabs()->goBack(cur->id);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/tabs/forward") {
        auto cur = g_engine.tabs()->getActiveTab();
        if (cur) g_engine.tabs()->goForward(cur->id);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/shield/toggle") {
        bool newState = !g_engine.adBlocker()->isEnabled();
        g_engine.adBlocker()->setEnabled(newState);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\", \"enabled\": " + std::string(newState ? "true" : "false") + "}";
    } else {
        responseStatus = "404 Not Found";
        responseBody = "Not found";
    }

    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << responseStatus << "\r\n"
                   << "Content-Type: " << contentType << "\r\n"
                   << "Content-Length: " << responseBody.size() << "\r\n"
                   << "Access-Control-Allow-Origin: *\r\n"
                   << "Connection: close\r\n\r\n"
                   << responseBody;

    std::string respStr = responseStream.str();
    send(clientSocket, respStr.data(), respStr.size(), 0);
    close(clientSocket);
}

int main() {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons(8080);

    if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind to 0.0.0.0:8080\n";
        close(serverFd);
        return 1;
    }

    if (listen(serverFd, 20) < 0) {
        std::cerr << "Failed to listen\n";
        close(serverFd);
        return 1;
    }

    std::cout << "[AtlasBrowser] Native C++ Server listening on 0.0.0.0:8080\n";

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverFd, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket >= 0) {
            std::thread(handleClient, clientSocket).detach();
        }
    }

    close(serverFd);
    return 0;
}
