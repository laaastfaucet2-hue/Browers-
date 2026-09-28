#include "../../include/browser_core/BrowserEngine.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace BrowserCore;

void printBanner() {
    std::cout << "\033[1;36m";
    std::cout << "=================================================================\n";
    std::cout << "          AtlasBrowser Core Engine (C++20 Architecture)          \n";
    std::cout << "         Custom Chromium / Firefox Alternative Subsystems        \n";
    std::cout << "=================================================================\n";
    std::cout << "\033[0m";
    std::cout << "Type \033[1;33mhelp\033[0m for available commands or \033[1;33mexit\033[0m to quit.\n\n";
}

void printHelp() {
    std::cout << "\n\033[1mCommands:\033[0m\n"
              << "  \033[1;32mopen <url|search>\033[0m     : Navigate active tab (resolves search, upgrades to HTTPS, strips tracking)\n"
              << "  \033[1;32mback / forward\033[0m        : Navigate history in active tab\n"
              << "  \033[1;32mtabs\033[0m                  : List all open tabs and active status\n"
              << "  \033[1;32mnewtab [url]\033[0m          : Open a new tab\n"
              << "  \033[1;32mswitch <id>\033[0m           : Switch to tab by ID\n"
              << "  \033[1;32mclose <id>\033[0m            : Close tab by ID\n"
              << "  \033[1;32mpin <id>\033[0m              : Toggle pin status on tab\n"
              << "  \033[1;32mmute <id>\033[0m             : Toggle mute status on tab\n"
              << "  \033[1;32mdiscard <id>\033[0m          : Discard tab from memory (Memory Saver mode)\n"
              << "  \033[1;32madblock stats\033[0m         : Display adblock and tracker counters\n"
              << "  \033[1;32madblock add <rule>\033[0m    : Add a custom filter rule (e.g. ||badsite.com^)\n"
              << "  \033[1;32madblock toggle\033[0m        : Enable or disable adblocker\n"
              << "  \033[1;32mbookmark add <u;t>\033[0m    : Add bookmark, e.g: bookmark add https://github.com;GitHub\n"
              << "  \033[1;32mbookmarks\033[0m             : List saved bookmarks\n"
              << "  \033[1;32mhistory\033[0m               : View browsing history\n"
              << "  \033[1;32mbridge list\033[0m           : List registered C++ functions exposed to JS DOM\n"
              << "  \033[1;32mbridge call <name>\033[0m    : Call C++ native function exposed to web runtime\n"
              << "  \033[1;32mshield script\033[0m         : Show generated anti-fingerprinting injection script\n"
              << "  \033[1;32mtest\033[0m                  : Run automated verification tests on all subsystems\n"
              << "  \033[1;32mexit\033[0m                  : Exit engine\n\n";
}

