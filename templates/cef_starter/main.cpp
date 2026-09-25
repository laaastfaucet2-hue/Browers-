/**
 * Custom Chromium Browser using CEF (Chromium Embedded Framework) in C++
 * Demonstrates:
 * 1. Initializing Chromium Blink & V8 engine
 * 2. Configuring custom User-Agent and Privacy Switches
 * 3. Creating custom browser window with custom settings
 * 4. Running the C++ Message Loop
 */

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_browser.h"
#include "client_app.h"
#include "client_handler.h"
#include <iostream>

#if defined(OS_WIN)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    CefMainArgs main_args(hInstance);
#else
int main(int argc, char* argv[]) {
    CefMainArgs main_args(argc, argv);
#endif

    // 1. Create client application instance
    CefRefPtr<ClientApp> app(new ClientApp());

    // 2. Execute early CEF process check (CEF uses multi-process model: browser, renderer, gpu)
    int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
    if (exit_code >= 0) {
        return exit_code;
    }

    // 3. Configure Chromium settings
    CefSettings settings;
    settings.no_sandbox = true; // Set to false in production with proper sandbox executable
    settings.multi_threaded_message_loop = false;
    
    // Custom browser cache directory
    CefString(&settings.cache_path).FromString("./browser_cache");
    
    // Custom User Agent to brand your browser
    CefString(&settings.user_agent).FromString("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36 AtlasBrowser/1.0");

    // 4. Initialize CEF
    if (!CefInitialize(main_args, settings, app.get(), nullptr)) {
        std::cerr << "Failed to initialize CEF!" << std::endl;
        return 1;
    }

    // 5. Setup Window info and Browser settings
    CefWindowInfo window_info;
#if defined(OS_WIN)
    window_info.SetAsPopup(NULL, "AtlasBrowser - Custom C++ Chromium Engine");
#endif

    CefBrowserSettings browser_settings;
    browser_settings.web_security = STATE_ENABLED;
    browser_settings.javascript = STATE_ENABLED;
    browser_settings.local_storage = STATE_ENABLED;

    // 6. Create client handler with our custom C++ request interceptor and adblocker
    CefRefPtr<ClientHandler> handler(new ClientHandler());

    // Start with custom internal new tab page
    std::string initial_url = "https://duckduckgo.com";
    CefBrowserHost::CreateBrowser(window_info, handler.get(), initial_url, browser_settings, nullptr, nullptr);

    // 7. Run CEF message loop until user closes window
    CefRunMessageLoop();

    // 8. Clean shutdown
    CefShutdown();

    return 0;
}
