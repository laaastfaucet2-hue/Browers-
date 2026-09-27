// Firefox Preferences for Custom Fork with Multi-Account Containers Support
// Path: browser/app/profile/user.js

// 1. Enable Firefox Contextual Identities (Multi-Account Containers Engine)
pref("privacy.userContext.enabled", true);
pref("privacy.userContext.ui.enabled", true);
pref("privacy.userContext.longPressBehavior", 2);

// 2. Allow installing extensions directly from addons.mozilla.org & local XPI
pref("xpinstall.signatures.required", false);
pref("extensions.webextensions.restrictedDomains", "");
pref("extensions.autoDisableScopes", 0);

// 3. Pre-install or allow Multi-Account Containers
pref("extensions.installDistroAddons", true);

// 4. Disable Mozilla Telemetry & Tracking
pref("toolkit.telemetry.enabled", false);
pref("toolkit.telemetry.unified", false);
pref("experiments.activeExperiment", false);
pref("experiments.supported", false);
pref("extensions.pocket.enabled", false);
pref("browser.newtabpage.activity-stream.feeds.telemetry", false);
pref("browser.ping-centre.telemetry", false);
pref("datareporting.healthreport.uploadEnabled", false);
