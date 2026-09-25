#include "custom_request_interceptor.h"
#include <QUrl>
#include <QDebug>

CustomRequestInterceptor::CustomRequestInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent) {}

void CustomRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
    QUrl url = info.requestUrl();
    QString host = url.host().toLower();

    // 1. Block common ad and tracking networks
    static const QStringList adDomains = {
        "doubleclick.net", "googleadservices.com", "googlesyndication.com",
        "taboola.com", "outbrain.com", "criteo.com", "adnxs.com",
        "google-analytics.com", "analytics.google.com", "hotjar.com"
    };

    for (const QString &blocked : adDomains) {
        if (host == blocked || host.endsWith("." + blocked)) {
            qDebug() << "[Interceptor] Blocked ad/tracker:" << url.toString();
            info.block(true);
            return;
        }
    }

    // 2. Inject Privacy Headers (Do-Not-Track, Sec-GPC)
    info.setHttpHeader(QByteArray("DNT"), QByteArray("1"));
    info.setHttpHeader(QByteArray("Sec-GPC"), QByteArray("1"));
}
