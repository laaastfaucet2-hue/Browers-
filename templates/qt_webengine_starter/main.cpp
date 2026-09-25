/**
 * Custom Browser built with Qt WebEngine (Chromium Blink/V8 Engine) in C++
 */

#include <QApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include "browser_window.h"
#include "custom_request_interceptor.h"
#include "custom_scheme_handler.h"

int main(int argc, char* argv[]) {
    // 1. Enable Chromium High-DPI scaling
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName("AtlasBrowser");
    app.setApplicationVersion("1.0.0");

    // 2. Configure Chromium Profile & Global Settings
    auto* defaultProfile = QWebEngineProfile::defaultProfile();
    defaultProfile->setHttpUserAgent("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36 AtlasBrowser/1.0");

    // 3. Attach our Custom C++ Request Interceptor (AdBlocker & Privacy Shield)
    auto* interceptor = new CustomRequestInterceptor(&app);
    defaultProfile->setUrlRequestInterceptor(interceptor);

    // 4. Attach Custom Scheme Handler (mybrowser://newtab, mybrowser://settings)
    auto* schemeHandler = new CustomSchemeHandler(&app);
    defaultProfile->installUrlSchemeHandler(QByteArray("mybrowser"), schemeHandler);

    // 5. Configure WebEngine security & feature switches
    auto* settings = defaultProfile->settings();
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, false); // Disable insecure plugins
    settings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);

    // 6. Launch Browser Main Window
    BrowserWindow window;
    window.resize(1280, 800);
    window.show();

    return app.exec();
}