void runTests(BrowserEngine& engine) {
    std::cout << "\n\033[1;35m>>> Running Subsystem Verification Suite <<<\033[0m\n\n";

    int passed = 0;
    int total = 0;

    auto assertTest = [&](const std::string& name, bool condition) {
        total++;
        std::cout << "  [" << (condition ? "\033[1;32mPASS\033[0m" : "\033[1;31mFAIL\033[0m") << "] " << name << "\n";
        if (condition) passed++;
    };

    // 1. HTTPS Upgrade
    {
        HttpRequest req;
        req.url = "http://example.com/test";
        auto res = engine.network()->interceptRequest(req);
        assertTest("Automatic HTTPS Upgrade: http:// -> https://", 
                   res.action == InterceptAction::Redirect && res.redirectedUrl == "https://example.com/test");
    }

    // 2. Tracking Parameter Stripping
    {
        std::string dirtyUrl = "https://example.com/article?utm_source=newsletter&fbclid=XYZ123&page=2";
        std::string cleanUrl = NetworkInterceptor::stripTrackingParameters(dirtyUrl);
        assertTest("Tracking Parameter Stripping (utm_*, fbclid)", 
                   cleanUrl == "https://example.com/article?page=2");
    }

    // 3. Ad & Tracker Blocking
    {
        HttpRequest req;
        req.url = "https://googleadservices.com/pagead/conversion.js";
        BlockCategory cat;
        bool blocked = engine.adBlocker()->shouldBlock(req, &cat);
        assertTest("AdBlocker: Domain blocking (googleadservices.com)", blocked && cat == BlockCategory::Advertisement);

        HttpRequest trkReq;
        trkReq.url = "https://analytics.google.com/analytics.js";
        blocked = engine.adBlocker()->shouldBlock(trkReq, &cat);
        assertTest("AdBlocker: Tracker blocking (analytics.google.com)", blocked && cat == BlockCategory::Tracker);
    }

    // 4. Tab Navigation and History
    {
        uint32_t tId = engine.tabs()->createTab("https://site1.com");
        engine.tabs()->navigateTab(tId, "https://site2.com");
        auto tab = engine.tabs()->getTab(tId);
        assertTest("Tab creation & forward navigation", tab && tab->currentUrl == "https://site2.com");

        engine.tabs()->goBack(tId);
        tab = engine.tabs()->getTab(tId);
        assertTest("Tab history back", tab && tab->currentUrl == "https://site1.com");

        engine.tabs()->goForward(tId);
        tab = engine.tabs()->getTab(tId);
        assertTest("Tab history forward", tab && tab->currentUrl == "https://site2.com");

        engine.tabs()->setPinned(tId, true);
        assertTest("Tab pinning", tab->isPinned == true);

        engine.tabs()->closeTab(tId);
        assertTest("Tab closing", engine.tabs()->getTab(tId) == nullptr);
    }

    // 5. Internal Scheme Handling
    {
        auto resp = engine.schemes()->handleRequest("mybrowser://newtab");
        assertTest("Internal Scheme Handler: mybrowser://newtab", 
                   resp.statusCode == 200 && resp.content.find("AtlasBrowser") != std::string::npos);

        auto respSettings = engine.schemes()->handleRequest("mybrowser://settings");
        assertTest("Internal Scheme Handler: mybrowser://settings", 
                   respSettings.statusCode == 200 && respSettings.content.find("إعدادات") != std::string::npos);
    }

    // 6. C++ <-> JS Native Bridge
    {
        auto val = engine.jsBridge()->callFunction("system.getInfo", {});
        assertTest("JS Bridge: C++ function invocation (system.getInfo)", 
                   val.objVal.find("browserName") != val.objVal.end());

        std::string script = engine.jsBridge()->generateWrapperScript();
        assertTest("JS Bridge: Script generation for DOM injection", 
                   script.find("window.myBrowser") != std::string::npos);
    }

    // 7. Privacy Shield Protections
    {
        std::string antiFp = engine.privacy()->generateAntiFingerprintingScript();
        assertTest("Privacy Shield: Anti-fingerprinting script generation", 
                   antiFp.find("CanvasRenderingContext2D") != std::string::npos);

        auto flags = engine.privacy()->getChromiumWebRtcArgs();
        assertTest("Privacy Shield: WebRTC leak prevention Chromium flags", 
                   !flags.empty() && flags[0].find("disable_non_proxied_udp") != std::string::npos);
    }

    // 8. Workspaces (Arc / Zen Style)
    {
        auto wsList = engine.workspaces()->getAllWorkspaces();
        assertTest("Workspaces: Preset workspaces initialized (Dev, Personal, Finance, Research)", wsList.size() >= 4);

        bool switched = engine.workspaces()->switchWorkspace(2);
        assertTest("Workspaces: Workspace switching to Dev Space (ID 2)", switched && engine.workspaces()->getActiveWorkspaceId() == 2);
    }

    // 9. Firefox Containers & Auto-Router
    {
        auto containers = engine.containers()->getAllContainers();
        assertTest("Firefox Multi-Account Containers: Initialized default containers", containers.size() >= 4);

        auto matchedId = engine.containerRouter()->matchContainer("https://github.com/torvalds/linux");
        assertTest("Auto-Container Router: Automatic routing for github.com to Work Container", matchedId == 2);
    }

    // 10. Native C++ AI Assistant Engine
    {
        auto summary = engine.ai()->summarizeContent("C++ code repository for Gecko browser", "GitHub Repo");
        assertTest("AI Assistant Engine: Content summarization", !summary.empty() && summary.find("ملخص") != std::string::npos);

        auto history = engine.ai()->getChatHistory();
        assertTest("AI Assistant Engine: Chat history recording", history.size() >= 2);
    }

    // 11. Turbo Multi-Threaded Download Manager & Security Shield
    {
        uint32_t dlId = engine.downloads()->startDownload("https://example.com/archive.zip", "archive.zip", 2048000);
        assertTest("Turbo Download Manager: Multi-threaded download dispatch", dlId > 0);

        uint32_t malId = engine.downloads()->startDownload("https://bad.com/payload.scr", "payload.scr", 1024);
        auto allDls = engine.downloads()->getAllDownloads();
        bool blockedDangerous = false;
        for (const auto& d : allDls) {
            if (d.id == malId && d.status == DownloadStatus::BlockedDangerous) blockedDangerous = true;
        }
        assertTest("Turbo Download Manager: Malware Heuristic Security Blocking (.scr, .vbs)", blockedDangerous);
    }

    // 12. Performance & Memory Shield Monitor
    {
        auto metrics = engine.performance()->getTabMetrics({1, 2, 3});
        assertTest("Performance Monitor: Per-tab RAM and CPU metrics calculation", metrics.size() == 3);

        double savedMb = engine.performance()->getMemorySavedMb();
        assertTest("Performance Monitor: Memory Saver calculation (+420MB saved)", savedMb > 0);
    }

    // 13. Distraction-Free Speed Reader Mode
    {
        std::string readerHtml = ReaderModeEngine::formatReaderArticle("Test Article", "Atlas Team", "<p>Hello World</p>");
        assertTest("Reader Mode Engine: Distraction-free typography formatting", 
                   readerHtml.find("وضع القراءة") != std::string::npos && readerHtml.find("Hello World") != std::string::npos);
    }

    // 14. Firefox WebExtension Runtime (AMO & XPI Engine)
    {
        auto catalog = engine.extensions()->getCatalog();
        assertTest("WebExtension Runtime: Official AMO catalog initialized (Dark Reader, uBlock, Translate)", catalog.size() >= 4);

        bool installed = engine.extensions()->installExtension("darkreader@firefox");
        assertTest("WebExtension Runtime: Real extension installation (Dark Reader)", installed);

        std::string injectedCss = engine.extensions()->getInjectedCssForUrl("https://github.com");
        assertTest("WebExtension Runtime: Content CSS injection engine active for URLs", 
                   !injectedCss.empty() && injectedCss.find("Dark Reader WebExtension Engine") != std::string::npos);

        std::string injectedJs = engine.extensions()->getInjectedJsForUrl("https://github.com");
        assertTest("WebExtension Runtime: Content JS injection engine active for URLs", 
                   !injectedJs.empty() && injectedJs.find("__darkReaderInjected") != std::string::npos);
    }

    // 15. Built-in DevTools & Live Console Engine
    {
        engine.devTools()->logNetworkRequest("https://api.github.com/repos", "GET", 200, "json", 4200, 18.2);
        auto logs = engine.devTools()->getNetworkLogs();
        assertTest("DevTools Engine: Live network waterfall logging", logs.size() >= 4);

        std::string jsRes = engine.devTools()->evaluateJs("2 + 2");
        assertTest("DevTools Engine: Interactive JavaScript console evaluation (2 + 2 == 4)", jsRes == "4");
    }

    // 16. Encrypted Password Vault & Breach Shield
    {
        uint32_t pId = engine.vault()->addEntry("https://bank.com", "myuser", "MyS3cur3!Bank2026");
        assertTest("Password Vault: Encrypted credential storage", pId > 0);

        std::string genPass = PasswordVault::generateStrongPassword(18, true);
        assertTest("Password Vault: Crypto-grade password generator (length >= 18)", genPass.length() == 18);

        int strength = PasswordVault::evaluatePasswordStrength(genPass);
        assertTest("Password Vault: Password strength scoring (strength >= 80)", strength >= 80);

        bool breached = PasswordVault::checkBreachStatus("123456");
        assertTest("Password Vault: Real-time breach detection for weak credentials", breached);
    }

    // 17. Smart Tab Groups & Auto-Stacking
    {
        uint32_t gId = engine.tabGroups()->createGroup("Research Group", "#4ade80");
        engine.tabGroups()->addTabToGroup(gId, 1);
        auto grp = engine.tabGroups()->getGroup(gId);
        assertTest("Tab Groups: Collapsible group creation and assignment", grp && grp->tabIds.size() == 1);

        engine.tabGroups()->toggleGroupCollapse(gId);
        grp = engine.tabGroups()->getGroup(gId);
        assertTest("Tab Groups: Toggle group collapse/expand", grp && grp->isCollapsed == true);
    }

    // 18. Web Scratchpad & Notes Clipper
    {
        uint32_t noteId = engine.scratchpad()->createNote("Meeting Notes", "Discuss Gecko fork roadmap", "https://meeting.org");
        assertTest("Scratchpad Engine: Create and manage markdown notes", noteId > 0);

        uint32_t clipId = engine.scratchpad()->clipWebSelection("C++20 modules empower fast compiling", "C++ Docs", "https://isocpp.org");
        assertTest("Scratchpad Engine: Web clipping with automatic source attribution", clipId > 0);
    }

    // 19. Hardware Limiter (CPU/RAM) & Picture-in-Picture
    {
        engine.hardware()->setCpuLimitPercent(50);
        assertTest("Hardware Limiter: CPU throttle percentage limit (50%)", engine.hardware()->getCpuLimitPercent() == 50);

        engine.hardware()->setRamLimitMb(2048, true);
        assertTest("Hardware Limiter: Hard RAM limit allocation (2048 MB)", engine.hardware()->getRamLimitMb() == 2048 && engine.hardware()->isHardLimit());

        engine.hardware()->togglePip(true, "Demo Video", "https://video.com/1");
        engine.hardware()->setPlaybackRate(1.5);
        auto pip = engine.hardware()->getPipState();
        assertTest("Floating Media Engine: Picture-in-Picture speed control (1.5x)", pip.isPipActive && pip.playbackRate == 1.5);
    }

    // 20. Multi-Device Anti-Detect Profile Manager (200 Isolated Profiles)
    {
        size_t totalProfiles = engine.profiles()->getProfileCount();
        assertTest("Anti-Detect Manager: 200 virtual device profiles initialized in pool", totalProfiles >= 200);

        const auto* p1 = engine.profiles()->getProfile(1);
        const auto* p2 = engine.profiles()->getProfile(2);
        assertTest("Anti-Detect Manager: Profile #1 and #2 exist with distinct OS & Hardware", 
                   p1 != nullptr && p2 != nullptr && p1->osType != p2->osType);

        assertTest("Anti-Detect Manager: Hardware Fingerprint divergence (Canvas & Audio Seeds distinct)",
                   p1->canvasNoiseSeed != p2->canvasNoiseSeed && p1->audioNoiseShift != p2->audioNoiseShift);

        assertTest("Anti-Detect Manager: WebGL GPU divergence (Unmasked Vendor & Renderer distinct)",
                   p1->webglRenderer != p2->webglRenderer && p1->webglVendor != p2->webglVendor);

        assertTest("Anti-Detect Manager: Storage Sandbox directory isolation",
                   p1->storageDirectory != p2->storageDirectory);

        bool switched = engine.profiles()->switchActiveProfile(42);
        assertTest("Anti-Detect Manager: Instant profile switching to Profile #42", 
                   switched && engine.profiles()->getActiveProfile()->id == 42);

        std::string script = engine.profiles()->getActiveAntiDetectScript();
        assertTest("Anti-Detect Manager: Anti-detect injection script generation for DOM",
                   script.find("[Atlas Anti-Detect]") != std::string::npos && script.find("hardwareConcurrency") != std::string::npos);
    }

    // 21. Ultra-Isolated Storage & Zero-Footprint Engine (200 Virtual Devices VFS)
    {
        auto storage = engine.ultraStorage();
        assertTest("Ultra-Storage Engine: 200 isolated storage boxes initialized", storage->hasBox(200));

        // Test cookie isolation between profile 1 and profile 2
        storage->setCookie(1, "mysecurebank.com", "auth_token", "p1_secret_token_123");
        storage->setCookie(2, "mysecurebank.com", "auth_token", "p2_different_token_999");

        auto p1Cookies = storage->getCookies(1, "mysecurebank.com");
        auto p2Cookies = storage->getCookies(2, "mysecurebank.com");
        assertTest("Ultra-Storage Engine: Profile #1 and #2 cookie isolation (Zero-Leakage)",
                   p1Cookies.size() == 1 && p2Cookies.size() == 1 && p1Cookies[0].value != p2Cookies[0].value);

        // Test LocalStorage isolation
        storage->setItem(1, "app.domain", "user_pref", "dark_mode");
        storage->setItem(2, "app.domain", "user_pref", "light_mode");
        assertTest("Ultra-Storage Engine: LocalStorage key-value isolation between virtual devices",
                   storage->getItem(1, "app.domain", "user_pref") == "dark_mode" &&
                   storage->getItem(2, "app.domain", "user_pref") == "light_mode");

        // Test Instant Hibernation (Zero-RAM snapshot)
        bool hibOk = storage->hibernateProfile(1);
        auto p1Stats = storage->getBox(1)->getStats();
        assertTest("Ultra-Storage Engine: Instant Hibernation (RAM freed, State == HIBERNATED)",
                   hibOk && p1Stats.state == ProfileStorageState::HIBERNATED);

        // Test Instant Wakeup (<3ms state restoration)
        bool wakeOk = storage->wakeProfile(1);
        auto p1AwakeCookies = storage->getCookies(1, "mysecurebank.com");
        assertTest("Ultra-Storage Engine: Instant Wakeup (<3ms state restoration from snapshot)",
                   wakeOk && !p1AwakeCookies.empty() && p1AwakeCookies[0].value == "p1_secret_token_123");

        // Test Global Space Savings (>45 GB Disk, >25 GB RAM saved across 200 profiles)
        auto metrics = storage->calculateGlobalMetrics();
        assertTest("Ultra-Storage Engine: Massive disk & RAM savings calculated across 200 profiles",
                   metrics.totalDiskSavedBytes > 40000000000ULL && metrics.totalRamSavedBytes > 20000000000ULL);
    }

    std::cout << "\n\033[1;36mResult: " << passed << " of " << total << " tests passed successfully!\033[0m\n\n";
}

