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
    <title>AtlasBrowser Quantum v128.0 (Firefox Edition)</title>
    <style>
        :root {
            --bg-dark: #0f172a;
            --bg-panel: #1e293b;
            --bg-tab-active: #334155;
            --bg-tab-inactive: #1e293b;
            --border-color: #334155;
            --accent: #38bdf8;
            --text-main: #f8fafc;
            --text-muted: #94a3b8;
            --danger: #ef4444;
            --success: #22c55e;
            --warning: #fb923c;
        }

        body.theme-oled {
            --bg-dark: #000000;
            --bg-panel: #0a0a0a;
            --bg-tab-active: #171717;
            --bg-tab-inactive: #0a0a0a;
            --border-color: #262626;
            --accent: #38bdf8;
        }

        body.theme-cyberpunk {
            --bg-dark: #0d0221;
            --bg-panel: #19053b;
            --bg-tab-active: #260959;
            --bg-tab-inactive: #12032e;
            --border-color: #ff007f;
            --accent: #00f0ff;
        }

        body.theme-nord {
            --bg-dark: #2e3440;
            --bg-panel: #3b4252;
            --bg-tab-active: #434c5e;
            --bg-tab-inactive: #2e3440;
            --border-color: #4c566a;
            --accent: #88c0d0;
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

        .tabs-container { display: flex; gap: 4px; flex: 1; overflow-x: auto; scrollbar-width: none; align-items: center; }
        .tab-group-pill {
            padding: 3px 10px;
            border-radius: 6px;
            font-size: 0.75rem;
            font-weight: bold;
            cursor: pointer;
            display: flex;
            align-items: center;
            gap: 6px;
            margin-left: 6px;
            border: 1px solid transparent;
            white-space: nowrap;
        }
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
        .tab:hover { background: #283548; color: var(--text-main); }
        .tab.active {
            background: var(--bg-tab-active);
            color: #fff;
            border-color: var(--border-color);
            border-bottom-color: var(--container-color, var(--accent));
            font-weight: 500;
        }
        .tab-title { white-space: nowrap; overflow: hidden; text-overflow: ellipsis; flex: 1; }
        .tab-close {
            opacity: 0.6;
            padding: 2px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            width: 18px;
            height: 18px;
            font-size: 0.8rem;
        }
        .tab-close:hover { background: rgba(255,255,255,0.2); opacity: 1; }
        .container-badge {
            font-size: 0.65rem;
            padding: 1px 6px;
            border-radius: 4px;
            color: #fff;
            background: var(--container-color, #94a3b8);
            font-weight: bold;
        }

        .btn-new-tab {
            background: transparent;
            border: none;
            color: var(--text-muted);
            font-size: 1.1rem;
            padding: 6px 10px;
            cursor: pointer;
            border-radius: 6px;
            margin-right: 4px;
        }
        .btn-new-tab:hover { background: var(--border-color); color: var(--text-main); }

        .container-dropdown { position: relative; display: inline-block; }
        .container-menu {
            display: none;
            position: absolute;
            top: 100%;
            left: 0;
            background: var(--bg-panel);
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
            cursor: pointer;
            font-size: 0.85rem;
            color: var(--text-main);
        }
        .container-option:hover { background: #334155; }
        .c-dot { width: 10px; height: 10px; border-radius: 50%; display: inline-block; }

        /* Toolbar */
        .toolbar {
            background: var(--bg-panel);
            padding: 8px 16px;
            display: flex;
            align-items: center;
            gap: 8px;
            border-bottom: 1px solid var(--border-color);
            position: relative;
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
            white-space: nowrap;
        }
        .shield-btn.off { background: rgba(239, 68, 68, 0.15); border-color: rgba(239, 68, 68, 0.4); color: var(--danger); }

        .layout-btn {
            background: #0f172a;
            border: 1px solid var(--border-color);
            color: var(--text-muted);
            padding: 4px 10px;
            border-radius: 6px;
            font-size: 0.8rem;
            cursor: pointer;
            display: flex;
            align-items: center;
            gap: 6px;
            white-space: nowrap;
        }
        .layout-btn:hover { border-color: var(--accent); color: #fff; }

        /* Extensions Toolbar */
        .ext-toolbar-group {
            display: flex;
            align-items: center;
            gap: 6px;
            border-right: 1px solid #334155;
            padding-right: 8px;
            margin-right: 4px;
        }
        .ext-btn {
            background: rgba(255,255,255,0.05);
            border: 1px solid #334155;
            color: #fff;
            width: 32px;
            height: 32px;
            border-radius: 6px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 1.1rem;
            cursor: pointer;
            position: relative;
            transition: all 0.2s;
        }
        .ext-btn:hover { background: rgba(56, 189, 248, 0.2); border-color: var(--accent); }
        .ext-btn.active-glow { box-shadow: 0 0 8px rgba(56, 189, 248, 0.6); }

        /* Extension Popup Modal */
        .ext-popup-dropdown {
            display: none;
            position: absolute;
            top: 48px;
            left: 120px;
            background: #1e293b;
            border: 1px solid #38bdf8;
            border-radius: 10px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.6);
            z-index: 2000;
            color: #fff;
        }
        .ext-popup-dropdown.show { display: block; }

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
            background: var(--bg-panel);
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
            background: var(--bg-panel);
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

        .content-area { flex: 1; background: var(--bg-dark); overflow-y: auto; position: relative; }

        /* DevTools Dock */
        .devtools-dock {
            height: 280px;
            background: #0a0e17;
            border-top: 2px solid var(--accent);
            display: none;
            flex-direction: column;
            z-index: 100;
        }
        .devtools-dock.show { display: flex; }
        .dt-header {
            background: #0f172a;
            border-bottom: 1px solid #334155;
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 4px 12px;
        }
        .dt-tabs { display: flex; gap: 4px; }
        .dt-tab {
            background: transparent;
            border: none;
            color: #94a3b8;
            padding: 6px 14px;
            font-size: 0.8rem;
            cursor: pointer;
            border-bottom: 2px solid transparent;
        }
        .dt-tab.active { color: #38bdf8; border-bottom-color: #38bdf8; font-weight: bold; }
        .dt-body { flex: 1; overflow-y: auto; padding: 10px 14px; font-family: monospace; font-size: 0.85rem; }

        /* Scratchpad Drawer */
        .scratchpad-drawer {
            width: 320px;
            background: #0f172a;
            border-right: 1px solid var(--border-color);
            display: none;
            flex-direction: column;
            z-index: 50;
        }
        .scratchpad-drawer.show { display: flex; }

        /* Floating PiP Window */
        .pip-window {
            position: fixed;
            bottom: 24px;
            left: 24px;
            width: 280px;
            background: #000;
            border: 2px solid var(--accent);
            border-radius: 12px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.8);
            z-index: 9000;
            display: none;
            flex-direction: column;
            overflow: hidden;
        }
        .pip-window.show { display: flex; }
        .pip-header {
            background: #0f172a;
            padding: 6px 10px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid #334155;
        }
        .pip-video-mock {
            height: 140px;
            background: linear-gradient(135deg, #1e1b4b 0%, #0f172a 100%);
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        .pip-controls {
            background: #0f172a;
            padding: 6px 12px;
            display: flex;
            justify-content: space-around;
            border-top: 1px solid #334155;
        }
        .pip-controls button {
            background: transparent;
            border: none;
            color: #fff;
            cursor: pointer;
            font-size: 0.9rem;
        }

        /* Hardware Modal */
        .hardware-modal-backdrop {
            display: none;
            position: fixed;
            top: 0; left: 0; right: 0; bottom: 0;
            background: rgba(0,0,0,0.7);
            backdrop-filter: blur(4px);
            z-index: 2000;
            justify-content: center;
            align-items: center;
        }
        .hardware-modal-backdrop.show { display: flex; }
        .hw-box {
            background: #1e293b;
            width: 480px;
            max-width: 90%;
            border-radius: 12px;
            border: 1px solid var(--accent);
            padding: 24px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.6);
        }

        /* AI Copilot Side Drawer */
        .ai-copilot-drawer {
            width: 340px;
            background: #080d1a;
            border-right: 1px solid var(--border-color);
            display: none;
            flex-direction: column;
            z-index: 50;
            box-shadow: -5px 0 25px rgba(0,0,0,0.5);
        }
        .ai-copilot-drawer.show { display: flex; }
        .ai-header {
            padding: 12px 16px;
            background: #0f172a;
            border-bottom: 1px solid var(--border-color);
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .ai-quick-actions {
            padding: 8px 12px;
            display: flex;
            gap: 6px;
            overflow-x: auto;
            border-bottom: 1px solid rgba(255,255,255,0.05);
        }
        .ai-pill {
            background: #1e293b;
            color: #38bdf8;
            border: 1px solid #334155;
            padding: 4px 10px;
            border-radius: 14px;
            font-size: 0.75rem;
            cursor: pointer;
            white-space: nowrap;
        }
        .ai-pill:hover { background: #38bdf8; color: #000; }
        .ai-chat-body {
            flex: 1;
            padding: 14px;
            overflow-y: auto;
            display: flex;
            flex-direction: column;
            gap: 12px;
        }
        .ai-msg {
            padding: 10px 14px;
            border-radius: 10px;
            font-size: 0.85rem;
            line-height: 1.5;
            max-width: 88%;
            word-break: break-word;
        }
        .ai-msg.assistant {
            background: #1e293b;
            border: 1px solid #334155;
            color: #f8fafc;
            align-self: flex-start;
            border-bottom-left-radius: 2px;
        }
        .ai-msg.user {
            background: #0284c7;
            color: #fff;
            align-self: flex-end;
            border-bottom-right-radius: 2px;
        }
        .ai-input-area {
            padding: 10px 12px;
            background: #0f172a;
            border-top: 1px solid var(--border-color);
            display: flex;
            gap: 8px;
        }
        .ai-input-area input {
            flex: 1;
            background: #1e293b;
            border: 1px solid #334155;
            color: #fff;
            padding: 8px 12px;
            border-radius: 6px;
            outline: none;
            font-size: 0.85rem;
        }
        .ai-input-area button {
            background: var(--accent);
            color: #0b1120;
            border: none;
            border-radius: 6px;
            padding: 0 14px;
            font-weight: bold;
            cursor: pointer;
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
            padding: 14px 22px;
            border-radius: 8px;
            border-right: 4px solid var(--accent);
            box-shadow: 0 10px 25px rgba(0,0,0,0.4);
            display: none;
            animation: slideIn 0.3s ease;
            z-index: 9999;
            font-size: 0.9rem;
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
        <span>🦊 AtlasBrowser Quantum • نظام الأجهزة الافتراضية (200 Virtual Devices Anti-Detect Pool)</span>
        <span id="activeDeviceBanner">الجهاز النشط: #1 (Windows 11 • NVIDIA RTX)</span>
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
            <input type="text" class="omnibar-input" id="urlInput" placeholder="اكتب موقعاً (مثال: about:devices أو addons.mozilla.org)..." onkeydown="if(event.key==='Enter') handleUrlSubmit()">
        </div>

        <!-- Active WebExtension Icons Toolbar -->
        <div class="ext-toolbar-group" id="extToolbarGroup"></div>

        <button class="layout-btn" onclick="navigate('about:devices')" style="background: rgba(34, 197, 94, 0.15); border-color: rgba(34, 197, 94, 0.4); color: #4ade80;" title="إدارة 200 متصفح وجهاز افتراضي معزول">💻 200 جهاز افتراضي</button>
        <button class="layout-btn" onclick="navigate('about:storage')" style="background: rgba(16, 185, 129, 0.15); border-color: rgba(16, 185, 129, 0.4); color: #34d399;" title="مركز إدارة التخزين فائق الخفة والعزل لـ 200 متصفح (Ultra-VFS Storage)">💾 التخزين الخارق</button>
        <button class="layout-btn" onclick="toggleMultiDeviceMatrix()" style="background: rgba(56, 189, 248, 0.15); border-color: rgba(56, 189, 248, 0.4); color: #38bdf8;" title="عرض مصفوفة المتصفحات المتزامنة (3 أجهزة مختلفة جنباً إلى جنب)">🖥️ شاشة متعددة</button>
        <button class="layout-btn" onclick="toggleDevTools()" style="background: rgba(168, 85, 247, 0.15); border-color: rgba(168, 85, 247, 0.4); color: #c084fc;" title="أدوات المطورين وفاحص الشبكة (F12 DevTools)">🛠️ DevTools</button>
        <button class="nav-btn" onclick="navigate('about:passwords')" title="الخزنة المشفرة لكلمات المرور">🔐</button>
        <button class="layout-btn" onclick="toggleScratchpad()" title="لوحة الملاحظات السريعة (Scratchpad)">📝 ملاحظات</button>
        <button class="layout-btn" onclick="toggleHardwareModal()" title="لوحة التحكم بالعتاد والميديا (Opera GX Style)">🎛️ العتاد</button>
        <button class="layout-btn" onclick="toggleAiDrawer()" style="background: rgba(56, 189, 248, 0.15); border-color: rgba(56, 189, 248, 0.4); color: #38bdf8;" title="المساعد الذكي (Atlas Copilot AI)">🤖 Atlas AI</button>
        <button class="nav-btn" onclick="navigate('https://addons.mozilla.org/firefox/')" title="متجر إضافات فايرفوكس (AMO)">🧩</button>
        <button class="nav-btn" onclick="navigate('about:downloads')" title="التنزيلات">📥</button>
        <button class="nav-btn" onclick="navigate('about:performance')" title="مراقبة الأداء">⚡</button>
        <button class="nav-btn" onclick="toggleReaderMode()" title="وضع القراءة">📖</button>
        <button class="layout-btn" onclick="cycleTheme()" title="المظهر">🎨 <span id="themeName">الداكن</span></button>
        <button class="layout-btn" onclick="toggleSplitView()" title="تقسيم الشاشة">🪟 <span id="splitText">تقسيم</span></button>
        <button class="layout-btn" onclick="toggleVerticalTabs()" title="ألسنة جانبية">📑 <span id="layoutText">جانبية</span></button>
        <button class="layout-btn" onclick="openCommandPalette()" style="background: rgba(168, 85, 247, 0.15); border-color: rgba(168, 85, 247, 0.4); color: #c084fc;" title="لوحة الأوامر السريعة (Ctrl+K)">⚡ Ctrl+K</button>

        <div class="ext-popup-dropdown" id="extPopupDropdown"></div>
    </div>

    <!-- Bookmarks Bar -->
    <div class="bookmarks-bar" id="bookmarksBar">
        <span style="color: var(--accent); font-weight: bold;">المفضلات:</span>
    </div>

    <!-- Main Wrapper (Sidebar + Viewport + AI Copilot Drawer + Scratchpad) -->
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

        <!-- Scratchpad Drawer -->
        <div class="scratchpad-drawer" id="scratchpadDrawer">
            <div style="padding:12px 16px; background:#0f172a; border-bottom:1px solid #334155; display:flex; justify-content:space-between; align-items:center;">
                <b style="color:#38bdf8;">📝 مفكرة الملاحظات (Scratchpad)</b>
                <div style="display:flex; gap:6px;">
                    <button onclick="clipActivePage()" class="ai-pill" style="font-size:0.75rem;">✂️ قص الصفحة</button>
                    <button onclick="toggleScratchpad()" style="background:transparent; border:none; color:#94a3b8; cursor:pointer;">✕</button>
                </div>
            </div>
            <div id="spNotesList" style="flex:1; overflow-y:auto; padding:12px; display:flex; flex-direction:column; gap:8px;"></div>
            <div style="padding:12px; background:#0f172a; border-top:1px solid #334155; display:flex; flex-direction:column; gap:6px;">
                <input type="text" id="spNewTitle" placeholder="عنوان الملاحظة..." style="background:#1e293b; border:1px solid #334155; color:#fff; padding:6px 10px; border-radius:6px; font-size:0.8rem; outline:none;">
                <textarea id="spNewContent" rows="3" placeholder="محتوى الملاحظة..." style="background:#1e293b; border:1px solid #334155; color:#fff; padding:6px 10px; border-radius:6px; font-size:0.8rem; outline:none; resize:none;"></textarea>
                <button onclick="saveNewNote()" style="background:var(--accent); color:#000; border:none; padding:6px; border-radius:6px; font-weight:bold; cursor:pointer; font-size:0.8rem;">حفظ الملاحظة</button>
            </div>
        </div>

        <!-- AI Copilot Drawer -->
        <div class="ai-copilot-drawer" id="aiDrawer">
            <div class="ai-header">
                <div style="display: flex; align-items: center; gap: 8px;">
                    <span style="font-size: 1.2rem;">🤖</span>
                    <div>
                        <div style="font-weight: bold; color: #fff; font-size: 0.9rem;">Atlas Copilot AI</div>
                        <div style="font-size: 0.7rem; color: #22c55e;">● C++ AI Engine متصل</div>
                    </div>
                </div>
                <div style="display: flex; gap: 6px;">
                    <button onclick="clearAiChat()" title="مسح المحادثة" style="background:transparent; border:none; color:#94a3b8; cursor:pointer;">🗑️</button>
                    <button onclick="toggleAiDrawer()" style="background:transparent; border:none; color:#94a3b8; font-size:1.1rem; cursor:pointer;">✕</button>
                </div>
            </div>
            <div class="ai-quick-actions">
                <button class="ai-pill" onclick="sendAiPrompt('summarize')">📌 تلخيص الصفحة</button>
                <button class="ai-pill" onclick="sendAiPrompt('explain')">🔍 شرح الكود</button>
                <button class="ai-pill" onclick="sendAiPrompt('privacy')">🛡️ فحص الخصوصية</button>
            </div>
            <div class="ai-chat-body" id="aiChatBody"></div>
            <div class="ai-input-area">
                <input type="text" id="aiInput" placeholder="اسأل الذكاء الاصطناعي عن الصفحة..." onkeydown="if(event.key==='Enter') sendAiMessage()">
                <button onclick="sendAiMessage()">إرسال</button>
            </div>
        </div>
    </div>

    <!-- DevTools Dock -->
    <div class="devtools-dock" id="devToolsDock">
        <div class="dt-header">
            <div class="dt-tabs">
                <button class="dt-tab active" id="tabBtnNetwork" onclick="switchDevTab('network')">🌐 الشبكة (Network Waterfall)</button>
                <button class="dt-tab" id="tabBtnConsole" onclick="switchDevTab('console')">💻 الكونسول (JS Console)</button>
                <button class="dt-tab" id="tabBtnDom" onclick="switchDevTab('dom')">🌳 عناصر DOM</button>
            </div>
            <button onclick="toggleDevTools()" style="background:transparent; border:none; color:#94a3b8; font-size:1.1rem; cursor:pointer;">✕</button>
        </div>
        <div class="dt-body" id="dtBody"></div>
    </div>

    <!-- Floating PiP Window -->
    <div class="pip-window" id="pipWindow">
        <div class="pip-header">
            <span style="font-size:0.75rem; color:#fff;" id="pipTitle">🎥 مشغل الفيديو العائم (PiP)</span>
            <button onclick="togglePip(false)" style="background:transparent; border:none; color:#94a3b8; cursor:pointer;">✕</button>
        </div>
        <div class="pip-video-mock">
            <div style="color:#38bdf8; font-size:2rem;">▶️</div>
            <div style="font-size:0.8rem; color:#cbd5e1; margin-top:4px;" id="pipMediaName">Firefox Quantum Media Stream</div>
        </div>
        <div class="pip-controls">
            <button onclick="pipTogglePlay()" id="pipPlayBtn">⏸️</button>
            <button onclick="pipCycleSpeed()" id="pipSpeedBtn">1.0x</button>
            <button onclick="pipToggleMute()" id="pipMuteBtn">🔊</button>
        </div>
    </div>

    <!-- Hardware Limiter Modal -->
    <div class="hardware-modal-backdrop" id="hwModalBackdrop" onclick="if(event.target===this) toggleHardwareModal()">
        <div class="hw-box">
            <div style="display:flex; justify-content:space-between; align-items:center; border-bottom:1px solid #334155; padding-bottom:12px; margin-bottom:16px;">
                <h3 style="color:#38bdf8; margin:0;">🎛️ لوحة التحكم في عتاد الجهاز (Opera GX Style)</h3>
                <button onclick="toggleHardwareModal()" style="background:transparent; border:none; color:#94a3b8; font-size:1.1rem; cursor:pointer;">✕</button>
            </div>
            <div style="display:flex; flex-direction:column; gap:16px;">
                <div>
                    <label style="display:flex; justify-content:space-between; font-size:0.85rem; margin-bottom:6px;">
                        <span>محدد استهلاك المعالج (CPU Limiter):</span>
                        <b id="cpuLimitText" style="color:#38bdf8;">100%</b>
                    </label>
                    <input type="range" min="10" max="100" value="100" style="width:100%;" oninput="document.getElementById('cpuLimitText').innerText=this.value+'%'; applyHardwareLimit();">
                </div>
                <div>
                    <label style="display:flex; justify-content:space-between; font-size:0.85rem; margin-bottom:6px;">
                        <span>محدد استهلاك الذاكرة (RAM Limiter):</span>
                        <b id="ramLimitText" style="color:#22c55e;">4096 MB</b>
                    </label>
                    <input type="range" min="512" max="8192" step="256" value="4096" style="width:100%;" oninput="document.getElementById('ramLimitText').innerText=this.value+' MB'; applyHardwareLimit();">
                </div>
                <div style="background:#0f172a; padding:12px; border-radius:8px; border:1px solid #334155; display:flex; justify-content:space-between; align-items:center;">
                    <span style="font-size:0.85rem; color:#fff;">مشغل الفيديو العائم (Picture-in-Picture)</span>
                    <button onclick="togglePip(true)" style="background:#0284c7; color:#fff; border:none; padding:6px 14px; border-radius:6px; font-weight:bold; cursor:pointer;">تشغيل النافذة العائمة</button>
                </div>
            </div>
        </div>
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
        let activeThemeIdx = 0;
        let amoCategoryFilter = 'all';
        let amoSearchQuery = '';
        let currentDevTab = 'network';
        let isMultiDeviceMatrix = false;
        let deviceSearchQuery = '';

        const THEMES = [
            { name: 'الداكن', cls: '' },
            { name: 'OLED نقي', cls: 'theme-oled' },
            { name: 'سايبربانك', cls: 'theme-cyberpunk' },
            { name: 'نورد', cls: 'theme-nord' }
        ];

        function cycleTheme() {
            activeThemeIdx = (activeThemeIdx + 1) % THEMES.length;
            document.body.className = THEMES[activeThemeIdx].cls;
            if (document.body.classList.contains('vertical-mode')) {
                document.body.classList.add('vertical-mode');
            }
            document.getElementById('themeName').innerText = THEMES[activeThemeIdx].name;
            showToast(`تم تطبيق مظهر: ${THEMES[activeThemeIdx].name}`);
        }

        async function fetchState() {
            try {
                const res = await fetch('/api/state');
                currentState = await res.json();
                renderUI();
                renderAiChat();
                renderExtensionsToolbar();
                renderScratchpad();
                renderDevTools();

                const banner = document.getElementById('activeDeviceBanner');
                if (banner && currentState.activeProfile) {
                    banner.innerText = `الجهاز النشط: #${currentState.activeProfile.id} (${currentState.activeProfile.name})`;
                }
            } catch (e) {
                console.error('Failed to fetch state from C++ backend', e);
            }
        }

        function showToast(msg, isSuccess = true) {
            const toast = document.getElementById('toastBox');
            toast.innerText = msg;
            toast.style.borderRightColor = isSuccess ? 'var(--accent)' : 'var(--danger)';
            toast.style.display = 'block';
            setTimeout(() => { toast.style.display = 'none'; }, 4000);
        }

        function toggleContainerMenu() {
            document.getElementById('containerMenu').classList.toggle('show');
        }

        function toggleAiDrawer() {
            document.getElementById('aiDrawer').classList.toggle('show');
        }

        function toggleScratchpad() {
            document.getElementById('scratchpadDrawer').classList.toggle('show');
        }

        function toggleDevTools() {
            document.getElementById('devToolsDock').classList.toggle('show');
            renderDevTools();
        }

        function toggleHardwareModal() {
            document.getElementById('hwModalBackdrop').classList.toggle('show');
        }

        function toggleMultiDeviceMatrix() {
            isMultiDeviceMatrix = !isMultiDeviceMatrix;
            showToast(isMultiDeviceMatrix ? '🖥️ تم تفعيل شاشة المصفوفة المتزامنة (3 أجهزة مختلفة جنباً إلى جنب)' : 'إلغاء وضع الشاشة المتعددة');
            const activeTab = currentState.tabs.find(t => t.id === currentState.activeTabId);
            if (activeTab) renderContent(activeTab);
        }

        function togglePip(show) {
            const pip = document.getElementById('pipWindow');
            if (show) pip.classList.add('show');
            else pip.classList.remove('show');
        }

        function pipTogglePlay() {
            const btn = document.getElementById('pipPlayBtn');
            btn.innerText = btn.innerText === '⏸️' ? '▶️' : '⏸️';
        }

        let pipSpeeds = ['1.0x', '1.25x', '1.5x', '2.0x', '0.5x'];
        let pipSpeedIdx = 0;
        function pipCycleSpeed() {
            pipSpeedIdx = (pipSpeedIdx + 1) % pipSpeeds.length;
            document.getElementById('pipSpeedBtn').innerText = pipSpeeds[pipSpeedIdx];
        }

        function pipToggleMute() {
            const btn = document.getElementById('pipMuteBtn');
            btn.innerText = btn.innerText === '🔊' ? '🔇' : '🔊';
        }

        async function applyHardwareLimit() {
            const cpu = parseInt(document.getElementById('cpuLimitText').innerText);
            const ram = parseInt(document.getElementById('ramLimitText').innerText);
            await fetch('/api/hardware/set', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ cpu: cpu, ram: ram })
            });
        }

        function toggleReaderMode() {
            navigate('about:reader');
        }

        window.onclick = function(e) {
            if (!e.target.matches('.container-dropdown *')) {
                const menu = document.getElementById('containerMenu');
                if (menu && menu.classList.contains('show')) menu.classList.remove('show');
            }
            if (!e.target.matches('.ext-btn, .ext-popup-dropdown *')) {
                const popup = document.getElementById('extPopupDropdown');
                if (popup && popup.classList.contains('show')) popup.classList.remove('show');
            }
        };

        function toggleVerticalTabs() {
            document.body.classList.toggle('vertical-mode');
            const isV = document.body.classList.contains('vertical-mode');
            document.getElementById('layoutText').innerText = isV ? 'أفقية' : 'جانبية';
            renderUI();
        }

        function renderExtensionsToolbar() {
            const group = document.getElementById('extToolbarGroup');
            if (!group || !currentState.extensions) return;
            group.innerHTML = '';

            const installed = currentState.extensions.filter(e => e.isInstalled);
            installed.forEach(ext => {
                const btn = document.createElement('button');
                btn.className = 'ext-btn' + (ext.isEnabled ? ' active-glow' : '');
                btn.title = `${ext.name} (${ext.isEnabled ? 'نشطة' : 'معطلة'})`;
                btn.innerHTML = ext.icon;
                btn.onclick = (e) => {
                    e.stopPropagation();
                    toggleExtensionPopup(ext.id);
                };
                group.appendChild(btn);
            });
        }

        function toggleExtensionPopup(id) {
            const popup = document.getElementById('extPopupDropdown');
            const ext = currentState.extensions.find(e => e.id === id);
            if (!popup || !ext) return;

            if (popup.dataset.activeId === id && popup.classList.contains('show')) {
                popup.classList.remove('show');
                return;
            }

            popup.dataset.activeId = id;
            popup.innerHTML = ext.popupHtml + `
                <div style="padding: 10px 16px; background:#0f172a; border-top:1px solid #334155; display:flex; justify-content:space-between; align-items:center;">
                    <button onclick="toggleExtension('${ext.id}', ${!ext.isEnabled})" style="background:${ext.isEnabled ? '#ef4444' : '#22c55e'}; color:#fff; border:none; padding:4px 10px; border-radius:4px; font-size:0.75rem; cursor:pointer;">
                        ${ext.isEnabled ? 'تعطيل الإضافة' : 'تفعيل الإضافة'}
                    </button>
                    <button onclick="uninstallExtension('${ext.id}')" style="background:transparent; color:#94a3b8; border:1px solid #475569; padding:4px 8px; border-radius:4px; font-size:0.75rem; cursor:pointer;">
                        إزالة من فايرفوكس
                    </button>
                </div>
            `;
            popup.classList.add('show');
        }

        async function installExtension(id, name, icon) {
            showToast(`📦 جاري تنزيل ملف .xpi لإضافة ${name} من addons.mozilla.org وفحص الصلاحيات...`);
            const res = await fetch('/api/extensions/install', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            const data = await res.json();
            if (data.status === 'ok') {
                setTimeout(() => {
                    showToast(`🎉 تم تثبيت إضافة ${name} (${icon}) بنجاح! الأداة تعمل وتُحقن في جميع الصفحات الآن.`);
                    fetchState();
                }, 500);
            }
        }

        async function toggleExtension(id, enabled) {
            await fetch('/api/extensions/toggle', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id, enabled: enabled ? 'true' : 'false' })
            });
            const popup = document.getElementById('extPopupDropdown');
            if (popup) popup.classList.remove('show');
            showToast(enabled ? 'تم تفعيل الإضافة بنجاح ✅' : 'تم تعطيل الإضافة مؤقتاً ⏸️');
            fetchState();
        }

        async function uninstallExtension(id) {
            await fetch('/api/extensions/uninstall', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            const popup = document.getElementById('extPopupDropdown');
            if (popup) popup.classList.remove('show');
            showToast('تمت إزالة الإضافة من المتصفح بنجاح');
            fetchState();
        }

        function switchDevTab(tab) {
            currentDevTab = tab;
            document.getElementById('tabBtnNetwork').className = 'dt-tab' + (tab==='network'?' active':'');
            document.getElementById('tabBtnConsole').className = 'dt-tab' + (tab==='console'?' active':'');
            document.getElementById('tabBtnDom').className = 'dt-tab' + (tab==='dom'?' active':'');
            renderDevTools();
        }

        function renderDevTools() {
            const body = document.getElementById('dtBody');
            if (!body) return;
            if (currentDevTab === 'network') {
                let html = '<div style="display:flex; flex-direction:column; gap:4px;">';
                html += '<div style="display:flex; justify-content:space-between; color:#64748b; font-size:0.75rem; border-bottom:1px solid #334155; padding-bottom:4px;"><span>المورد (URL)</span><span>الحالة</span><span>النوع</span><span>الحجم</span><span>الزمن</span></div>';
                (currentState.devNetwork || []).forEach(n => {
                    html += `<div style="display:flex; justify-content:space-between; padding:4px 0; border-bottom:1px solid rgba(255,255,255,0.03); color:#cbd5e1;">
                        <span style="direction:ltr; text-align:left; color:#38bdf8; overflow:hidden; text-overflow:ellipsis; max-width:40%;">${n.url}</span>
                        <span style="color:#22c55e;">${n.status} OK</span>
                        <span style="color:#94a3b8;">${n.mime}</span>
                        <span>${(n.size/1024).toFixed(1)} KB</span>
                        <span style="color:#fb923c;">${n.duration}ms</span>
                    </div>`;
                });
                html += '</div>';
                body.innerHTML = html;
            } else if (currentDevTab === 'console') {
                let html = '<div style="display:flex; flex-direction:column; gap:4px;">';
                (currentState.devConsole || []).forEach(c => {
                    html += `<div style="color:${c.level==='error'?'#ef4444':(c.level==='warn'?'#fb923c':'#38bdf8')};">[${c.time}] ${c.message}</div>`;
                });
                html += '</div>';
                html += '<div style="display:flex; gap:8px; margin-top:10px;"><span style="color:#38bdf8;">></span><input type="text" id="jsEvalInp" placeholder="نفّذ كود JavaScript هنا (مثال: document.title أو 2+2)..." onkeydown="if(event.key==='Enter') evalJs()" style="flex:1; background:#1e293b; border:1px solid #334155; color:#fff; padding:4px 8px; border-radius:4px; font-family:monospace; outline:none;"></div>';
                body.innerHTML = html;
            } else if (currentDevTab === 'dom') {
                body.innerHTML = '<pre style="color:#94a3b8; margin:0;">' + (currentState.devDom || '') + '</pre>';
            }
        }

        async function evalJs() {
            const inp = document.getElementById('jsEvalInp');
            const val = inp.value.trim();
            if (!val) return;
            inp.value = '';
            await fetch('/api/devtools/eval', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ code: val })
            });
            fetchState();
        }

        function renderScratchpad() {
            const list = document.getElementById('spNotesList');
            if (!list || !currentState.notes) return;
            list.innerHTML = '';
            currentState.notes.forEach(n => {
                const item = document.createElement('div');
                item.style = 'background:#1e293b; padding:10px; border-radius:6px; border:1px solid #334155;';
                item.innerHTML = `
                    <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:4px;">
                        <b style="color:#fff; font-size:0.85rem;">${n.title}</b>
                        <button onclick="deleteNote(${n.id})" style="background:transparent; border:none; color:#ef4444; cursor:pointer;">×</button>
                    </div>
                    <div style="color:#cbd5e1; font-size:0.8rem; line-height:1.4; white-space:pre-wrap;">${n.content}</div>
                    <div style="font-size:0.7rem; color:#64748b; margin-top:6px;">${n.updatedAt}</div>
                `;
                list.appendChild(item);
            });
        }

        async function saveNewNote() {
            const title = document.getElementById('spNewTitle').value.trim();
            const content = document.getElementById('spNewContent').value.trim();
            if (!content) return;
            await fetch('/api/notes/add', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ title: title, content: content })
            });
            document.getElementById('spNewTitle').value = '';
            document.getElementById('spNewContent').value = '';
            showToast('تم حفظ الملاحظة بنجاح 📝');
            fetchState();
        }

        async function clipActivePage() {
            const activeTab = currentState.tabs.find(t => t.id === currentState.activeTabId);
            const text = 'تم قص هذا الموقع وحفظه في الملاحظات السريعة كمرجع برمجي وبحثي.';
            await fetch('/api/notes/clip', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ selectedText: text, pageTitle: activeTab ? activeTab.title : '', pageUrl: activeTab ? activeTab.url : '' })
            });
            showToast('تم قص رابط ومحتوى الصفحة للملاحظات بنجاح ✂️');
            fetchState();
        }

        async function deleteNote(id) {
            await fetch('/api/notes/delete', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            fetchState();
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

            // 2. Render Tab Groups and Tabs
            const tabsList = document.getElementById('tabsList');
            const vTabsList = document.getElementById('vTabsList');
            tabsList.innerHTML = '';
            if (vTabsList) vTabsList.innerHTML = '';

            (currentState.tabGroups || []).forEach(g => {
                const gPill = document.createElement('div');
                gPill.className = 'tab-group-pill';
                gPill.style = `background:${g.color}22; color:${g.color}; border-color:${g.color}55;`;
                gPill.onclick = () => toggleTabGroup(g.id);
                gPill.innerHTML = `<span>● ${g.title}</span> <span style="font-size:0.65rem;">${g.isCollapsed?'▶':'▼'}</span>`;
                tabsList.appendChild(gPill);
            });

            const activeWsId = currentState.activeWorkspaceId || 1;
            const filteredTabs = currentState.tabs.filter(t => !t.workspaceId || t.workspaceId === activeWsId);

            filteredTabs.forEach(tab => {
                const tabEl = document.createElement('div');
                tabEl.className = 'tab' + (tab.id === currentState.activeTabId ? ' active' : '');
                tabEl.style.setProperty('--container-color', tab.containerColor || 'transparent');
                tabEl.onclick = () => switchTab(tab.id);

                let badgeHtml = '';
                if (tab.containerId > 0) {
                    badgeHtml = `<span class="container-badge" style="background:${tab.containerColor}">${tab.containerName}</span>`;
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
                splitText.innerText = currentState.splitView.enabled ? 'إلغاء التقسيم' : 'تقسيم';
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

        async function toggleTabGroup(id) {
            await fetch('/api/tabgroups/toggle', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            fetchState();
        }

        function renderAiChat() {
            const body = document.getElementById('aiChatBody');
            if (!body || !currentState.aiChat) return;
            body.innerHTML = '';
            currentState.aiChat.forEach(msg => {
                const d = document.createElement('div');
                d.className = 'ai-msg ' + msg.role;
                d.innerHTML = msg.text.replace(/\n/g, '<br>');
                body.appendChild(d);
            });
            body.scrollTop = body.scrollHeight;
        }

        async function sendAiMessage() {
            const inp = document.getElementById('aiInput');
            const val = inp.value.trim();
            if (!val) return;
            inp.value = '';
            await fetch('/api/ai/ask', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ query: val, action: 'ask' })
            });
            fetchState();
        }

        async function sendAiPrompt(action) {
            await fetch('/api/ai/ask', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ query: '', action: action })
            });
            const drawer = document.getElementById('aiDrawer');
            if (!drawer.classList.contains('show')) drawer.classList.add('show');
            fetchState();
        }

        async function clearAiChat() {
            await fetch('/api/ai/clear', { method: 'POST' });
            fetchState();
        }

        function renderContent(primaryTab) {
            const area = document.getElementById('contentArea');

            // Apply WebExtension Injected CSS dynamically to page
            let styleTag = document.getElementById('atlas-webextensions-injected-css');
            if (currentState.injectedCss && currentState.injectedCss.trim().length > 0) {
                if (!styleTag) {
                    styleTag = document.createElement('style');
                    styleTag.id = 'atlas-webextensions-injected-css';
                    document.head.appendChild(styleTag);
                }
                styleTag.textContent = currentState.injectedCss;
            } else if (styleTag) {
                styleTag.remove();
            }

            // Multi-Device Matrix View: 3 isolated browsers side by side
            if (isMultiDeviceMatrix) {
                area.className = 'content-area';
                area.innerHTML = `
                    <div style="display:grid; grid-template-columns: 1fr 1fr 1fr; gap:8px; height:100%; padding:8px; background:#0b1120;">
                        <!-- Browser 1 -->
                        <div style="background:#0f172a; border:2px solid #38bdf8; border-radius:10px; display:flex; flex-direction:column; overflow:hidden;">
                            <div style="background:#1e293b; padding:8px 12px; border-bottom:1px solid #334155; display:flex; justify-content:space-between; align-items:center;">
                                <b style="color:#38bdf8; font-size:0.85rem;">🪟 متصفح 1 (جهاز #1: Win 11)</b>
                                <span style="background:#38bdf822; color:#38bdf8; font-size:0.7rem; padding:2px 6px; border-radius:4px;">RTX 4090 • 32GB</span>
                            </div>
                            <div style="flex:1; padding:16px; overflow-y:auto; background:#182234; color:#fff;">
                                <h4>هوية عتادية مستقلة 100%</h4>
                                <p style="font-size:0.8rem; color:#94a3b8; margin-top:6px;">الكوكيز والجلسة معزولة تماماً في مجلد <code>profiles/device_1/</code>.</p>
                                <div style="margin-top:12px; background:#0f172a; padding:10px; border-radius:6px; font-size:0.75rem; font-family:monospace; color:#4ade80;">
                                  OS: Windows 11 Pro<br>
                                  Canvas Seed: #1037<br>
                                  Audio Delta: +0.000103
                                </div>
                            </div>
                        </div>
                        <!-- Browser 2 -->
                        <div style="background:#0f172a; border:2px solid #4ade80; border-radius:10px; display:flex; flex-direction:column; overflow:hidden;">
                            <div style="background:#1e293b; padding:8px 12px; border-bottom:1px solid #334155; display:flex; justify-content:space-between; align-items:center;">
                                <b style="color:#4ade80; font-size:0.85rem;">🍏 متصفح 2 (جهاز #2: macOS Sonoma)</b>
                                <span style="background:#4ade8022; color:#4ade80; font-size:0.7rem; padding:2px 6px; border-radius:4px;">Apple M3 Max • 36GB</span>
                            </div>
                            <div style="flex:1; padding:16px; overflow-y:auto; background:#182234; color:#fff;">
                                <h4>هوية عتادية مستقلة 100%</h4>
                                <p style="font-size:0.8rem; color:#94a3b8; margin-top:6px;">الكوكيز والجلسة معزولة تماماً في مجلد <code>profiles/device_2/</code>.</p>
                                <div style="margin-top:12px; background:#0f172a; padding:10px; border-radius:6px; font-size:0.75rem; font-family:monospace; color:#4ade80;">
                                  OS: macOS 14.5 Sonoma<br>
                                  Canvas Seed: #1074<br>
                                  Audio Delta: +0.000106
                                </div>
                            </div>
                        </div>
                        <!-- Browser 3 -->
                        <div style="background:#0f172a; border:2px solid #fb923c; border-radius:10px; display:flex; flex-direction:column; overflow:hidden;">
                            <div style="background:#1e293b; padding:8px 12px; border-bottom:1px solid #334155; display:flex; justify-content:space-between; align-items:center;">
                                <b style="color:#fb923c; font-size:0.85rem;">🐧 متصفح 3 (جهاز #4: Ubuntu Linux)</b>
                                <span style="background:#fb923c22; color:#fb923c; font-size:0.7rem; padding:2px 6px; border-radius:4px;">Intel UHD • 16GB</span>
                            </div>
                            <div style="flex:1; padding:16px; overflow-y:auto; background:#182234; color:#fff;">
                                <h4>هوية عتادية مستقلة 100%</h4>
                                <p style="font-size:0.8rem; color:#94a3b8; margin-top:6px;">الكوكيز والجلسة معزولة تماماً في مجلد <code>profiles/device_4/</code>.</p>
                                <div style="margin-top:12px; background:#0f172a; padding:10px; border-radius:6px; font-size:0.75rem; font-family:monospace; color:#4ade80;">
                                  OS: Ubuntu Linux 24.04<br>
                                  Canvas Seed: #1148<br>
                                  Audio Delta: +0.000112
                                </div>
                            </div>
                        </div>
                    </div>
                `;
                return;
            }

            // If on addons.mozilla.org, render the real AMO store
            if (primaryTab.url.includes('addons.mozilla.org') || primaryTab.url === 'about:addons') {
                renderAmoStore(area);
                return;
            }

            // If on about:passwords
            if (primaryTab.url === 'about:passwords') {
                renderPasswordVault(area);
                return;
            }

            // If on about:devices
            if (primaryTab.url === 'about:devices' || primaryTab.url === 'about:fingerprint') {
                renderDevicesManager(area);
                return;
            }

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
                            <span style="color: #4ade80; font-weight:bold;">اللسان الثاني: ${secondaryTab.title}</span>
                            <span style="font-size:0.75rem; color:${secondaryTab.containerColor}">${secondaryTab.containerName}</span>
                        </div>
                        <div style="flex:1; overflow-y:auto;">${secondaryTab.content || ''}</div>
                    </div>
                `;
            } else {
                area.classList.remove('split-active');
                area.innerHTML = primaryTab.content || '';
            }
        }

        function renderDevicesManager(container) {
            const profiles = (currentState.profiles && currentState.profiles.profiles) ? currentState.profiles.profiles : [];
            let filtered = profiles;
            if (deviceSearchQuery) {
                const q = deviceSearchQuery.toLowerCase();
                filtered = filtered.filter(p => p.name.toLowerCase().includes(q) || p.osType.toLowerCase().includes(q) || p.id.toString() === q);
            }

            let cardsHtml = '';
            filtered.forEach(p => {
                const isActive = (currentState.activeProfile && currentState.activeProfile.id === p.id);
                cardsHtml += `
                    <div style="background:#1e293b; border:1px solid ${isActive?'#38bdf8':'#334155'}; border-radius:12px; padding:18px; display:flex; justify-content:space-between; align-items:center; transition:all 0.2s;">
                        <div style="display:flex; gap:16px; align-items:center;">
                            <div style="font-size:2rem; background:${isActive?'rgba(56,189,248,0.2)':'rgba(255,255,255,0.05)'}; width:56px; height:56px; border-radius:10px; display:flex; align-items:center; justify-content:center;">
                                ${p.osType.includes('Windows')?'🪟':(p.osType.includes('macOS')?'🍏':'🐧')}
                            </div>
                            <div>
                                <div style="display:flex; align-items:center; gap:8px;">
                                    <b style="color:#fff; font-size:1.05rem;">${p.name}</b>
                                    <span style="background:rgba(56,189,248,0.15); color:#38bdf8; font-size:0.75rem; padding:2px 8px; border-radius:12px;">ID #${p.id}</span>
                                    ${isActive?'<span style="background:#22c55e22; color:#22c55e; font-size:0.75rem; padding:2px 8px; border-radius:12px; font-weight:bold;">● الجهاز النشط حالياً</span>':''}
                                </div>
                                <div style="color:#94a3b8; font-size:0.8rem; margin-top:4px;">
                                    <span>الأنوية: <b>${p.hardwareConcurrency} Cores</b></span> • 
                                    <span>الرام: <b>${p.deviceMemory} GB</b></span> • 
                                    <span>الشاشة: <b>${p.screenWidth}×${p.screenHeight}</b></span> • 
                                    <span>كرت الشاشة: <b style="color:#cbd5e1;">${p.webglVendor}</b></span>
                                </div>
                                <div style="font-size:0.7rem; color:#64748b; margin-top:4px; font-family:monospace;">
                                    Canvas Seed: #${p.canvasSeed} | مسار التخزين: ${p.storage}/
                                </div>
                            </div>
                        </div>
                        <div style="display:flex; gap:8px;">
                            ${isActive ? 
                                '<button style="background:#22c55e; color:#0f172a; border:none; padding:8px 16px; border-radius:6px; font-weight:bold; font-size:0.8rem;">✓ الجهاز نشط</button>' : 
                                `<button onclick="switchDeviceProfile(${p.id})" style="background:#0284c7; color:#fff; border:none; padding:8px 16px; border-radius:6px; font-weight:bold; font-size:0.8rem; cursor:pointer;">▶️ تفعيل هذا الجهاز</button>`
                            }
                        </div>
                    </div>
                `;
            });

            container.innerHTML = `
                <div style="max-width:1050px; margin:24px auto; padding:0 20px;">
                    <!-- Devices Header Banner -->
                    <div style="background: linear-gradient(135deg, #0f172a 0%, #1e1b4b 100%); border:1px solid #334155; border-radius:16px; padding:24px 30px; margin-bottom:20px; display:flex; justify-content:space-between; align-items:center;">
                        <div>
                            <div style="display:flex; align-items:center; gap:10px;">
                                <span style="font-size:2rem;">💻</span>
                                <h1 style="color:#fff; font-size:1.8rem; margin:0;">مركز الأجهزة الافتراضية (200 Virtual Devices Hub)</h1>
                            </div>
                            <p style="color:#94a3b8; font-size:0.9rem; margin-top:6px;">
                                200 بيئة تصفح معزولة تماماً. كل جهاز يمتلك كوكيز مستقلة، بصمة Canvas مشوشة، وكرت شاشة ومواصفات عتادية فريدة تمنع المواقع من الربط بينها.
                            </p>
                        </div>
                        <button onclick="toggleMultiDeviceMatrix()" style="background:#0060df; color:#fff; border:none; padding:10px 18px; border-radius:8px; font-weight:bold; cursor:pointer;">🖥️ شاشة متعددة متزامنة</button>
                    </div>

                    <!-- Filter Controls -->
                    <div style="display:flex; gap:12px; margin-bottom:16px;">
                        <input type="text" value="${deviceSearchQuery}" placeholder="🔍 ابحث برقم الجهاز (1 إلى 200) أو النظام (Windows, macOS, Linux)..." oninput="deviceSearchQuery=this.value; renderDevicesManager(document.getElementById('contentArea'))" style="flex:1; background:#1e293b; border:1px solid #334155; color:#fff; padding:10px 16px; border-radius:8px; outline:none; font-size:0.95rem;">
                        <button onclick="deviceSearchQuery=''; renderDevicesManager(document.getElementById('contentArea'))" style="background:#1e293b; color:#94a3b8; border:1px solid #334155; padding:8px 16px; border-radius:8px; cursor:pointer;">عرض الكل (200)</button>
                    </div>

                    <!-- Devices List -->
                    <div style="display:flex; flex-direction:column; gap:12px; max-height:650px; overflow-y:auto; padding-left:4px;">
                        ${cardsHtml}
                    </div>
                </div>
            `;
        }

        async function switchDeviceProfile(id) {
            showToast(`جاري عزل التخزين والكوكيز وتفعيل مواصفات الجهاز الافتراضي #${id}...`);
            await fetch('/api/profiles/switch', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            setTimeout(() => {
                showToast(`✅ تم تفعيل الجهاز الافتراضي #${id} بنجاح! تم تطبيق بصمة العتاد المعزولة.`);
                fetchState();
            }, 300);
        }

        function renderPasswordVault(container) {
            const v = currentState.vault || { entries: [] };
            let rowsHtml = '';
            v.entries.forEach(e => {
                rowsHtml += `
                    <tr style="border-bottom:1px solid #334155;">
                        <td style="padding:12px; color:#38bdf8; font-weight:bold;">${e.website}</td>
                        <td style="padding:12px;">${e.username}</td>
                        <td style="padding:12px; font-family:monospace;">••••••••••••</td>
                        <td style="padding:12px;">
                            <span style="background:${e.strength>70?'rgba(34,197,94,0.2)':'rgba(251,146,60,0.2)'}; color:${e.strength>70?'#22c55e':'#fb923c'}; padding:2px 8px; border-radius:10px; font-size:0.75rem;">${e.strength}% أمان</span>
                        </td>
                        <td style="padding:12px;">
                            <span style="color:${e.isBreached?'#ef4444':'#22c55e'}; font-size:0.8rem;">${e.isBreached?'⚠️ تم رصد تسريب':'✅ آمن تماماً'}</span>
                        </td>
                        <td style="padding:12px;">
                            <button onclick="deleteVaultEntry(${e.id})" style="background:transparent; border:1px solid #ef4444; color:#ef4444; padding:2px 8px; border-radius:4px; font-size:0.75rem; cursor:pointer;">حذف</button>
                        </td>
                    </tr>
                `;
            });

            container.innerHTML = `
                <div style="max-width:950px; margin:30px auto; padding:0 20px;">
                    <div style="display:flex; justify-content:space-between; align-items:center; border-bottom:2px solid #334155; padding-bottom:16px; margin-bottom:24px;">
                        <div>
                            <h1 style="color:#38bdf8; font-size:1.8rem; margin:0;">🔐 الخزنة المشفرة لكلمات المرور (AES-256 Vault)</h1>
                            <p style="color:#94a3b8; margin-top:4px;">تشفير محلي سيادي مع فحص استباقي للتسريبات ومولد كلمات مرور قوية</p>
                        </div>
                        <span style="background:rgba(34,197,94,0.2); color:#22c55e; border:1px solid #22c55e; padding:6px 14px; border-radius:20px; font-weight:bold; font-size:0.85rem;">الخزنة مؤمنة ومقفلة تلقائياً</span>
                    </div>

                    <!-- Password Generator Card -->
                    <div style="background:#1e293b; border:1px solid #334155; border-radius:12px; padding:18px; margin-bottom:24px; display:flex; justify-content:space-between; align-items:center;">
                        <div>
                            <b style="color:#fff;">🔑 مولد كلمات المرور المشفرة:</b>
                            <div id="generatedPassDisplay" style="font-family:monospace; color:#38bdf8; font-size:1.1rem; margin-top:4px;">P@ssw0rdSecure!2026#Atlas</div>
                        </div>
                        <button onclick="generateNewPassword()" style="background:#0284c7; color:#fff; border:none; padding:8px 16px; border-radius:8px; font-weight:bold; cursor:pointer;">توليد كلمة سر جديدة</button>
                    </div>

                    <!-- Passwords Table -->
                    <div style="background:#1e293b; border:1px solid #334155; border-radius:12px; overflow:hidden;">
                        <table style="width:100%; border-collapse:collapse; text-align:right; font-size:0.85rem;">
                            <thead style="background:#0f172a; color:#94a3b8;">
                                <tr>
                                    <th style="padding:12px;">الموقع</th>
                                    <th style="padding:12px;">اسم المستخدم</th>
                                    <th style="padding:12px;">كلمة المرور</th>
                                    <th style="padding:12px;">مستوى القوة</th>
                                    <th style="padding:12px;">حالة التسريب</th>
                                    <th style="padding:12px;">إجراءات</th>
                                </tr>
                            </thead>
                            <tbody>
                                ${rowsHtml}
                            </tbody>
                        </table>
                    </div>
                </div>
            `;
        }

        async function generateNewPassword() {
            const res = await fetch('/api/vault/generate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ length: 18 })
            });
            const data = await res.json();
            document.getElementById('generatedPassDisplay').innerText = data.password;
            showToast('تم توليد كلمة سر معقدة وقوية بنجاح 🔑');
        }

        async function deleteVaultEntry(id) {
            await fetch('/api/vault/delete', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: id })
            });
            showToast('تم حذف السجل من الخزنة');
            fetchState();
        }

        function renderAmoStore(container) {
            let filtered = currentState.extensions || [];
            if (amoCategoryFilter !== 'all') {
                filtered = filtered.filter(e => e.category.includes(amoCategoryFilter));
            }
            if (amoSearchQuery) {
                const q = amoSearchQuery.toLowerCase();
                filtered = filtered.filter(e => e.name.toLowerCase().includes(q) || e.description.toLowerCase().includes(q));
            }

            let cardsHtml = '';
            filtered.forEach(ext => {
                let actionBtn = '';
                if (!ext.isInstalled) {
                    actionBtn = `<button onclick="installExtension('${ext.id}', '${ext.name}', '${ext.icon}')" style="background:#0060df; color:#fff; border:none; padding:10px 18px; border-radius:8px; font-weight:bold; cursor:pointer; font-size:0.9rem; white-space:nowrap; transition:background 0.2s;">+ أضف إلى فايرفوكس</button>`;
                } else {
                    actionBtn = `
                        <div style="display:flex; flex-direction:column; gap:6px; align-items:flex-end;">
                            <span style="background:rgba(34,197,94,0.15); color:#22c55e; border:1px solid #22c55e; padding:4px 10px; border-radius:6px; font-size:0.8rem; font-weight:bold;">✓ مثبتة في المتصفح</span>
                            <div style="display:flex; gap:6px;">
                                <button onclick="toggleExtension('${ext.id}', ${!ext.isEnabled})" style="background:${ext.isEnabled ? '#e11d48' : '#059669'}; color:#fff; border:none; padding:4px 10px; border-radius:4px; font-size:0.75rem; cursor:pointer;">${ext.isEnabled ? 'تعطيل' : 'تفعيل'}</button>
                                <button onclick="uninstallExtension('${ext.id}')" style="background:transparent; color:#94a3b8; border:1px solid #475569; padding:4px 8px; border-radius:4px; font-size:0.75rem; cursor:pointer;">إزالة</button>
                            </div>
                        </div>
                    `;
                }

                cardsHtml += `
                    <div style="background:#1e293b; border:1px solid #334155; border-radius:12px; padding:20px; display:flex; justify-content:space-between; align-items:center; transition:border-color 0.2s;">
                        <div style="display:flex; gap:16px; align-items:center; flex:1;">
                            <div style="font-size:2.4rem; background:rgba(56,189,248,0.1); width:64px; height:64px; border-radius:12px; display:flex; align-items:center; justify-content:center;">${ext.icon}</div>
                            <div style="flex:1;">
                                <div style="display:flex; align-items:center; gap:8px;">
                                    <h3 style="color:#fff; font-size:1.15rem; margin:0;">${ext.name}</h3>
                                    <span style="background:rgba(251,146,60,0.2); color:#fb923c; font-size:0.7rem; padding:1px 6px; border-radius:4px;">v${ext.version}</span>
                                    <span style="color:#38bdf8; font-size:0.75rem;">بواسطة: ${ext.author}</span>
                                </div>
                                <p style="color:#94a3b8; font-size:0.85rem; margin-top:6px; line-height:1.4;">${ext.description}</p>
                                <div style="display:flex; gap:16px; margin-top:8px; font-size:0.75rem; color:#64748b;">
                                    <span>👥 ${ext.users}</span>
                                    <span style="color:#f59e0b;">★ ${ext.rating} / 5</span>
                                    <span style="color:#38bdf8;">🏷️ ${ext.category}</span>
                                </div>
                            </div>
                        </div>
                        <div style="margin-right:20px;">
                            ${actionBtn}
                        </div>
                    </div>
                `;
            });

            container.innerHTML = `
                <div style="max-width:980px; margin:0 auto; padding:30px 20px;">
                    <!-- AMO Header -->
                    <div style="background: linear-gradient(135deg, #0f172a 0%, #1e1b4b 100%); border:1px solid #334155; border-radius:16px; padding:24px 30px; margin-bottom:24px; display:flex; justify-content:space-between; align-items:center;">
                        <div>
                            <div style="display:flex; align-items:center; gap:10px;">
                                <span style="font-size:2rem;">🦊</span>
                                <h1 style="color:#fff; font-size:1.8rem; margin:0;">متجر إضافات فايرفوكس (addons.mozilla.org)</h1>
                            </div>
                            <p style="color:#94a3b8; font-size:0.9rem; margin-top:6px;">مستودع إضافات WebExtensions الرسمي لنواة فايرفوكس. ثبّت الإضافات فوراً لتعمل مباشرة على المتصفح.</p>
                        </div>
                        <span style="background:#0060df; color:#fff; padding:6px 14px; border-radius:20px; font-weight:bold; font-size:0.85rem;">AMO v128.0 متصل</span>
                    </div>

                    <!-- Search & Filter Controls -->
                    <div style="display:flex; gap:12px; margin-bottom:20px; flex-wrap:wrap;">
                        <input type="text" value="${amoSearchQuery}" placeholder="🔍 ابحث في آلاف الإضافات الرسمية..." oninput="amoSearchQuery=this.value; renderAmoStore(document.getElementById('contentArea'))" style="flex:1; min-width:240px; background:#1e293b; border:1px solid #334155; color:#fff; padding:10px 16px; border-radius:8px; outline:none; font-size:0.95rem;">
                        <button onclick="amoCategoryFilter='all'; renderAmoStore(document.getElementById('contentArea'))" style="background:${amoCategoryFilter==='all'?'#0060df':'#1e293b'}; color:#fff; border:1px solid #334155; padding:8px 16px; border-radius:8px; cursor:pointer;">الكل</button>
                        <button onclick="amoCategoryFilter='المظهر'; renderAmoStore(document.getElementById('contentArea'))" style="background:${amoCategoryFilter==='المظهر'?'#0060df':'#1e293b'}; color:#fff; border:1px solid #334155; padding:8px 16px; border-radius:8px; cursor:pointer;">المظهر وراحة العين</button>
                        <button onclick="amoCategoryFilter='الأمان'; renderAmoStore(document.getElementById('contentArea'))" style="background:${amoCategoryFilter==='الأمان'?'#0060df':'#1e293b'}; color:#fff; border:1px solid #334155; padding:8px 16px; border-radius:8px; cursor:pointer;">الأمان والخصوصية</button>
                        <button onclick="amoCategoryFilter='الإنتاجية'; renderAmoStore(document.getElementById('contentArea'))" style="background:${amoCategoryFilter==='الإنتاجية'?'#0060df':'#1e293b'}; color:#fff; border:1px solid #334155; padding:8px 16px; border-radius:8px; cursor:pointer;">الإنتاجية والترجمة</button>
                    </div>

                    <!-- Extensions List -->
                    <div style="display:flex; flex-direction:column; gap:14px;">
                        ${cardsHtml}
                    </div>
                </div>
            `;
        }

        async function switchWorkspace(wsId) {
            await fetch('/api/workspaces/switch', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id: wsId })
            });
            fetchState();
        }

        async function toggleSplitView() {
            await fetch('/api/splitview/toggle', { method: 'POST' });
            fetchState();
        }

        // Command Palette
        const COMMANDS = [
            { id: 'storage', title: 'إدارة التخزين فائق الخفة والعزل لـ 200 متصفح (about:storage)', icon: '💾', action: () => navigate('about:storage') },
            { id: 'devices', title: 'إدارة 200 متصفح وجهاز افتراضي (about:devices)', icon: '💻', action: () => navigate('about:devices') },
            { id: 'matrix', title: 'شاشة المتصفحات المتزامنة (Multi-Device Matrix)', icon: '🖥️', action: () => toggleMultiDeviceMatrix() },
            { id: 'fingerprint', title: 'فحص بصمة الجهاز والعتاد (about:fingerprint)', icon: '🛡️', action: () => navigate('about:fingerprint') },
            { id: 'devtools', title: 'أدوات المطورين وفاحص الشبكة (F12 DevTools)', icon: '🛠️', action: () => toggleDevTools() },
            { id: 'vault', title: 'الخزنة المشفرة لكلمات المرور (Password Vault)', icon: '🔐', action: () => navigate('about:passwords') },
            { id: 'notes', title: 'لوحة الملاحظات وقصاصات الويب (Scratchpad)', icon: '📝', action: () => toggleScratchpad() },
            { id: 'hardware', title: 'لوحة التحكم في استهلاك العتاد والميديا (Opera GX)', icon: '🎛️', action: () => toggleHardwareModal() },
            { id: 'pip', title: 'تشغيل الفيديو العائم (Picture-in-Picture)', icon: '🎥', action: () => togglePip(true) },
            { id: 'amo', title: 'متجر إضافات فايرفوكس (addons.mozilla.org)', icon: '🧩', action: () => navigate('https://addons.mozilla.org/firefox/') },
            { id: 'dark_reader', title: 'تثبيت وتشغيل Dark Reader فوراً', icon: '🌙', action: () => installExtension('darkreader@firefox', 'Dark Reader', '🌙') },
            { id: 'ublock', title: 'تثبيت وتشغيل uBlock Origin فوراً', icon: '🛑', action: () => installExtension('uBlock0@raymondhill.net', 'uBlock Origin', '🛑') },
            { id: 'ai', title: 'المساعد الذكي (Open Atlas Copilot AI)', icon: '🤖', action: () => toggleAiDrawer() },
            { id: 'downloads', title: 'مدير التنزيلات فائق السرعة (about:downloads)', icon: '📥', action: () => navigate('about:downloads') },
            { id: 'perf', title: 'مركز مراقبة الأداء واستهلاك الرام (about:performance)', icon: '⚡', action: () => navigate('about:performance') },
            { id: 'split', title: 'تقسيم الشاشة لعرض لسانين (Split View)', icon: '🪟', action: () => toggleSplitView() }
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
            } else if (e.key === 'F12') {
                e.preventDefault();
                toggleDevTools();
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

        fetchState();
    </script>
</body>
</html>
)RAW_HTML";

