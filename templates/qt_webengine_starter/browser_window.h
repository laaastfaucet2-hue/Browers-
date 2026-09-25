#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QToolBar>
#include <QAction>
#include <QProgressBar>
#include <QWebEngineView>

class BrowserWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit BrowserWindow(QWidget *parent = nullptr);
    ~BrowserWindow() override = default;

public slots:
    void addNewTab(const QUrl &url = QUrl("mybrowser://newtab"));
    void closeTab(int index);
    void currentTabChanged(int index);
    void handleUrlEntered();
    void updateUrlBar(const QUrl &url);
    void updateTitle(const QString &title);
    void updateLoadProgress(int progress);

private:
    void setupUi();
    QWebEngineView* currentWebView() const;

    QTabWidget *m_tabWidget = nullptr;
    QLineEdit *m_urlBar = nullptr;
    QProgressBar *m_progressBar = nullptr;
    QAction *m_actionBack = nullptr;
    QAction *m_actionForward = nullptr;
    QAction *m_actionReload = nullptr;
    QAction *m_actionHome = nullptr;
};
