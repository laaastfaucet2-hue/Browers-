# دليل ومشروع بناء متصفح مخصص بلغة C++ (Chromium / Firefox Fork Architecture)

أهلاً بك! يهدف هذا المستودع إلى تزويدك بالبنية التحتية البرمجية والمعمارية الكاملة لبناء **متصفح إنترنت خاص بك وبميزاتك الحصرية بلغة C++**، سواء أردت الاعتماد على نواة **Chromium** أو نواة **Firefox**.

---

## 🧭 خريطة الطرق الثلاث لبناء متصفحك الخاص

| المسار | متى تختاره؟ | متطلبات العتاد | سرعة التطوير |
| :--- | :--- | :--- | :--- |
| **1. C++ CEF (Chromium Embedded Framework)** | الخيار القياسي لـ 90% من المشاريع المستقلة والشركات (مثل Spotify و Steam). متصفح C++ خالص 100% بنواة كروميوم كاملة دون تحميل 100 جيجابايت. | حاسوب عادي (8GB RAM) | سريعة جداً (دقائق) |
| **2. Qt WebEngine (C++ / Chromium)** | إذا أردت واجهة رسومية كاملة (تبويبات، شريط عناوين، إعدادات، مفضلات) مبنية بـ C++ مع نواة Blink/V8. | حاسوب عادي (8GB RAM) | سريعة ومباشرة |
| **3. Raw Fork (Chromium / Firefox Source)** | إذا أردت تعديل شفرة النواة الأصلية مباشرة، حذف خدمات جوجل/موزيلا جذرياً، أو إطلاق مشتق مثل Brave أو LibreWolf. | جهاز خارق (32GB+ RAM و 100GB+ SSD) | تتطلب ساعات بناء طويلة |

---

## 🚀 تشغيل وتجربة نواة المتصفح التفاعلية (جاهزة في هذا المستودع الآن!)

يحتوي هذا المستودع على **محرك متصفح C++20 متكامل ومكتمل الكود** يغطي كافة الأنظمة الأساسية لأي متصفح حديث.

### 1. فحص واختبار كافة الأنظمة الفرعية:
```bash
make test
```
يقوم بفحص واختبار:
- [x] ترقية الروابط التلقائية لـ HTTPS (`Automatic HTTPS Upgrade`).
- [x] تنظيف الروابط من معلمات التتبع (`Tracking Parameter Stripping` مثل `fbclid`, `utm_*`).
- [x] محرك حظر الإعلانات والتتبع (`Domain & Regex AdBlocker`).
- [x] إدارة التبويبات والتنقل في السجل (`Tab Management & Navigation Stack`).
- [x] معالجة البروتوكولات والصفحات الداخلية (`mybrowser://newtab`, `mybrowser://settings`, `mybrowser://stats`).
- [x] جسر التواصل بين C++ وبيئة JavaScript في الويب (`C++ <-> JS Native Bridge`).
- [x] دروع حماية البصمة الرقمية ومنع تسريب IP في WebRTC (`Privacy & Fingerprint Shield`).

### 2. تشغيل المحرك التفاعلي (Interactive Shell):
```bash
make run
```
أوامر يمكنك تجربتها فوراً داخل المحرك:
- `open https://example.com?utm_source=ad&fbclid=123` *(ستلاحظ تنظيف الرابط وترقيته إلى HTTPS)*
- `open https://googleadservices.com/ad.js` *(ستلاحظ حجب الطلب فوراً بواسطة AdBlocker)*
- `open mybrowser://newtab` *(عرض صفحة البداية المخصصة)*
- `tabs` و `newtab https://duckduckgo.com` *(إدارة ألسنة التصفح)*
- `discard <tab_id>` *(تفعيل وضع توفير الذاكرة وإراحة التبويب غير النشط)*
- `adblock stats` *(عرض إحصائيات الإعلانات ومسارات التتبع المحجوبة)*
- `bridge list` و `bridge call system.getInfo` *(استدعاء دوال C++ من بيئة الجافاسكريبت)*
- `shield script` *(عرض سكربت الحماية المحقون في DOM)*

---

## 📁 هيكل المستودع ومحتوياته

