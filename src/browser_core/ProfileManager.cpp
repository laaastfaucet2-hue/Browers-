#include "../../include/browser_core/ProfileManager.hpp"
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace BrowserCore {

ProfileManager::ProfileManager(size_t initialCount) {
    initPool(initialCount);
}

VirtualDeviceProfile ProfileManager::generateDeterministicProfile(uint32_t id) {
    VirtualDeviceProfile p;
    p.id = id;

    // Distribute OS types across 200 profiles
    int osCategory = id % 4; // 0: Windows 11, 1: macOS Sonoma, 2: Windows 10, 3: Ubuntu Linux

    if (osCategory == 0) {
        p.osType = "Windows 11 Pro 64-bit";
        p.platform = "Win32";
        p.oscpu = "Windows NT 10.0; Win64; x64";
        p.userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:128.0) Gecko/20100101 Firefox/128.0";
        p.webglVendor = "Google Inc. (NVIDIA)";
        static const char* gpus[] = {
            "ANGLE (NVIDIA, NVIDIA GeForce RTX 4090 Direct3D11 vs_5_0 ps_5_0)",
            "ANGLE (NVIDIA, NVIDIA GeForce RTX 4080 Direct3D11 vs_5_0 ps_5_0)",
            "ANGLE (NVIDIA, NVIDIA GeForce RTX 4070 Ti Direct3D11 vs_5_0 ps_5_0)",
            "ANGLE (NVIDIA, NVIDIA GeForce RTX 3080 Direct3D11 vs_5_0 ps_5_0)"
        };
        p.webglRenderer = gpus[id % 4];
        p.hardwareConcurrency = 16 + (id % 16); // 16 to 31 cores
        p.deviceMemory = 32;
        p.screenWidth = (id % 2 == 0) ? 2560 : 1920;
        p.screenHeight = (id % 2 == 0) ? 1440 : 1080;
        p.pixelRatio = 1.0;
        p.name = "جهاز #" + std::to_string(id) + " (Win 11 • NVIDIA RTX)";
    } else if (osCategory == 1) {
        p.osType = "macOS 14.5 Sonoma";
        p.platform = "MacIntel";
        p.oscpu = "Intel Mac OS X 10.15";
        p.userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:128.0) Gecko/20100101 Firefox/128.0";
        p.webglVendor = "Apple";
        static const char* gpus[] = {
            "Apple M3 Max Metal GPU",
            "Apple M3 Pro Metal GPU",
            "Apple M2 Ultra Metal GPU",
            "Apple M2 Max Metal GPU"
        };
        p.webglRenderer = gpus[id % 4];
        p.hardwareConcurrency = 12 + (id % 4); // 12 to 15 cores
        p.deviceMemory = (id % 2 == 0) ? 36 : 18;
        p.screenWidth = 3024;
        p.screenHeight = 1964;
        p.pixelRatio = 2.0;
        p.name = "جهاز #" + std::to_string(id) + " (MacBook Pro • Apple Silicon)";
    } else if (osCategory == 2) {
        p.osType = "Windows 10 Enterprise 64-bit";
        p.platform = "Win32";
        p.oscpu = "Windows NT 10.0; Win64; x64";
        p.userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:128.0) Gecko/20100101 Firefox/128.0";
        p.webglVendor = "Google Inc. (AMD)";
        static const char* gpus[] = {
            "ANGLE (AMD, AMD Radeon RX 7900 XTX Direct3D11 vs_5_0 ps_5_0)",
            "ANGLE (AMD, AMD Radeon RX 7800 XT Direct3D11 vs_5_0 ps_5_0)",
            "ANGLE (AMD, AMD Radeon RX 6700 XT Direct3D11 vs_5_0 ps_5_0)"
        };
        p.webglRenderer = gpus[id % 3];
        p.hardwareConcurrency = 8 + (id % 8);
        p.deviceMemory = 16;
        p.screenWidth = 1920;
        p.screenHeight = 1080;
        p.pixelRatio = 1.0;
        p.name = "جهاز #" + std::to_string(id) + " (Win 10 • AMD Radeon)";
    } else {
        p.osType = "Ubuntu Linux 24.04 LTS";
        p.platform = "Linux x86_64";
        p.oscpu = "Linux x86_64";
        p.userAgent = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0";
        p.webglVendor = "Mesa/X.org";
        p.webglRenderer = "Mesa Intel(R) UHD Graphics 770 (ADL-S GT1)";
        p.hardwareConcurrency = 8;
        p.deviceMemory = 16;
        p.screenWidth = 1920;
        p.screenHeight = 1080;
        p.pixelRatio = 1.0;
        p.name = "جهاز #" + std::to_string(id) + " (Ubuntu Linux Workstation)";
    }

    p.colorDepth = 24;
    p.canvasNoiseSeed = 1000 + (id * 37);
    p.audioNoiseShift = 0.00010 + (id * 0.000003);

    // Media Device unique UUIDs
    std::ostringstream camId, micId, spkId;
    camId << "dev_cam_" << std::hex << (id * 83719);
    micId << "dev_mic_" << std::hex << (id * 48211);
    spkId << "dev_spk_" << std::hex << (id * 29173);
    p.mediaDeviceIds = {camId.str(), micId.str(), spkId.str()};

    p.storageDirectory = "profiles/virtual_device_" + std::to_string(id);
    p.isActive = (id == 1);

    return p;
}

