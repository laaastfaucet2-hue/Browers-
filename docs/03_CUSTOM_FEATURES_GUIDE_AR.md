# دليل برمجة الميزات المخصصة لمتصفحك بلغة C++

يقدم هذا الدليل شرحاً تفصيلياً لكيفية برمجة أهم الميزات الحصرية في متصفحك الخاص، وكيف قمنا بتنفيذها في كود النواة الموجود في هذا المستودع (`src/browser_core/`):

---

## 1. مانع الإعلانات والتتبع المدمج (Native C++ AdBlocker)

بدلاً من الاعتماد على إضافات خارجية، يمكنك فحص الطلبات في طبقة الشبكة بلغة C++ قبل إرسالها إلى بطاقة الشبكة.

### المنطق البرمجي:
- تحليل ملفات قواعد EasyList / uBlock Filters.
- استخراج اسم النطاق (Domain Matching) ومطابقته في جدول تجزئة سريع (`std::unordered_map`).
- فحص أنماط العناوين (Wildcard & Regex Matching) عبر `std::regex`.
- إذا تطابق الطلب مع شبكة إعلانات أو أداة تتبع، يتم إرجاع إشارة `Block` أو `RV_CANCEL` فوراً.

راجع الكود المكتمل في:
- `include/browser_core/AdBlocker.hpp`
- `src/browser_core/AdBlocker.cpp`

---

## 2. ترقية HTTPS وتنظيف معلمات التجسس من الروابط (URL Cleanser)

تقوم الشركات (مثل Facebook و Google و TikTok) بحقن معرفات تتبع في الروابط مثل `?fbclid=...&utm_source=...`.

### المنطق البرمجي:
- اعتراف الرابط في `NetworkInterceptor`.
- ترقية البروتوكول من `http://` إلى `https://`.
- تحليل معلمات الاستعلام (Query String) وحذف المعلمات المحظورة برمجياً.

راجع الكود في:
- `src/browser_core/NetworkInterceptor.cpp`

---

## 3. درع مكافحة البصمة الرقمية (Anti-Fingerprinting Shield)

تحاول المواقع التعرف على هوية جهازك عبر استدعاء Canvas API و AudioContext و Navigator.

### المنطق البرمجي:
- حقن سكربت أولي (Preload Script) يُنفذ قبل تحميل أي كود في الصفحة.
- إضافة تشويش ميكروي غير مرئي (Micro-noise jitter) إلى قيم `getImageData` و `toDataURL`.
- توحيد قيم عدد الأنوية والذاكرة العشوائية (`hardwareConcurrency: 4`, `deviceMemory: 8`).
- تفعيل أعلام منع تسريب الـ IP عبر WebRTC في طبقة النواة.

راجع الكود في:
- `include/browser_core/PrivacyShield.hpp`
- `src/browser_core/PrivacyShield.cpp`

---

## 4. الصفحات الداخلية المخصصة (Custom Schemes: `mybrowser://`)

بدلاً من صفحات `chrome://`، يمكنك بناء صفحاتك الخاصة مثل:
- `mybrowser://newtab`: صفحة بداية أنيقة مع محرك بحث واختصارات.
- `mybrowser://settings`: لوحة تحكم كاملة بإعدادات المتصفح.
- `mybrowser://stats`: عدادات الإعلانات المحجوبة والذاكرة الموفرة.

راجع الكود في:
- `include/browser_core/SchemeHandler.hpp`
- `src/browser_core/SchemeHandler.cpp`

---

## 5. جسر C++ إلى JavaScript (Native Web API Bridge)

لإنشاء واجهة برمجية مخصصة لمتصفحك تستطيع المواقع وصفحاتك الداخلية استدعاؤها:
- تعريف كائنات في V8 Context عبر `CefV8Handler` أو `JsBridge`.
- يتيح لك تنفيذ دوال C++ من داخل صفحة الويب مثل:
  ```javascript
  const stats = await window.myBrowser.getAdblockStats();
  console.log('Blocked items:', stats.totalBlocked);
  ```

راجع الكود في:
- `include/browser_core/JsBridge.hpp`
- `src/browser_core/JsBridge.cpp`

---

## 6. وضع توفير الذاكرة وإراحة التبويبات (Tab Discarding / Memory Saver)

- عندما يصبح التبويب غير نشط لفترة طويلة، يقوم `TabManager` بتعليم التبويب كـ `Discarded` وتفريغ كائنات الـ DOM والذاكرة الرسومية.
- عند عودة المستخدم للتبويب، يتم إعادة تحميل المحتوى بسلاسة.

راجع الكود في:
- `include/browser_core/TabManager.hpp`
- `src/browser_core/TabManager.cpp`
