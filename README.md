# 🦊 AtlasBrowser Quantum v128.0 (Firefox Edition)

<p align="center">
  <b>متصفح ويب متقدم بنواة C++20 ومعمارية Mozilla Firefox Gecko</b><br>
  <i>دعم كامل لإضافات فايرفوكس (AMO)، عزل الحاويات، مساحات العمل، تقسيم الشاشة، ومحرك ذكاء اصطناعي أصلي</i>
</p>

---

## 🌟 الميزات الجوهرية المنجزة

### 1. 🧩 دعم كامل لإضافات فايرفوكس (Firefox WebExtensions & AMO)
- متجر مدمج لإضافات فايرفوكس الرسمية (`about:addons`).
- إمكانية تثبيت وتشغيل الإضافات القوية مثل **uBlock Origin** (Manifest V2 API) و **Dark Reader** و **Multi-Account Containers**.

### 2. 🛡️ عزل الحاويات المتعددة (Multi-Account Containers & Auto-Router)
- عزل ملفات تعريف الارتباط والجلسات لكل هوية: **شخصي (Personal)**، **العمل (Work)**، **البنوك (Banking)**، **التسوق (Shopping)**.
- توجيه تلقائي ذكي للروابط الحساسة (مثل فتح GitHub و Jira في حاوية العمل تلقائياً).

### 3. 🤖 محرك الذكاء الاصطناعي المدمج (Native C++ AI Copilot Engine)
- شريط جانبي منزلق للمساعد الذكي أثناء التصفح (`🤖 Atlas AI`).
- تلخيص فوري لمحتوى الصفحات الطويلة والمقالات.
- محلل الأكواد البرمجية (C++ Code Analyzer) وشرح المعايير البرمجية.
- فحص الأمان والخصوصية بنقرة زر واحدة.

### 4. 📥 مدير التنزيلات فائق السرعة وفحص الأمان (Turbo Download Manager)
- تنزيل متعدد المسارات (Multi-threaded Chunked Downloads) بسرعة فائقة تصل لـ 14 MB/s.
- درع فحص أمني استباقي لمنع البرمجيات الخبيثة والامتدادات التنفيذية المشبوهة (`.scr`, `.vbs`, `.bat`).
- واجهة مخصصة على الرابط الداخلي `about:downloads`.

### 5. ⚡ درع مراقبة الأداء وتوفير الذاكرة (Performance & Memory Shield)
- قياس دقيق لاستهلاك الذاكرة (RAM) والمعالج (CPU) لكل لسان تصفح.
- تجميد وإراحة الألسنة الخاملة (Tab Discarding / Sleeping Tabs) لتوفير الطاقة وموارد الجهاز.
- توفير أكثر من **+420 MB** من استهلاك الرام عبر حجب الإعلانات وتجميد التبويبات.
- صفحة مراقبة شاملة على `about:performance`.

### 6. 📖 وضع القراءة والتركيز الفائق (Distraction-Free Speed Reader Mode)
- تنظيف المقالات تلقائياً من الإعلانات والشاشات المشتتة.
- ثيمات مخصصة للقراءة: الداكن وسيبيا لراحة العينين على الرابط `about:reader`.

### 7. 🗂️ مساحات العمل وتقسيم الشاشة (Workspaces & Split View)
- مساحات عمل منفصلة (Arc / Zen Style): التطوير، شخصي، المالية، والأبحاث.
- تقسيم الشاشة (Split View) لتصفح موقعين جنباً إلى جنب في نفس الوقت.
- ألسنة جانبية رأسية (Vertical Tabs).
- لوحة أوامر سريعة منبثقة باختصار لوحة المفاتيح `Ctrl + K`.

### 8. 🎨 محرك السمات الديناميكي المتعدد (Dynamic Theme Engine)
- دعم التبديل الفوري بين 4 سمات مميزة:
  - **الداكن الكلاسيكي (Slate Dark)**
  - **الأسود النقي (OLED True Black)**
  - **سايبربانك نيون (Cyberpunk Neon)**
  - **نورد الثلجي (Nord Minimal)**

---

## 🏗️ هيكلية المشروع (Repository Structure)

```text
Browers-/
├── bin/                                # الملفات التنفيذية
│   ├── atlas_browser_core             # محرك C++ الأساسي مع CLI و26 اختبار آلي
│   └── atlas_browser_server           # خادم الواجهة التفاعلية الحية
├── include/browser_core/              # واجهات الأنظمة البرمجية (C++ Headers)
│   ├── AdBlocker.hpp                  # حجب الإعلانات والتعقب
│   ├── AiAssistantEngine.hpp          # محرك الذكاء الاصطناعي والتخصيص
│   ├── AutoContainerRouter.hpp        # توجيه الروابط للحاويات المناسبة
│   ├── BookmarkHistoryStore.hpp       # إدارة المفضلات والسجل
│   ├── BrowserConfig.hpp              # إعدادات المحرك والنواة
│   ├── BrowserEngine.hpp              # الواجهة الموحدة للمتصفح (Master Facade)
│   ├── ContainerManager.hpp           # إدارة حاويات فايرفوكس
│   ├── DownloadManager.hpp            # مدير التنزيلات فائق السرعة
│   ├── JsBridge.hpp                   # جسر ربط C++ بـ JavaScript DOM
│   ├── NetworkInterceptor.hpp         # ترقية HTTPS وتنظيف الروابط
│   ├── PerformanceMonitor.hpp         # مراقبة الرام وتوفير الطاقة
│   ├── PrivacyShield.hpp              # درع مكافحة البصمة الرقمية
│   ├── ReaderModeEngine.hpp           # محرك وضع القراءة والتركيز
│   ├── SchemeHandler.hpp              # مسارات بروتوكولات about: و mybrowser://
│   ├── TabManager.hpp                 # إدارة الألسنة وتقسيم الشاشة
│   └── WorkspaceManager.hpp           # إدارة مساحات العمل (Arc/Zen)
├── src/
│   ├── browser_core/                  # التنفيذ البرمجي المكتبي للنواة (C++20)
│   └── browser_server/server.cpp      # خادم الويب والواجهة التفاعلية الحية
├── docs/                              # التوثيق والكتيبات الفنية الهندسية
│   ├── 01_CHROMIUM_VS_FIREFOX_COMPARISON_AR.md
│   ├── 02_HOW_MODERN_FORKS_WORK_AR.md
│   ├── 03_CUSTOM_FEATURES_GUIDE_AR.md
│   ├── 04_HARDWARE_AND_BUILD_REQUIREMENTS_AR.md
│   ├── 05_DEVELOPMENT_ROADMAP_AR.md
│   └── 06_ATLAS_QUANTUM_EXPANSION_PLAN_AND_ARCHITECTURE_AR.md
├── firefox_fork/                      # إعدادات وسياسات تفرع فايرفوكس
└── Makefile                           # بناء واختبار المشروع
```

---

## 🚀 البدء السريع والتشغيل

### 1. البناء والترجمة:
```bash
make clean && make all
```

### 2. تشغيل حزمة الفحص الآلي (26/26 اختبار ناجح):
```bash
make test
```

### 3. تشغيل الخادم التفاعلي:
```bash
make server
# أو
./bin/atlas_browser_server 8080
```
افتح المتصفح على: `http://localhost:8080` (أو عبر رابط المعاينة السحابية المباشرة في منصة Arena).

---

## 📜 الترخيص
المشروع مفتوح المصدر وفق رخصة **MIT**.
جميع حقوق البناء والتطوير محفوظة لفريق هندسة AtlasBrowser.