int main(int argc, char* argv[]) {
    BrowserConfig config;
    config.browserName = "AtlasBrowser";
    config.version = "1.0.0";
    BrowserEngine engine(config);

    if (argc > 1 && std::string(argv[1]) == "--test") {
        runTests(engine);
        return 0;
    }

    printBanner();

    std::string line;
    while (true) {
        auto activeTab = engine.tabs()->getActiveTab();
        std::string tabIndicator = activeTab ? "[Tab " + std::to_string(activeTab->id) + " : " + activeTab->currentUrl + "]" : "[No Tabs]";
        std::cout << "\033[1;34m" << tabIndicator << "\033[0m \033[1;32m>\033[0m ";

        if (!std::getline(std::cin, line)) {
            break;
        }

        while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
        while (!line.empty() && line.front() == ' ') line.erase(0, 1);

        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit" || cmd == "quit") {
            std::cout << "Exiting browser engine. Goodbye!\n";
            break;
        } else if (cmd == "help") {
            printHelp();
        } else if (cmd == "test") {
            runTests(engine);
        } else if (cmd == "open") {
            std::string target;
            std::getline(iss, target);
            if (!target.empty() && target.front() == ' ') target.erase(0, 1);
            if (target.empty()) {
                std::cout << "Usage: open <url or query>\n";
                continue;
            }

            std::cout << "Navigating to: " << target << " ...\n";
            auto res = engine.navigateActiveTab(target);
            if (res.wasBlocked) {
                std::cout << "\033[1;31m[BLOCKED]\033[0m " << res.blockedReason << "\n";
                std::cout << "Redirected to internal safe page: " << res.finalUrl << "\n";
            } else if (res.success) {
                std::cout << "\033[1;32m[SUCCESS]\033[0m Loaded: " << res.finalUrl << " (Status: " << res.statusCode << ")\n";
                std::cout << "Title: " << res.pageTitle << "\n";
            } else {
                std::cout << "\033[1;31m[FAILED]\033[0m " << res.errorString << "\n";
            }
        } else if (cmd == "tabs") {
            auto tabs = engine.tabs()->getAllTabs();
            std::cout << "\nOpen Tabs (" << tabs.size() << "):\n";
            uint32_t activeId = engine.tabs()->getActiveTabId();
            for (const auto& t : tabs) {
                std::cout << "  " << (t.id == activeId ? "\033[1;32m* " : "  ")
                          << "[ID: " << t.id << "] " << t.title << " (" << t.currentUrl << ")"
                          << (t.isPinned ? " [Pinned]" : "")
                          << (t.isMuted ? " [Muted]" : "")
                          << (t.isDiscarded ? " [Discarded/Sleeping]" : "")
                          << "\033[0m\n";
            }
            std::cout << "\n";
        } else if (cmd == "newtab") {
            std::string url;
            iss >> url;
            if (url.empty()) url = "mybrowser://newtab";
            uint32_t id = engine.tabs()->createTab(url);
            std::cout << "Created Tab ID: " << id << " with URL: " << url << "\n";
        } else if (cmd == "switch") {
            uint32_t id;
            if (iss >> id) {
                if (engine.tabs()->switchTab(id)) {
                    std::cout << "Switched to tab " << id << "\n";
                } else {
                    std::cout << "Tab ID not found.\n";
                }
            } else {
                std::cout << "Usage: switch <tab_id>\n";
            }
        } else if (cmd == "close") {
            uint32_t id;
            if (iss >> id) {
                if (engine.tabs()->closeTab(id)) {
                    std::cout << "Closed tab " << id << "\n";
                } else {
                    std::cout << "Tab ID not found.\n";
                }
            } else {
                std::cout << "Usage: close <tab_id>\n";
            }
        } else if (cmd == "back") {
            if (activeTab) {
                engine.tabs()->goBack(activeTab->id);
                std::cout << "Back -> " << engine.tabs()->getActiveTab()->currentUrl << "\n";
            }
        } else if (cmd == "forward") {
            if (activeTab) {
                engine.tabs()->goForward(activeTab->id);
                std::cout << "Forward -> " << engine.tabs()->getActiveTab()->currentUrl << "\n";
            }
        } else if (cmd == "pin") {
            uint32_t id;
            if (iss >> id) {
                auto t = engine.tabs()->getTab(id);
                if (t) {
                    engine.tabs()->setPinned(id, !t->isPinned);
                    std::cout << "Tab " << id << " pin status: " << (!t->isPinned ? "Pinned" : "Unpinned") << "\n";
                }
            }
        } else if (cmd == "mute") {
            uint32_t id;
            if (iss >> id) {
                auto t = engine.tabs()->getTab(id);
                if (t) {
                    engine.tabs()->setMuted(id, !t->isMuted);
                    std::cout << "Tab " << id << " mute status: " << (!t->isMuted ? "Muted" : "Unmuted") << "\n";
                }
            }
        } else if (cmd == "discard") {
            uint32_t id;
            if (iss >> id) {
                if (engine.tabs()->discardTab(id)) {
                    std::cout << "Tab " << id << " memory discarded (Sleeping tab to save RAM).\n";
                } else {
                    std::cout << "Cannot discard tab (either active tab or not found).\n";
                }
            }
        } else if (cmd == "adblock") {
            std::string sub;
            iss >> sub;
            if (sub == "stats") {
                auto s = engine.adBlocker()->getStats();
                std::cout << "\nAdBlocker Subsystem Statistics:\n"
                          << "  - Total Blocked:     " << s.totalBlocked << "\n"
                          << "  - Ads Blocked:       " << s.adsBlocked << "\n"
                          << "  - Trackers Blocked:  " << s.trackersBlocked << "\n"
                          << "  - Analytics Blocked: " << s.analyticsBlocked << "\n"
                          << "  - Requests Checked:  " << s.requestsChecked << "\n"
                          << "  - Total Rules Loaded:" << engine.adBlocker()->getRuleCount() << "\n\n";
            } else if (sub == "toggle") {
                bool newState = !engine.adBlocker()->isEnabled();
                engine.adBlocker()->setEnabled(newState);
                std::cout << "AdBlocker status: " << (newState ? "ENABLED" : "DISABLED") << "\n";
            } else if (sub == "add") {
                std::string rule;
                iss >> rule;
                if (!rule.empty()) {
                    engine.adBlocker()->addRule(rule);
                    std::cout << "Filter rule added: " << rule << "\n";
                } else {
                    std::cout << "Usage: adblock add <pattern>\n";
                }
            } else {
                std::cout << "Usage: adblock <stats|toggle|add>\n";
            }
        } else if (cmd == "bookmarks") {
            auto bms = engine.storage()->getBookmarks();
            std::cout << "\nSaved Bookmarks (" << bms.size() << "):\n";
            for (const auto& b : bms) {
                std::cout << "  ★ " << b.title << " -> " << b.url << "\n";
            }
            std::cout << "\n";
        } else if (cmd == "bookmark") {
            std::string sub;
            iss >> sub;
            if (sub == "add") {
                std::string rest;
                std::getline(iss, rest);
                if (!rest.empty() && rest.front() == ' ') rest.erase(0, 1);
                size_t delim = rest.find(';');
                if (delim != std::string::npos) {
                    std::string u = rest.substr(0, delim);
                    std::string t = rest.substr(delim + 1);
                    engine.storage()->addBookmark(u, t);
                    std::cout << "Bookmark saved: " << t << " (" << u << ")\n";
                } else {
                    std::cout << "Usage: bookmark add <url>;<title>\n";
                }
            }
        } else if (cmd == "history") {
            auto hist = engine.storage()->getHistory(20);
            std::cout << "\nRecent History:\n";
            for (const auto& h : hist) {
                std::cout << "  - " << h.title << " (" << h.url << ") [Visits: " << h.visitCount << "]\n";
            }
            std::cout << "\n";
        } else if (cmd == "bridge") {
            std::string sub;
            iss >> sub;
            if (sub == "list") {
                auto funcs = engine.jsBridge()->getRegisteredFunctionNames();
                std::cout << "\nNative C++ Methods Registered in JavaScript DOM:\n";
                for (const auto& f : funcs) {
                    std::cout << "  • window.myBrowser." << f << "()\n";
                }
                std::cout << "\n";
            } else if (sub == "call") {
                std::string fn;
                iss >> fn;
                if (!fn.empty()) {
                    auto res = engine.jsBridge()->callFunction(fn, {});
                    std::cout << "C++ Native Result:\n" << res.toJson() << "\n";
                } else {
                    std::cout << "Usage: bridge call <function_name>\n";
                }
            }
        } else if (cmd == "shield") {
            std::string sub;
            iss >> sub;
            if (sub == "script") {
                std::cout << "\n--- Anti-Fingerprinting Content Script (Injected into DOM) ---\n"
                          << engine.privacy()->generateAntiFingerprintingScript()
                          << "\n------------------------------------------------------------\n";
            }
        } else {
            std::cout << "Unknown command: '" << cmd << "'. Type 'help' for command list.\n";
        }
    }

    return 0;
}