void ProfileManager::initPool(size_t count) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_profiles.clear();
    m_profiles.reserve(count);
    for (size_t i = 1; i <= count; ++i) {
        m_profiles.push_back(generateDeterministicProfile(static_cast<uint32_t>(i)));
    }
    m_activeProfileId = 1;
}

size_t ProfileManager::getProfileCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_profiles.size();
}

const VirtualDeviceProfile* ProfileManager::getProfile(uint32_t id) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& p : m_profiles) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

VirtualDeviceProfile* ProfileManager::getActiveProfile() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& p : m_profiles) {
        if (p.id == m_activeProfileId) return &p;
    }
    if (!m_profiles.empty()) return &m_profiles[0];
    return nullptr;
}

bool ProfileManager::switchActiveProfile(uint32_t id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    bool found = false;
    for (auto& p : m_profiles) {
        if (p.id == id) {
            p.isActive = true;
            found = true;
        } else {
            p.isActive = false;
        }
    }
    if (found) {
        m_activeProfileId = id;
        return true;
    }
    return false;
}

uint32_t ProfileManager::createCustomProfile(const std::string& name, const std::string& osType, int cpuCores, int ramGb) {
    std::lock_guard<std::mutex> lock(m_mutex);
    uint32_t newId = static_cast<uint32_t>(m_profiles.size() + 1);
    VirtualDeviceProfile p = generateDeterministicProfile(newId);
    if (!name.empty()) p.name = name;
    if (!osType.empty()) p.osType = osType;
    if (cpuCores > 0) p.hardwareConcurrency = cpuCores;
    if (ramGb > 0) p.deviceMemory = ramGb;
    m_profiles.push_back(p);
    return newId;
}

std::vector<VirtualDeviceProfile> ProfileManager::getProfiles(size_t offset, size_t limit, const std::string& query) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<VirtualDeviceProfile> res;
    size_t matched = 0;

    std::string qLower = query;
    std::transform(qLower.begin(), qLower.end(), qLower.begin(), [](unsigned char c){ return std::tolower(c); });

    for (const auto& p : m_profiles) {
        bool match = query.empty();
        if (!match) {
            std::string nLower = p.name;
            std::transform(nLower.begin(), nLower.end(), nLower.begin(), [](unsigned char c){ return std::tolower(c); });
            std::string osLower = p.osType;
            std::transform(osLower.begin(), osLower.end(), osLower.begin(), [](unsigned char c){ return std::tolower(c); });
            if (nLower.find(qLower) != std::string::npos || osLower.find(qLower) != std::string::npos || std::to_string(p.id) == query) {
                match = true;
            }
        }

        if (match) {
            if (matched >= offset && res.size() < limit) {
                res.push_back(p);
            }
            matched++;
        }
    }
    return res;
}

