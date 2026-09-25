#pragma once

#include <QWebEngineUrlSchemeHandler>

class CustomSchemeHandler : public QWebEngineUrlSchemeHandler {
    Q_OBJECT

public:
    explicit CustomSchemeHandler(QObject *parent = nullptr);
    void requestStarted(QWebEngineUrlRequestJob *request) override;
};
