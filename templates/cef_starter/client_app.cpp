#include "client_app.h"
#include "custom_v8_handler.h"
#include "include/cef_command_line.h"
#include "include/cef_scheme.h"
#include <iostream>

ClientApp::ClientApp() {}

void ClientApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) {
    if (process_type.empty()) {
        // Main browser process command-line tweaks

        // 1. Prevent WebRTC local & public IP leaks
        command_line->AppendSwitchWithValue("webrtc-ip-handling-policy", "disable_non_proxied_udp");
        command_line->AppendSwitch("enforce-webrtc-ip-permission-check");

        // 2. Disable Google telemetry / metrics phone-home
        command_line->AppendSwitch("disable-metrics");
        command_line->AppendSwitch("disable-metrics-reporting");
        command_line->AppendSwitch("disable-background-networking");
        command_line->AppendSwitch("disable-default-apps");
        command_line->AppendSwitch("disable-sync");

        // 3. Performance & Hardware acceleration
        command_line->AppendSwitch("enable-gpu-rasterization");
        command_line->AppendSwitch("enable-zero-copy");
    }
}

void ClientApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
    // Register custom protocol "mybrowser://" as standard and secure
    int options = CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE | CEF_SCHEME_OPTION_CORS_ENABLED;
    registrar->AddCustomScheme("mybrowser", options);
}

void ClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) {
    // Inject custom C++ APIs into window.myBrowser for web pages
    CefRefPtr<CefV8Value> global = context->GetGlobal();
    CefRefPtr<CefV8Value> myBrowserObj = CefV8Value::CreateObject(nullptr, nullptr);

    CefRefPtr<CustomV8Handler> handler = new CustomV8Handler();

    // Register native C++ functions callable from JS
    CefRefPtr<CefV8Value> funcGetVersion = CefV8Value::CreateFunction("getVersion", handler);
    CefRefPtr<CefV8Value> funcGetStats = CefV8Value::CreateFunction("getAdblockStats", handler);
    CefRefPtr<CefV8Value> funcToggleShield = CefV8Value::CreateFunction("togglePrivacyShield", handler);

    myBrowserObj->SetValue("getVersion", funcGetVersion, V8_PROPERTY_ATTRIBUTE_READONLY);
    myBrowserObj->SetValue("getAdblockStats", funcGetStats, V8_PROPERTY_ATTRIBUTE_READONLY);
    myBrowserObj->SetValue("togglePrivacyShield", funcToggleShield, V8_PROPERTY_ATTRIBUTE_READONLY);

    global->SetValue("myBrowser", myBrowserObj, V8_PROPERTY_ATTRIBUTE_READONLY);
}
