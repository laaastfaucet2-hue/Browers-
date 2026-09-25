# قوالب مشروع المتصفح المبني بـ CEF (Chromium Embedded Framework) بالـ C++

## ما هو CEF ولماذا يُعد الخيار الأفضل؟
**Chromium Embedded Framework (CEF)** هو المشروع القياسي عالمياً لبناء متصفحات مخصصة بلغة **C++**.
يستخدمه مشاريع عملاقة مثل:
- مشغل Spotify لسطح المكتب
- عميل متجر Steam (Valve)
- مشغلات الألعاب وتطبيقات سطح المكتب المتخصصة

### المميزات الأساسية:
1. **نواة كروميوم كاملة 100%:** محرك التصيير **Blink** ومحرك الجافاسكريبت **V8** بكل سرعته وتوافقه مع معايير الويب.
2. **بدون الحاجة لتحميل 100 جيجابايت:** تستخدم نسخ CEF الجاهزة (Pre-built Binaries) وتكتب متصفحك بلغة C++ فقط.
3. **تحكم تام في الشبكة:** فحص كل طلب، حجب الإعلانات، ترقية التشفير، حقن ترويسات أمان.
4. **جسر C++ إلى JavaScript:** إمكانية ربط أي دالة في C++ بـ DOM الصفحة ليتم استدعاؤها عبر `window.myBrowser.myFunction()`.

---

## خطوات البناء والتشغيل

### 1. تحميل حزمة CEF الجاهزة
قم بتحميل الإصدار المناسب لنظامك من موقع CEF الرسمي:
👉 [https://cef-builds.spotifycdn.com/index.html](https://cef-builds.spotifycdn.com/index.html)
(اختر "Standard Distribution").

فك الضغط وضع المجلد في مسار محدد، مثلاً:
```bash
/home/user/cef_binary/
# أو على ويندوز:
C:/cef_binary/
```

### 2. البناء عبر CMake
```bash
mkdir build && cd build
cmake -DCEF_ROOT=/path/to/cef_binary ..
make -j4
```

### 3. الملفات الرئيسية وشرحها:
- `main.cpp`: نقطة الدخول، إعدادات المتصفح، تفعيل مسار الكاش، تشغيل `CefMessageLoop`.
- `client_app.h/cpp`: ضبط أعلام سطر الأوامر (Command Line Flags) مثل منع تسريب WebRTC وتعطيل التتبع، وتسجيل البروتوكولات المخصصة `mybrowser://`.
- `client_handler.h/cpp`: معالجة النوافذ واعتراض الشبكة (`OnBeforeResourceLoad`) لتطبيق مانع الإعلانات.
- `custom_v8_handler.h/cpp`: جسر دوال C++ إلى بيئة JavaScript.
