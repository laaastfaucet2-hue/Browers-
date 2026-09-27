// =============================================================================
// AtlasBrowser Core Preferences (Firefox Gecko Engine)
// File: browser/app/profile/atlas-prefs.js
// =============================================================================

// 1. Enable Firefox Contextual Identities (Multi-Account Containers Engine)
pref("privacy.userContext.enabled", true);
pref("privacy.userContext.ui.enabled", true);
pref("privacy.userContext.longPressBehavior", 2);
pref("privacy.userContext.extension", "@testpilot-containers");

// 2. WebExtensions Engine & AMO (addons.mozilla.org) Access
pref("xpinstall.signatures.required", false);
pref("extensions.webextensions.restrictedDomains", "");
pref("extensions.autoDisableScopes", 0);
pref("extensions.installDistroAddons", true);
pref("extensions.htmlaboutaddons.recommendations.enabled", true);
pref("extensions.webextensions.background-delayed-startup", false);

// 3. Network & Cookie Isolation
pref("network.cookie.cookieBehavior", 1); // Reject 3rd party trackers
pref("network.http.referer.XOriginPolicy", 1);
pref("network.http.referer.trimmingPolicy", 2);

// 4. Privacy & Anti-Telemetry
pref("toolkit.telemetry.enabled", false);
pref("toolkit.telemetry.unified", false);
pref("toolkit.telemetry.archive.enabled", false);
pref("experiments.activeExperiment", false);
pref("experiments.supported", false);
pref("extensions.pocket.enabled", false);
pref("browser.newtabpage.activity-stream.feeds.telemetry", false);
pref("browser.newtabpage.activity-stream.telemetry", false);
pref("browser.ping-centre.telemetry", false);
pref("datareporting.healthreport.uploadEnabled", false);
pref("datareporting.policy.dataSubmissionEnabled", false);

// 5. Default UI & Search
pref("browser.search.defaultenginename", "DuckDuckGo");
pref("browser.startup.homepage", "about:home");
pref("browser.tabs.warnOnClose", false);
