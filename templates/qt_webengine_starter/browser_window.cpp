#include "browser_window.h"
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeySequence>

BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUi();
    addNewTab(QUrl("mybrowser://newtab"));
}

void BrowserWindow::setupUi() {
    // Toolbar setup
    auto *toolbar = addToolBar("Navigation");
    toolbar->setMovable(false);

    m_actionBack = toolbar->addAction("←");
    m_actionForward = toolbar->addAction("→");
    m_actionReload = toolbar->addAction("⟳");
    m_actionHome = toolbar->addAction("🏠");

    m_urlBar = new QLineEdit(this);
    m_urlBar->setPlaceholderText("Enter URL or search query...");
    m_urlBar->setClearButtonEnabled(true);
    toolbar->addWidget(m_urlBar);

    auto *actionNewTab = toolbar->addAction("+");
    actionNewTab->setToolTip("Open New Tab");

    // Progress bar for page loads
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMaximumHeight(3);
    m_progressBar->setTextVisible(false);
    m_progressBar->hide();

    // Tab widget setup
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    auto *centralWidget = new QWidget(this);
    auto *vbox = new QVBoxLayout(centralWidget);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    vbox->addWidget(m_progressBar);
    vbox->addWidget(m_tabWidget);
    setCentralWidget(centralWidget);

    // Connections
    connect(m_actionBack, &QAction::triggered, [this]() {
        if (auto *view = currentWebView()) view->back();
    });
    connect(m_actionForward, &QAction::triggered, [this]() {
        if (auto *view = currentWebView()) view->forward();
    });
    connect(m_actionReload, &QAction::triggered, [this]() {
        if (auto *view = currentWebView()) view->reload();
    });
    connect(m_actionHome, &QAction::triggered, [this]() {
        if (auto *view = currentWebView()) view->setUrl(QUrl("mybrowser://newtab"));
    });
    connect(actionNewTab, &QAction::triggered, [this]() {
        addNewTab(QUrl("mybrowser://newtab"));
    });
    connect(m_urlBar, &QLineEdit::returnPressed, this, &BrowserWindow::handleUrlEntered);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &BrowserWindow::closeTab);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &BrowserWindow::currentTabChanged);
}

void BrowserWindow::addNewTab(const QUrl &url) {
    auto *webView = new QWebEngineView(this);
    webView->setUrl(url);

    int index = m_tabWidget->addTab(webView, "Loading...");
    m_tabWidget->setCurrentIndex(index);

    connect(webView, &QWebEngineView::urlChanged, this, &BrowserWindow::updateUrlBar);
    connect(webView, &QWebEngineView::titleChanged, this, &BrowserWindow::updateTitle);
    connect(webView, &QWebEngineView::loadProgress, this, &BrowserWindow::updateLoadProgress);
    connect(webView, &QWebEngineView::loadFinished, [this](bool) {
        m_progressBar->hide();
    });
}

void BrowserWindow::closeTab(int index) {
    if (m_tabWidget->count() > 1) {
        auto *view = qobject_cast<QWebEngineView*>(m_tabWidget->widget(index));
        m_tabWidget->removeTab(index);
        delete view;
    } else {
        // If last tab closed, open fresh new tab
        if (auto *view = currentWebView()) {
            view->setUrl(QUrl("mybrowser://newtab"));
        }
    }
}

void BrowserWindow::currentTabChanged(int index) {
    if (auto *view = currentWebView()) {
        m_urlBar->setText(view->url().toString());
        setWindowTitle(view->title().isEmpty() ? "AtlasBrowser" : view->title() + " - AtlasBrowser");
    }
}

void BrowserWindow::handleUrlEntered() {
    QString input = m_urlBar->text().trimmed();
    if (input.isEmpty()) return;

    QUrl url;
    if (input.startsWith("mybrowser://") || input.startsWith("http://") || input.startsWith("https://")) {
        url = QUrl(input);
    } else if (input.contains('.') && !input.contains(' ')) {
        url = QUrl("https://" + input);
    } else {
        url = QUrl("https://duckduckgo.com/?q=" + QUrl::toPercentEncoding(input));
    }

    if (auto *view = currentWebView()) {
        view->setUrl(url);
    }
}

void BrowserWindow::updateUrlBar(const QUrl &url) {
    if (sender() == currentWebView()) {
        m_urlBar->setText(url.toString());
    }
}

void BrowserWindow::updateTitle(const QString &title) {
    auto *view = qobject_cast<QWebEngineView*>(sender());
    if (!view) return;
    int idx = m_tabWidget->indexOf(view);
    if (idx != -1) {
        m_tabWidget->setTabText(idx, title.isEmpty() ? "New Tab" : title.left(20));
    }
    if (view == currentWebView()) {
        setWindowTitle(title + " - AtlasBrowser");
    }
}

void BrowserWindow::updateLoadProgress(int progress) {
    if (sender() == currentWebView()) {
        m_progressBar->show();
        m_progressBar->setValue(progress);
    }
}

QWebEngineView* BrowserWindow::currentWebView() const {
    return qobject_cast<QWebEngineView*>(m_tabWidget->currentWidget());
}
