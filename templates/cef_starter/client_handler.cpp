#include "client_handler.h"
#include "include/cef_app.h"
#include <iostream>
#include <string>

ClientHandler::ClientHandler() {}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    if (!m_browser) {
        m_browser = browser;
    }
    m_browserCount++;
}

bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) {
    return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    m_browserCount--;
    if (m_browserCount == 0) {
        m_browser = nullptr;
        CefQuitMessageLoop();
    }
}

CefResourceRequestHandler::ReturnValue ClientHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback) {

    std::string url = request->GetURL().ToString();

    // 1. AdBlocking & Tracker filtering in C++
    const std::vector<std::string> blockedKeywords = {
        "doubleclick.net", "googleadservices.com", "googlesyndication.com",
        "taboola.com", "outbrain.com", "google-analytics.com", "telemetry"
    };

    for (const auto& kw : blockedKeywords) {
        if (url.find(kw) != std::string::npos) {
            std::cout << "[CEF Interceptor] Blocked ad/tracker: " << url << std::endl;
            return RV_CANCEL; // Cancels and blocks request!
        }
    }

    // 2. HTTPS Upgrade
    if (url.rfind("http://", 0) == 0) {
        std::string httpsUrl = "https://" + url.substr(7);
        request->SetURL(httpsUrl);
        std::cout << "[CEF Interceptor] Upgraded to HTTPS: " << httpsUrl << std::endl;
    }

    // 3. Inject Privacy Headers (DNT, Global Privacy Control)
    CefRequest::HeaderMap headers;
    request->GetHeaderMap(headers);
    headers.insert(std::make_pair("DNT", "1"));
    headers.insert(std::make_pair("Sec-GPC", "1"));
    request->SetHeaderMap(headers);

    return RV_CONTINUE;
}

void ClientHandler::OnResourceLoadComplete(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefResponse> response,
    URLRequestStatus status,
    int64_t received_content_length) {
    // Log or analyze network performance
}