```text
├── include/browser_core/         # ترويسات محرك C++20 للمتصفح المخصص
│   ├── BrowserEngine.hpp         # المنسق العام لدورة حياة المتصفح
│   ├── AdBlocker.hpp             # محرك حظر الإعلانات وقواعد EasyList
│   ├── NetworkInterceptor.hpp    # فحص واعتراض وتعديل طلبات الشبكة
│   ├── SchemeHandler.hpp         # معالج الصفحات الداخلية (mybrowser://)
│   ├── PrivacyShield.hpp         # حماية البصمة الرقمية و WebRTC
│   ├── JsBridge.hpp              # ربط دوال C++ الأصلية بصفحات الويب
│   ├── TabManager.hpp            # إدارة التبويبات وحفظ استهلاك الذاكرة
│   └── BookmarkHistoryStore.hpp  # تخزين وإدارة السجل والمفضلات
│
├── src/browser_core/             # التنفيذ البرمجي الكامل لكل نظام فرعي
│   └── main.cpp                  # واجهة التشغيل والاختبار التفاعلية
│
├── templates/
│   ├── cef_starter/              # مشروع كامل لمتصفح C++ بنواة Chromium CEF
│   │   ├── CMakeLists.txt
│   │   ├── main.cpp              # تهيئة محرك Blink و V8 والواجهة
│   │   ├── client_app.cpp        # تسجيل البروتوكولات وإعدادات الخصوصية
│   │   ├── client_handler.cpp    # اعتراض الطلبات وحظر الإعلانات بـ C++
│   │   └── custom_v8_handler.cpp # تعريف دوال C++ داخل كائن window.myBrowser
│   │
│   └── qt_webengine_starter/     # مشروع كامل لمتصفح واجهات C++ بـ Qt WebEngine
│       ├── CMakeLists.txt
│       ├── main.cpp
│       ├── browser_window.cpp    # واجهة التبويبات وشريط العناوين والتنقل
│       ├── custom_request_interceptor.cpp # فحص روابط الشبكة
│       └── custom_scheme_handler.cpp      # صفحة البداية والإعدادات
│
├── forks/
│   ├── chromium/                 # بناء Fork مباشر من كود كروميوم الأصلي
│   │   ├── args.gn               # خيارات بناء الإنتاج وحذف تتبع جوجل
│   │   ├── scripts/              # سكربتات الأتمتة (تحميل، باتشات، بناء)
│   │   └── patches/              # باتشات C++ لتغيير الهوية وحذف التيليميتري
│   │
│   └── firefox/                  # بناء Fork مباشر من كود فايرفوكس (Gecko)
│       ├── mozconfig             # ملف إعدادات بناء فايرفوكس
│       ├── scripts/              # سكربتات الأتمتة لبناء Gecko
│       └── patches/              # باتشات حذف Pocket وتتبع موزيلا
│
└── docs/                         # توثيق تقني معمق
    ├── 01_CHROMIUM_VS_FIREFOX_COMPARISON_AR.md # مقارنة شاملة لاختيار النواة
    ├── 02_HOW_MODERN_FORKS_WORK_AR.md          # كيف تعمل مشاريع Brave و LibreWolf
    ├── 03_CUSTOM_FEATURES_GUIDE_AR.md          # دليل برمجة ميزاتك الحصرية بـ C++
    └── 04_HARDWARE_AND_BUILD_REQUIREMENTS_AR.md # متطلبات الأجهزة وبدائل السحابة
```

---

## 🛠️ كيف تبدأ بتطوير ميزاتك المخصصة الآن؟

1. **إضافة قواعد حجب إعلانات جديدة:**
   افتح `include/browser_core/AdBlocker.hpp` واستخدم دالة `addDomainRule` أو `loadRulesFromEasyList`.
2. **إضافة صفحات داخلية لمتصفحك:**
   افتح `src/browser_core/SchemeHandler.cpp` وسجل رابطاً جديداً مثل `mybrowser://wallet` أو `mybrowser://downloads`.
3. **توفير واجهات برمجية جديدة لمواقعك (JS Bridge):**
   افتح `src/browser_core/BrowserEngine.cpp` واستخدم `m_jsBridge->registerFunction("myApi.doSomething", ...)` لاستدعاء كود C++ من المتصفح مباشرة.
4. **بناء تطبيق رسومي حقيقي (Desktop App):**
   انتقل إلى مجلد `templates/cef_starter/` أو `templates/qt_webengine_starter/` واتبع التعليمات في ملف `README.md` الخاص بكل منهما.

---

## 📜 الترخيص
هذا المشروع مفتوح المصدر ومتاح لتطوير متصفحات مخصصة لأغراض شخصية وتجارية.
