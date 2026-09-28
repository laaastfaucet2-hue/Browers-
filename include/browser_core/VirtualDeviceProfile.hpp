#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace BrowserCore {

struct VirtualDeviceProfile {
    uint32_t id = 1;
    std::string name;
    std::string osType;              // "Windows 11", "macOS Sonoma", "Ubuntu Linux"
    std::string userAgent;
    std::string platform;            // "Win32", "MacIntel", "Linux x86_64"
    std::string oscpu;               // "Windows NT 10.0; Win64; x64", etc.
    int hardwareConcurrency = 8;     // CPU cores: 4, 8, 12, 16, 24, 32
    int deviceMemory = 16;           // RAM GB: 4, 8, 16, 32, 64
    int screenWidth = 1920;
    int screenHeight = 1080;
    int colorDepth = 24;
    double pixelRatio = 1.0;

    // WebGL Unmasked Hardware
    std::string webglVendor;         // e.g. "Google Inc. (NVIDIA)"
    std::string webglRenderer;       // e.g. "ANGLE (NVIDIA, NVIDIA GeForce RTX 4080 Direct3D11 vs_5_0 ps_5_0)"
    
    // Canvas & Audio Fingerprint Seeds (Farbling)
    uint32_t canvasNoiseSeed = 1001;
    double audioNoiseShift = 0.00012;

    // Media Device IDs (Cameras, Microphones, Audio Output)
    std::vector<std::string> mediaDeviceIds;

    // Battery API Spoof
    double batteryLevel = 0.85;
    bool batteryCharging = true;

    // Dedicated Storage & Cookie Isolation
    std::string storageDirectory;
    bool isActive = false;

    // Generates client-side anti-detect injection script that modifies DOM and JS APIs
    std::string generateAntiDetectScript() const;
    std::string toJson() const;
};

} // namespace BrowserCore
