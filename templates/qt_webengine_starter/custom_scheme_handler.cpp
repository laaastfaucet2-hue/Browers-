#include "custom_scheme_handler.h"
#include <QWebEngineUrlRequestJob>
#include <QBuffer>
#include <QUrl>

CustomSchemeHandler::CustomSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent) {}

void CustomSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request) {
    QUrl url = request->requestUrl();
    QString path = url.host(); // In mybrowser://newtab, 'newtab' is the host

    QByteArray html;
    if (path == "newtab" || path.isEmpty()) {
        html = R"HTML(
            <!DOCTYPE html>
            <html dir="rtl" lang="ar">
            <head>
                <meta charset="utf-8">
                <title>صفحة البداية - AtlasBrowser</title>
                <style>
                    body { font-family: sans-serif; background: #0f172a; color: #fff; text-align: center; padding-top: 15vh; }
                    h1 { font-size: 3rem; color: #38bdf8; }
                    p { color: #94a3b8; font-size: 1.2rem; }
                    .search { margin-top: 30px; }
                    input { width: 500px; padding: 14px 20px; font-size: 1.1rem; border-radius: 25px; border: 2px solid #334155; background: #1e293b; color: #fff; }
                </style>
            </head>
            <body>
                <h1>AtlasBrowser</h1>
                <p>متصفح مخصص مبني بنواة Chromium و C++</p>
                <form class="search" action="https://duckduckgo.com" method="GET">
                    <input type="text" name="q" placeholder="ابحث في الإنترنت أو أدخل عنوان URL..." autofocus>
                </form>
            </body>
            </html>
        )HTML";
    } else {
        html = "<html><body><h1>AtlasBrowser Internal Page: " + path.toUtf8() + "</h1></body></html>";
    }

    auto *buffer = new QBuffer(request);
    buffer->setData(html);
    buffer->open(QIODevice::ReadOnly);

    request->reply(QByteArray("text/html; charset=utf-8"), buffer);
}
