# AtlasBrowser (Firefox / Gecko Fork Edition) 🦊
## خارطة الطريق لبناء متصفح عملاق يدعم إضافات فايرفوكس و Multi-Account Containers

> **"رحلة المليون خطوة تبدأ بخطة محكمة"**

هذا المجلد يحتوي على الهيكل الرسمي والمعماري الكامل لتحويل المتصفح إلى **Fork رسمي من نواة Mozilla Firefox (Gecko Engine)**، وهو الأساس الوحيد عالمياً الذي يتيح لك:
1. تشغيل وتثبيت أي إضافة مباشرة من موقع موزيلا الرسمي: **[addons.mozilla.org (AMO)](https://addons.mozilla.org)**.
2. تشغيل إضافة **Firefox Multi-Account Containers** بكامل طاقتها وعزلها لملفات تعريف الارتباط.
3. التعديل المباشر في كود C++ و Rust لمحرّك Gecko وواجهة Firefox.

---

## 🗺️ الخطة التنفيذية للمشروع (The Master Plan)

```text
المرحلة 1: تهيئة البيئة البرمجية (Clang, Rust, Python, cbindgen)
   └── scripts/01_setup_gecko_environment.sh

المرحلة 2: سحب كود مصدر Gecko الرسمي النظيف
   └── scripts/02_fetch_firefox_source.sh

المرحلة 3: تطبيق هوية متصفحك والسياسات المدمجة والباتشات
   ├── branding/                  <-- شعار وهوية AtlasBrowser
   ├── distribution/policies.json <-- تفعيل الحاويات وتثبيت الإضافات آلياً
   ├── preferences/atlas-prefs.js <-- فتح متجر AMO وإلغاء قيود التوقيع
   ├── cpp_gecko_modules/         <-- خطافات C++ الأصلية للنواة
   └── scripts/03_apply_atlas_patches.sh

المرحلة 4: بناء النواة عبر محرك Mach
   └── scripts/04_build_atlas_browser.sh

المرحلة 5: تحزيم وتوليد برامج التثبيت (Deb, Tar.bz2, Exe, Dmg)
   └── scripts/05_package_atlas_browser.sh

المرحلة 6: الإطلاق والتشغيل
   └── scripts/06_run_atlas_browser.sh
```

---

## ⚙️ أسرار التكوين لدعم إضافات موزيلا والحاويات

### 1. تفعيل محرك الحاويات (Contextual Identities):
في ملف `preferences/atlas-prefs.js`:
```javascript
pref("privacy.userContext.enabled", true);
pref("privacy.userContext.ui.enabled", true);
pref("privacy.userContext.longPressBehavior", 2);
```

### 2. السماح بتثبيت أي إضافة من المتجر الرسمي بدون فحص التوقيع الإجباري:
```javascript
pref("xpinstall.signatures.required", false);
pref("extensions.webextensions.restrictedDomains", "");
```

### 3. التثبيت التلقائي لإضافة Multi-Account Containers:
في ملف `distribution/policies.json`:
```json
{
  "policies": {
    "ExtensionSettings": {
      "@testpilot-containers": {
        "installation_mode": "normal_installed",
        "install_url": "https://addons.mozilla.org/firefox/downloads/latest/multi-account-containers/latest.xpi"
      }
    }
  }
}
```

---

## 🚀 كيفية تشغيل البناء الكامل خطوة بخطوة:

```bash
cd firefox_fork
# 1. تثبيت البيئة
./scripts/01_setup_gecko_environment.sh

# 2. سحب كود فايرفوكس
./scripts/02_fetch_firefox_source.sh

# 3. دمج التعديلات والسياسات
./scripts/03_apply_atlas_patches.sh

# 4. بدء البناء
./scripts/04_build_atlas_browser.sh

# 5. تشغيل المتصفح النهائي!
./scripts/06_run_atlas_browser.sh
```
مبروك! متصفحك العملاق المخصص سيعمل الآن بواجهة فايرفوكس الرسمية مع دعم كامل لتثبيت أي إضافة من موقع موزيلا الرسمي!
