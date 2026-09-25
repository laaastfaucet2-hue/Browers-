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