static std::string escapeJsonString(const std::string& str) {
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

static std::string buildStateJson() {
    auto tabs = g_engine.tabs()->getAllTabs();
    uint32_t activeId = g_engine.tabs()->getActiveTabId();
    auto stats = g_engine.adBlocker()->getStats();
    auto bookmarks = g_engine.storage()->getBookmarks();

    auto curTab = g_engine.tabs()->getActiveTab();
    std::string curUrl = curTab ? curTab->currentUrl : "";
    std::string injectedCss = g_engine.extensions()->getInjectedCssForUrl(curUrl);
    std::string injectedJs = g_engine.extensions()->getInjectedJsForUrl(curUrl);

    auto activeProfile = g_engine.profiles()->getActiveProfile();
    std::string antiDetectScript = g_engine.profiles()->getActiveAntiDetectScript();

    std::ostringstream ss;
    ss << "{\n"
       << "  \"activeTabId\": " << activeId << ",\n"
       << "  \"adBlockEnabled\": " << (g_engine.adBlocker()->isEnabled() ? "true" : "false") << ",\n"
       << "  \"injectedCss\": \"" << escapeJsonString(injectedCss) << "\",\n"
       << "  \"injectedJs\": \"" << escapeJsonString(injectedJs) << "\",\n"
       << "  \"antiDetectScript\": \"" << escapeJsonString(antiDetectScript) << "\",\n"
       << "  \"activeProfile\": " << (activeProfile ? activeProfile->toJson() : "null") << ",\n"
       << "  \"profiles\": " << g_engine.profiles()->exportProfilesJson(1, 200, "") << ",\n"
       << "  \"stats\": {\n"
       << "    \"totalBlocked\": " << stats.totalBlocked << ",\n"
       << "    \"adsBlocked\": " << stats.adsBlocked << ",\n"
       << "    \"trackersBlocked\": " << stats.trackersBlocked << ",\n"
       << "    \"analyticsBlocked\": " << stats.analyticsBlocked << ",\n"
       << "    \"requestsChecked\": " << stats.requestsChecked << "\n"
       << "  },\n"
       << "  \"devNetwork\": " << g_engine.devTools()->exportNetworkJson() << ",\n"
       << "  \"devConsole\": " << g_engine.devTools()->exportConsoleJson() << ",\n"
       << "  \"devDom\": \"" << escapeJsonString(g_engine.devTools()->inspectDom(curUrl, "")) << "\",\n"
       << "  \"vault\": " << g_engine.vault()->exportVaultJson() << ",\n"
       << "  \"tabGroups\": " << g_engine.tabGroups()->exportGroupsJson() << ",\n"
       << "  \"notes\": " << g_engine.scratchpad()->exportNotesJson() << ",\n"
       << "  \"hardware\": " << g_engine.hardware()->exportHardwareJson() << ",\n"
       << "  \"storageMetrics\": " << g_engine.ultraStorage()->exportGlobalMetricsJson() << ",\n"
       << "  \"tabs\": [\n";

    for (size_t i = 0; i < tabs.size(); ++i) {
        if (i > 0) ss << ",\n";
        std::string contentPreview = "";
        if (tabs[i].currentUrl.find("addons.mozilla.org") != std::string::npos || tabs[i].currentUrl == "about:addons") {
            contentPreview = "<!-- AMO Store -->";
        } else if (tabs[i].currentUrl == "about:passwords") {
            contentPreview = "<!-- Password Vault -->";
        } else if (tabs[i].currentUrl == "about:devices" || tabs[i].currentUrl == "about:fingerprint") {
            contentPreview = "<!-- Devices Hub -->";
        } else if (tabs[i].currentUrl == "about:storage" || tabs[i].currentUrl.rfind("mybrowser://storage", 0) == 0) {
            contentPreview = g_engine.ultraStorage()->generateStorageDiagnosticsHtml();
        } else if (tabs[i].currentUrl.rfind("about:downloads", 0) == 0 || tabs[i].currentUrl.rfind("mybrowser://downloads", 0) == 0) {
            auto dls = g_engine.downloads()->getAllDownloads();
            std::ostringstream dlStream;
            dlStream << "<div style=\"padding:40px; max-width:850px; margin:0 auto;\">"
                     << "  <div style=\"display:flex; justify-content:space-between; align-items:center; border-bottom:2px solid #334155; padding-bottom:14px; margin-bottom:20px;\">"
                     << "    <div>"
                     << "      <h1 style=\"color:#38bdf8; font-size:1.8rem; margin:0;\">📥 مدير التنزيلات السريعة (C++ Turbo Downloads)</h1>"
                     << "      <p style=\"color:#94a3b8; margin-top:4px;\">تنزيل متعدد المسارات (Multi-threaded Chunks) مع فحص أمني استباقي</p>"
                     << "    </div>"
                     << "    <span style=\"background:rgba(34,197,94,0.2); color:#22c55e; border:1px solid #22c55e; padding:4px 12px; border-radius:20px; font-weight:bold; font-size:0.8rem;\">المحرك يعمل بأقصى سرعة</span>"
                     << "  </div>"
                     << "  <div style=\"display:flex; flex-direction:column; gap:14px;\">";
            for (const auto& d : dls) {
                dlStream << "    <div style=\"background:#1e293b; padding:18px; border-radius:10px; border:1px solid #334155; display:flex; justify-content:space-between; align-items:center;\">"
                         << "      <div>"
                         << "        <div style=\"font-size:1.1rem; font-weight:bold; color:#fff;\">📦 " << d.fileName << "</div>"
                         << "        <div style=\"font-size:0.85rem; color:#38bdf8; margin-top:4px;\">السرعة: " << d.speedStr << " • " << d.securityNotice << "</div>"
                         << "        <div style=\"font-size:0.75rem; color:#94a3b8; margin-top:2px; direction:ltr; text-align:right;\">" << d.url << "</div>"
                         << "      </div>"
                         << "      <div style=\"text-align:left;\">"
                         << "        <span style=\"background:#22c55e; color:#0f172a; padding:6px 14px; border-radius:6px; font-weight:bold; font-size:0.85rem;\">" << downloadStatusToString(d.status) << "</span>"
                         << "      </div>"
                         << "    </div>";
            }
            dlStream << "  </div>"
                     << "</div>";
            contentPreview = dlStream.str();
        } else if (tabs[i].currentUrl.rfind("about:performance", 0) == 0 || tabs[i].currentUrl.rfind("mybrowser://performance", 0) == 0) {
            auto allTabs = g_engine.tabs()->getAllTabs();
            std::vector<uint32_t> ids;
            for (const auto& t : allTabs) ids.push_back(t.id);
            auto metrics = g_engine.performance()->getTabMetrics(ids);
            contentPreview = g_engine.performance()->generatePerformanceHtml(metrics);
        } else if (tabs[i].currentUrl.rfind("about:reader", 0) == 0 || tabs[i].currentUrl.rfind("mybrowser://reader", 0) == 0) {
            std::string articleTitle = "بناء محرك متصفح عملاق C++ على نواة Gecko";
            std::string articleAuthor = "فريق هندسة النواة AtlasBrowser";
            std::string articleBody = "<p>تعتبر بنية متصفح AtlasBrowser Gecko Edition قفزة نوعية في عالم المتصفحات مفتوحة المصدر، حيث تدمج بين خفة وأداء مكتبات C++20 الأصلية وبين مرونة نواة فايرفوكس الرائدة في دعم إضافات WebExtensions.</p>"
                                      "<p>من خلال عزل حاويات العمل والشخصي (Multi-Account Containers)، يضمن المتصفح استقلالية ملفات تعريف الارتباط والجلسات، مما يمنع شركات الإعلانات من تتبع المستخدم عبر الويب.</p>"
                                      "<p>بالإضافة إلى ذلك، يوفر المحرك طبقة مساعدة ذكية (Native AI Copilot) تعمل على تلخيص المحتوى وشرح الأكواد البرمجية مباشرة دون استهلاك موارد المعالج والذاكرة.</p>";
            contentPreview = ReaderModeEngine::formatReaderArticle(articleTitle, articleAuthor, articleBody);
        } else if (tabs[i].currentUrl.rfind("mybrowser://", 0) == 0) {
            auto schemeResp = g_engine.schemes()->handleRequest(tabs[i].currentUrl);
            contentPreview = schemeResp.content;
        } else {
            contentPreview = "<div style=\"max-width:860px; margin:40px auto; padding:30px; background:#ffffff; color:#1f2937; border-radius:12px; box-shadow:0 10px 30px rgba(0,0,0,0.1); font-family:system-ui, sans-serif;\">"
                             "<div style=\"display:flex; justify-content:space-between; align-items:center; border-bottom:1px solid #e5e7eb; padding-bottom:16px; margin-bottom:20px;\">"
                             "  <div>"
                             "    <h1 style=\"color:#0f172a; font-size:1.6rem; margin:0;\">🌐 " + tabs[i].title + "</h1>"
                             "    <div style=\"font-size:0.85rem; color:#6b7280; margin-top:4px; direction:ltr; text-align:right;\">" + tabs[i].currentUrl + "</div>"
                             "  </div>"
                             "  <span style=\"background:#e0f2fe; color:#0369a1; padding:4px 12px; border-radius:20px; font-weight:bold; font-size:0.8rem;\">اتصال آمن ومحمي ✅</span>"
                             "</div>"
                             "<div class=\"ad-banner\" style=\"background:#fef3c7; border:1px dashed #f59e0b; padding:12px; border-radius:8px; margin-bottom:20px; text-align:center; color:#b45309; font-size:0.85rem;\">"
                             "  [إعلان تجريبي] تم فحص هذا الموقع وتأمينه بواسطة درع حظر الإعلانات والتعقب في النواة"
                             "</div>"
                             "<p style=\"font-size:1.05rem; line-height:1.7; color:#374151; margin-bottom:16px;\">"
                             "أهلاً بك في الصفحة المعروضة! تم تفعيل نظام <b>الـ 200 جهاز افتراضي (Anti-Detect Multi-Profiles)</b> في هذا التبويب:"
                             "</p>"
                             "<div style=\"background:#f3f4f6; padding:16px; border-radius:8px; border:1px solid #e5e7eb; margin-bottom:20px;\">"
                             "  <ul style=\"margin-right:20px; color:#4b5563; font-size:0.9rem; line-height:1.7;\">"
                             "    <li><b>💻 200 جهاز افتراضي:</b> يمكنك التبديل بين 200 بيئة هاردوير ونظام مستقلة على <code>about:devices</code>.</li>"
                             "    <li><b>🖥️ شاشة متعددة:</b> اضغط زر 'شاشة متعددة' لتشغيل 3 متصفحات كأجهزة مختلفة تماماً جنباً إلى جنب.</li>"
                             "    <li><b>🛡️ عزل تام للكوكيز:</b> كل جهاز يمتلك ملف تعريف ومسار بيانات مغلق تماماً لا يرى المتصفحات الأخرى.</li>"
                             "    <li><b>🎭 تزييف البصمة (Canvas/WebGL):</b> كل جهاز يولد بصمة رسومية وصوتية فريدة لا تتطابق مع غيره.</li>"
                             "  </ul>"
                             "</div>"
                             "</div>";
        }

        std::string escContent = escapeJsonString(contentPreview);

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
       << "  \"downloads\": " << g_engine.downloads()->exportDownloadsJson() << ",\n"
       << "  \"aiChat\": " << g_engine.ai()->exportChatJson() << ",\n"
       << "  \"extensions\": " << g_engine.extensions()->exportExtensionsJson() << ",\n"
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
    std::string req(buffer);

    std::istringstream reqStream(req);
    std::string method, path, proto;
    reqStream >> method >> path >> proto;

    size_t bodyPos = req.find("\r\n\r\n");
    std::string body = (bodyPos != std::string::npos) ? req.substr(bodyPos + 4) : "";

    std::string responseBody;
    std::string contentType = "text/html; charset=utf-8";
    std::string responseStatus = "200 OK";

    if (method == "GET" && (path == "/" || path == "/index.html")) {
        responseBody = HTML_UI;
    } else if (method == "GET" && path == "/api/state") {
        contentType = "application/json";
        responseBody = buildStateJson();
    } else if (method == "POST" && path == "/api/storage/compact") {
        g_engine.ultraStorage()->compactAll();
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/storage/hibernate_inactive") {
        auto cur = g_engine.profiles()->getActiveProfile();
        uint32_t activeId = cur ? cur->id : 1;
        size_t count = g_engine.ultraStorage()->hibernateAllInactive(activeId);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\", \"hibernated\": " + std::to_string(count) + "}";
    } else if (method == "POST" && path == "/api/storage/wake") {
        std::string idStr = extractJsonField(body, "id");
        if (!idStr.empty()) g_engine.ultraStorage()->wakeProfile(std::stoi(idStr));
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/profiles/switch") {
        std::string idStr = extractJsonField(body, "id");
        if (!idStr.empty()) {
            uint32_t id = std::stoi(idStr);
            g_engine.profiles()->switchActiveProfile(id);
        }
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/devtools/eval") {
        std::string code = extractJsonField(body, "code");
        auto cur = g_engine.tabs()->getActiveTab();
        std::string res = g_engine.devTools()->evaluateJs(code, cur ? cur->currentUrl : "");
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\", \"result\": \"" + escapeJsonString(res) + "\"}";
    } else if (method == "POST" && path == "/api/vault/add") {
        std::string website = extractJsonField(body, "website");
        std::string username = extractJsonField(body, "username");
        std::string password = extractJsonField(body, "password");
        uint32_t id = g_engine.vault()->addEntry(website, username, password);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\", \"id\": " + std::to_string(id) + "}";
    } else if (method == "POST" && path == "/api/vault/generate") {
        std::string lenStr = extractJsonField(body, "length");
        int len = lenStr.empty() ? 16 : std::stoi(lenStr);
        std::string gen = PasswordVault::generateStrongPassword(len, true);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\", \"password\": \"" + gen + "\"}";
    } else if (method == "POST" && path == "/api/vault/delete") {
        std::string idStr = extractJsonField(body, "id");
        if (!idStr.empty()) g_engine.vault()->deleteEntry(std::stoi(idStr));
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/notes/add") {
        std::string title = extractJsonField(body, "title");
        std::string content = extractJsonField(body, "content");
        auto cur = g_engine.tabs()->getActiveTab();
        g_engine.scratchpad()->createNote(title, content, cur ? cur->currentUrl : "");
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/notes/clip") {
        std::string selectedText = extractJsonField(body, "selectedText");
        std::string pageTitle = extractJsonField(body, "pageTitle");
        std::string pageUrl = extractJsonField(body, "pageUrl");
        g_engine.scratchpad()->clipWebSelection(selectedText, pageTitle, pageUrl);
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/notes/delete") {
        std::string idStr = extractJsonField(body, "id");
        if (!idStr.empty()) g_engine.scratchpad()->deleteNote(std::stoi(idStr));
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/hardware/set") {
        std::string cpuStr = extractJsonField(body, "cpu");
        std::string ramStr = extractJsonField(body, "ram");
        if (!cpuStr.empty()) g_engine.hardware()->setCpuLimitPercent(std::stoi(cpuStr));
        if (!ramStr.empty()) g_engine.hardware()->setRamLimitMb(std::stoi(ramStr));
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/tabgroups/toggle") {
        std::string idStr = extractJsonField(body, "id");
        if (!idStr.empty()) g_engine.tabGroups()->toggleGroupCollapse(std::stoi(idStr));
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/extensions/install") {
        std::string id = extractJsonField(body, "id");
        bool ok = g_engine.extensions()->installExtension(id);
        contentType = "application/json";
        responseBody = "{\"status\": \"" + std::string(ok ? "ok" : "error") + "\"}";
    } else if (method == "POST" && path == "/api/extensions/toggle") {
        std::string id = extractJsonField(body, "id");
        std::string enStr = extractJsonField(body, "enabled");
        bool enable = (enStr == "true" || enStr == "1");
        bool ok = g_engine.extensions()->toggleExtension(id, enable);
        contentType = "application/json";
        responseBody = "{\"status\": \"" + std::string(ok ? "ok" : "error") + "\"}";
    } else if (method == "POST" && path == "/api/extensions/uninstall") {
        std::string id = extractJsonField(body, "id");
        bool ok = g_engine.extensions()->uninstallExtension(id);
        contentType = "application/json";
        responseBody = "{\"status\": \"" + std::string(ok ? "ok" : "error") + "\"}";
    } else if (method == "POST" && path == "/api/navigate") {
        std::string url = extractJsonField(body, "url");
        if (url.find("addons.mozilla.org") != std::string::npos || url == "addons" || url == "about:addons") {
            url = "https://addons.mozilla.org/firefox/";
        }
        auto navRes = g_engine.navigateActiveTab(url);
        if (url == "https://addons.mozilla.org/firefox/" || url == "about:downloads" || url == "about:performance" || url == "about:reader" || url == "about:passwords" || url == "about:devices" || url == "about:fingerprint" || url == "about:storage") {
            auto cur = g_engine.tabs()->getActiveTab();
            if (cur) {
                if (url == "https://addons.mozilla.org/firefox/") cur->title = "إضافات فايرفوكس (AMO)";
                else if (url == "about:devices") cur->title = "إدارة 200 جهاز افتراضي";
                else if (url == "about:fingerprint") cur->title = "فاحص بصمة العتاد";
                else if (url == "about:storage") cur->title = "التخزين الخارق وعزل الـ 200 متصفح";
                else if (url == "about:passwords") cur->title = "خزنة كلمات المرور";
                else if (url == "about:downloads") cur->title = "مدير التنزيلات";
                else if (url == "about:performance") cur->title = "مراقبة الأداء";
                else if (url == "about:reader") cur->title = "وضع القراءة";
                cur->currentUrl = url;
            }
            navRes.finalUrl = url;
        }

        // Log request to DevTools
        g_engine.devTools()->logNetworkRequest(navRes.finalUrl, "GET", navRes.statusCode, "document", 45000, 16.4);

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
    } else if (method == "POST" && path == "/api/ai/ask") {
        std::string query = extractJsonField(body, "query");
        std::string action = extractJsonField(body, "action");
        auto cur = g_engine.tabs()->getActiveTab();
        std::string title = cur ? cur->title : "";
        std::string content = cur ? cur->currentUrl : "";
        if (action == "summarize") {
            g_engine.ai()->summarizeContent(content, title);
        } else if (action == "explain") {
            g_engine.ai()->explainCode(content);
        } else if (action == "privacy") {
            g_engine.ai()->askQuestion("فحص الأمان والخصوصية: هل الصفحة والاتصال آمنان؟", content);
        } else {
            g_engine.ai()->askQuestion(query, content);
        }
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
    } else if (method == "POST" && path == "/api/ai/clear") {
        g_engine.ai()->clearChatHistory();
        contentType = "application/json";
        responseBody = "{\"status\": \"ok\"}";
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

    std::ostringstream resp;
    resp << "HTTP/1.1 " << responseStatus << "\r\n"
         << "Content-Type: " << contentType << "\r\n"
         << "Content-Length: " << responseBody.size() << "\r\n"
         << "Access-Control-Allow-Origin: *\r\n"
         << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
         << "Access-Control-Allow-Headers: Content-Type\r\n"
         << "Connection: close\r\n\r\n"
         << responseBody;

    std::string respStr = resp.str();
    send(clientSocket, respStr.c_str(), respStr.size(), 0);
    close(clientSocket);
}

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    g_engine.initialize();

    // Default tabs:
    // Tab 1: 200 Virtual Devices Hub
    g_engine.tabs()->createTab("about:devices", 1, "إدارة 200 جهاز افتراضي", "#22c55e", 1);
    // Tab 2: Regular Web Page to demonstrate isolated execution on
    g_engine.tabs()->createTab("https://github.com", 2, "GitHub: Free & Open Source", "#fb923c", 2);

    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Bind to 0.0.0.0
    address.sin_port = htons(port);

    if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind to port " << port << "\n";
        close(serverFd);
        return 1;
    }

    if (listen(serverFd, 10) < 0) {
        std::cerr << "Failed to listen on socket\n";
        close(serverFd);
        return 1;
    }

    std::cout << "=====================================================\n";
    std::cout << "  AtlasBrowser Firefox Quantum Server is Running!\n";
    std::cout << "  Listening on: http://0.0.0.0:" << port << "\n";
    std::cout << "  - 200 Virtual Device Profiles (Anti-Detect) Ready\n";
    std::cout << "  - Multi-Browser Live Matrix Grid Ready\n";
    std::cout << "  - WebGL Unmasked Hardware & Canvas Farbling Ready\n";
    std::cout << "  - Full Cookie & Storage Sandbox Active\n";
    std::cout << "=====================================================\n";

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
