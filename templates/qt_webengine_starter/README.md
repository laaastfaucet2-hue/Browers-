# قوالب متصفح C++ باستخدام Qt WebEngine (Chromium Core)

## ما هو Qt WebEngine؟
**Qt WebEngine** يوفر نواة متصفح **Chromium (محرك Blink لمحاذاة صفحات الويب + محرك V8 للجافاسكريبت)** مدمجة بشكل مباشر وأنيق مع مكتبة الواجهات **Qt C++**.

### لماذا هو الخيار الأسرع لبناء متصفح C++ كامل؟
1. **واجهة رسومية أصلية بالكامل بـ C++:** التبويبات (Tabs)، شريط العنوان (URL bar)، أزرار التنقل، شريط التحميل، والمفضلات كلها عناصر C++ حقيقية.
2. **محرك كروميوم في الخلفية:** تشغيل كافة مواقع الويب الحديثة (YouTube, WebAssembly, WebGL, HTML5, CSS Grid) بدقة وسرعة كروميوم 100%.
3. **اعتراض الروابط والشبكة:** `QWebEngineUrlRequestInterceptor` يتيح لك حظر الإعلانات وتعقب المستخدمين بدوال C++ بسيطة وسريعة جداً.
4. **بروتوكولات داخلية مخصصة:** `QWebEngineUrlSchemeHandler` يتيح لك بناء صفحات داخلية مثل `mybrowser://newtab` و `mybrowser://settings`.

---

## كيفية التثبيت والبناء

### على نظام Ubuntu / Debian:
```bash
sudo apt update
sudo apt install qt6-base-dev qt6-webengine-dev cmake build-essential
# أو لـ Qt5:
# sudo apt install qtbase5-dev libqt5webengine5 libqt5webengine-dev
```

### البناء:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./custom_qt_browser
```

### على Windows:
قم بتثبيت Qt عبر Qt Online Installer مع تحديد مكون `Qt WebEngine` و `MSVC 2022 64-bit`، ثم افتح المشروع في Qt Creator واضغط `Run`.
