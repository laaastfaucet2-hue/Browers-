# بناء Fork مخصص مباشر من كود Firefox / Gecko

## كيف تعمل مشتقات Firefox الشهيرة؟
مشاريع مثل **LibreWolf**, **Floorp**, **Waterfox**, **Zen Browser**:
- تأخذ كود نواة **Gecko** من موزيلا (Mozilla Central أو ESR).
- تستخدم ملف إعدادات `.mozconfig` لتعطيل الميزات غير المرغوبة (Telemetry, Pocket, CrashReporter).
- تطبق باتشات مخصصة بـ C++ و Rust و JavaScript لتغيير الواجهة وإضافة ميزات جديدة كالألسنة الجانبية (Vertical Tabs) وإدارة الخصوصية الصارمة.

---

## متطلبات البناء:
- **المعالج:** 8 أنوية أو أكثر.
- **الذاكرة العشوائية:** 16GB إلى 32GB RAM.
- **مساحة القرص:** 60 إلى 80GB SSD.
- **أدوات البرمجة:** Rust + Cargo الحديث، Clang، Python 3، أداة موزيلا `mach`.

---

## خطوات التشغيل:
```bash
chmod +x scripts/*.sh
./scripts/01_install_firefox_prerequisites.sh
./scripts/02_fetch_firefox.sh
./scripts/03_apply_patches.sh
./scripts/04_build.sh
```
بعد انتهاء الترجمة، يمكنك تجربة المتصفح مباشرة:
```bash
cd ~/firefox_build/mozilla-unified
./mach run
```