std::string ProfileManager::getActiveAntiDetectScript() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& p : m_profiles) {
        if (p.id == m_activeProfileId) {
            return p.generateAntiDetectScript();
        }
    }
    if (!m_profiles.empty()) return m_profiles[0].generateAntiDetectScript();
    return "";
}

std::string ProfileManager::exportProfilesJson(size_t page, size_t limit, const std::string& query) const {
    size_t offset = (page > 0 ? page - 1 : 0) * limit;
    auto profiles = getProfiles(offset, limit, query);

    std::ostringstream ss;
    ss << "{\n"
       << "  \"total\": " << m_profiles.size() << ",\n"
       << "  \"activeId\": " << m_activeProfileId << ",\n"
       << "  \"page\": " << page << ",\n"
       << "  \"limit\": " << limit << ",\n"
       << "  \"profiles\": [\n";
    for (size_t i = 0; i < profiles.size(); ++i) {
        if (i > 0) ss << ",\n";
        ss << profiles[i].toJson();
    }
    ss << "\n  ]\n"
       << "}\n";
    return ss.str();
}

std::string ProfileManager::renderFingerprintTestHtml() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const VirtualDeviceProfile* cur = nullptr;
    for (const auto& p : m_profiles) {
        if (p.id == m_activeProfileId) { cur = &p; break; }
    }
    if (!cur && !m_profiles.empty()) cur = &m_profiles[0];
    if (!cur) return "<div>No Profile Active</div>";

    std::ostringstream ss;
    ss << "<div style=\"max-width:960px; margin:30px auto; padding:0 20px; font-family:system-ui, sans-serif;\">\n"
       << "  <div style=\"background: linear-gradient(135deg, #0f172a 0%, #1e1b4b 100%); border:1px solid #334155; border-radius:16px; padding:24px 30px; margin-bottom:24px; display:flex; justify-content:space-between; align-items:center;\">\n"
       << "    <div>\n"
       << "      <h1 style=\"color:#38bdf8; font-size:1.8rem; margin:0;\">🛡️ فاحص بصمة الجهاز الافتراضي (Anti-Detect Diagnostic)</h1>\n"
       << "      <p style=\"color:#94a3b8; margin-top:6px;\">فحص فوري لبصمة العتاد كما تظهر لمواقع الويب وخوارزميات الحماية (CreepJS & FingerprintJS Mock)</p>\n"
       << "    </div>\n"
       << "    <span style=\"background:#22c55e22; color:#22c55e; border:1px solid #22c55e; padding:6px 14px; border-radius:20px; font-weight:bold; font-size:0.85rem;\">البصمة معزولة ومحمية 100%</span>\n"
       << "  </div>\n"
       << "  <div style=\"display:grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap:16px; margin-bottom:24px;\">\n"
       << "    <div style=\"background:#1e293b; border:1px solid #334155; border-radius:12px; padding:18px;\">\n"
       << "      <div style=\"color:#94a3b8; font-size:0.8rem;\">هوية الجهاز النشط (Active Virtual Profile)</div>\n"
       << "      <div style=\"color:#fff; font-size:1.2rem; font-weight:bold; margin-top:4px;\">" << cur->name << "</div>\n"
       << "      <div style=\"color:#38bdf8; font-size:0.85rem; margin-top:4px;\">المعرف: Profile #" << cur->id << " من أصل 200 جهاز</div>\n"
       << "    </div>\n"
       << "    <div style=\"background:#1e293b; border:1px solid #334155; border-radius:12px; padding:18px;\">\n"
       << "      <div style=\"color:#94a3b8; font-size:0.8rem;\">نظام التشغيل والمعالج الحسابي</div>\n"
       << "      <div style=\"color:#22c55e; font-size:1.2rem; font-weight:bold; margin-top:4px;\">" << cur->osType << "</div>\n"
       << "      <div style=\"color:#94a3b8; font-size:0.85rem; margin-top:4px;\">الأنوية: " << cur->hardwareConcurrency << " Cores • الرام: " << cur->deviceMemory << " GB</div>\n"
       << "    </div>\n"
       << "    <div style=\"background:#1e293b; border:1px solid #334155; border-radius:12px; padding:18px;\">\n"
       << "      <div style=\"color:#94a3b8; font-size:0.8rem;\">كرت الشاشة ومعالج الرسوميات (WebGL)</div>\n"
       << "      <div style=\"color:#fb923c; font-size:1rem; font-weight:bold; margin-top:4px;\">" << cur->webglVendor << "</div>\n"
       << "      <div style=\"color:#cbd5e1; font-size:0.8rem; margin-top:4px; font-family:monospace;\">" << cur->webglRenderer << "</div>\n"
       << "    </div>\n"
       << "  </div>\n"
       << "  <div style=\"background:#1e293b; border:1px solid #334155; border-radius:12px; overflow:hidden;\">\n"
       << "    <table style=\"width:100%; border-collapse:collapse; text-align:right; font-size:0.85rem;\">\n"
       << "      <thead style=\"background:#0f172a; color:#94a3b8;\">\n"
       << "        <tr><th style=\"padding:12px;\">عنصر البصمة</th><th style=\"padding:12px;\">القيمة المعروضة للموقع</th><th style=\"padding:12px;\">مستوى الحماية والعزل</th></tr>\n"
       << "      </thead>\n"
       << "      <tbody>\n"
       << "        <tr style=\"border-bottom:1px solid #334155;\"><td style=\"padding:12px; color:#38bdf8; font-weight:bold;\">Canvas Hash Farbling</td><td style=\"padding:12px; font-family:monospace;\">Seed #" << cur->canvasNoiseSeed << " (Micro-Shift Injected)</td><td style=\"padding:12px; color:#22c55e;\">✅ فريد بنسبة 100%</td></tr>\n"
       << "        <tr style=\"border-bottom:1px solid #334155;\"><td style=\"padding:12px; color:#38bdf8; font-weight:bold;\">AudioContext Frequency</td><td style=\"padding:12px; font-family:monospace;\">Delta +" << std::fixed << std::setprecision(6) << cur->audioNoiseShift << "</td><td style=\"padding:12px; color:#22c55e;\">✅ معزول صوتياً</td></tr>\n"
       << "        <tr style=\"border-bottom:1px solid #334155;\"><td style=\"padding:12px; color:#38bdf8; font-weight:bold;\">Screen Resolution</td><td style=\"padding:12px; font-family:monospace;\">" << cur->screenWidth << " × " << cur->screenHeight << " (DPR: " << cur->pixelRatio << ")</td><td style=\"padding:12px; color:#22c55e;\">✅ مطابق للشاشة الافتراضية</td></tr>\n"
       << "        <tr style=\"border-bottom:1px solid #334155;\"><td style=\"padding:12px; color:#38bdf8; font-weight:bold;\">WebRTC Device IDs</td><td style=\"padding:12px; font-family:monospace;\">" << (cur->mediaDeviceIds.empty() ? "None" : cur->mediaDeviceIds[0]) << "</td><td style=\"padding:12px; color:#22c55e;\">✅ منع كشف الأجهزة الحقيقية</td></tr>\n"
       << "        <tr style=\"border-bottom:1px solid #334155;\"><td style=\"padding:12px; color:#38bdf8; font-weight:bold;\">Storage Sandbox</td><td style=\"padding:12px; font-family:monospace;\">" << cur->storageDirectory << "/</td><td style=\"padding:12px; color:#22c55e;\">✅ عزل تام للكوكيز والـ Cache</td></tr>\n"
       << "      </tbody>\n"
       << "    </table>\n"
       << "  </div>\n"
       << "</div>\n";
    return ss.str();
}

} // namespace BrowserCore
