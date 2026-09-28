#include "../../include/browser_core/VirtualDeviceProfile.hpp"
#include <sstream>

namespace BrowserCore {

static std::string escapeJson(const std::string& str) {
    std::string out;
    for (char c : str) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

std::string VirtualDeviceProfile::generateAntiDetectScript() const {
    std::ostringstream ss;
    ss << "(function() {\n"
       << "  'use strict';\n"
       << "  if (window.__atlasDeviceSpoofed) return;\n"
       << "  window.__atlasDeviceSpoofed = true;\n"
       << "  console.log('[Atlas Anti-Detect] Device Profile #" << id << " (" << name << ") active.');\n\n"
       << "  // 1. Hardware Concurrency & Memory\n"
       << "  try {\n"
       << "    Object.defineProperty(navigator, 'hardwareConcurrency', { get: () => " << hardwareConcurrency << ", configurable: true });\n"
       << "    Object.defineProperty(navigator, 'deviceMemory', { get: () => " << deviceMemory << ", configurable: true });\n"
       << "    Object.defineProperty(navigator, 'platform', { get: () => '" << platform << "', configurable: true });\n"
       << "    Object.defineProperty(navigator, 'oscpu', { get: () => '" << oscpu << "', configurable: true });\n"
       << "    Object.defineProperty(navigator, 'userAgent', { get: () => '" << userAgent << "', configurable: true });\n"
       << "    Object.defineProperty(navigator, 'appVersion', { get: () => '" << userAgent << "', configurable: true });\n"
       << "  } catch(e) {}\n\n"
       << "  // 2. Screen Dimensions & Color Depth\n"
       << "  try {\n"
       << "    Object.defineProperty(screen, 'width', { get: () => " << screenWidth << " });\n"
       << "    Object.defineProperty(screen, 'height', { get: () => " << screenHeight << " });\n"
       << "    Object.defineProperty(screen, 'availWidth', { get: () => " << screenWidth << " });\n"
       << "    Object.defineProperty(screen, 'availHeight', { get: () => " << (screenHeight - 40) << " });\n"
       << "    Object.defineProperty(screen, 'colorDepth', { get: () => " << colorDepth << " });\n"
       << "    Object.defineProperty(screen, 'pixelDepth', { get: () => " << colorDepth << " });\n"
       << "    Object.defineProperty(window, 'devicePixelRatio', { get: () => " << pixelRatio << " });\n"
       << "  } catch(e) {}\n\n"
       << "  // 3. WebGL Unmasked Hardware Vendor & Renderer Spoofing\n"
       << "  try {\n"
       << "    const getParamProto = WebGLRenderingContext.prototype.getParameter;\n"
       << "    WebGLRenderingContext.prototype.getParameter = function(param) {\n"
       << "      if (param === 0x9245) return '" << escapeJson(webglVendor) << "'; // UNMASKED_VENDOR_WEBGL\n"
       << "      if (param === 0x9246) return '" << escapeJson(webglRenderer) << "'; // UNMASKED_RENDERER_WEBGL\n"
       << "      return getParamProto.apply(this, arguments);\n"
       << "    };\n"
       << "    if (typeof WebGL2RenderingContext !== 'undefined') {\n"
       << "      const getParam2Proto = WebGL2RenderingContext.prototype.getParameter;\n"
       << "      WebGL2RenderingContext.prototype.getParameter = function(param) {\n"
       << "        if (param === 0x9245) return '" << escapeJson(webglVendor) << "';\n"
       << "        if (param === 0x9246) return '" << escapeJson(webglRenderer) << "';\n"
       << "        return getParam2Proto.apply(this, arguments);\n"
       << "      };\n"
       << "    }\n"
       << "  } catch(e) {}\n\n"
       << "  // 4. Canvas Farbling (Noise Injection with Unique Profile Seed #" << canvasNoiseSeed << ")\n"
       << "  try {\n"
       << "    const origGetImageData = CanvasRenderingContext2D.prototype.getImageData;\n"
       << "    CanvasRenderingContext2D.prototype.getImageData = function(x, y, w, h) {\n"
       << "      const imgData = origGetImageData.apply(this, arguments);\n"
       << "      const shift = (" << (canvasNoiseSeed % 5 + 1) << ");\n"
       << "      for (let i = 0; i < imgData.data.length; i += 16) {\n"
       << "        imgData.data[i] = (imgData.data[i] + shift) % 256;\n"
       << "      }\n"
       << "      return imgData;\n"
       << "    };\n"
       << "    const origToDataURL = HTMLCanvasElement.prototype.toDataURL;\n"
       << "    HTMLCanvasElement.prototype.toDataURL = function() {\n"
       << "      const ctx = this.getContext('2d');\n"
       << "      if (ctx) {\n"
       << "        try { ctx.fillStyle = 'rgba(0,0,0,0.001)'; ctx.fillRect(0, 0, 1, 1); } catch(err) {}\n"
       << "      }\n"
       << "      return origToDataURL.apply(this, arguments);\n"
       << "    };\n"
       << "  } catch(e) {}\n\n"
       << "  // 5. AudioContext Micro-shift Spoofing\n"
       << "  try {\n"
       << "    if (window.AudioBuffer) {\n"
       << "      const origGetChannelData = AudioBuffer.prototype.getChannelData;\n"
       << "      AudioBuffer.prototype.getChannelData = function() {\n"
       << "        const res = origGetChannelData.apply(this, arguments);\n"
       << "        for (let i = 0; i < Math.min(res.length, 64); i += 4) {\n"
       << "          res[i] += " << audioNoiseShift << ";\n"
       << "        }\n"
       << "        return res;\n"
       << "      };\n"
       << "    }\n"
       << "  } catch(e) {}\n\n"
       << "  // 6. Media Devices Spoofing\n"
       << "  try {\n"
       << "    if (navigator.mediaDevices && navigator.mediaDevices.enumerateDevices) {\n"
       << "      navigator.mediaDevices.enumerateDevices = async function() {\n"
       << "        return [\n"
       << "          { deviceId: '" << (mediaDeviceIds.empty() ? "dev_cam_0" : mediaDeviceIds[0]) << "', kind: 'videoinput', label: 'Integrated High-Definition Camera', groupId: 'grp_0' },\n"
       << "          { deviceId: '" << (mediaDeviceIds.size() > 1 ? mediaDeviceIds[1] : "dev_mic_0") << "', kind: 'audioinput', label: 'Default High-Definition Audio Input', groupId: 'grp_1' },\n"
       << "          { deviceId: '" << (mediaDeviceIds.size() > 2 ? mediaDeviceIds[2] : "dev_spk_0") << "', kind: 'audiooutput', label: 'Realtek Audio Output', groupId: 'grp_2' }\n"
       << "        ];\n"
       << "      };\n"
       << "    }\n"
       << "  } catch(e) {}\n"
       << "})();\n";

    return ss.str();
}

std::string VirtualDeviceProfile::toJson() const {
    std::ostringstream ss;
    ss << "{\n"
       << "  \"id\": " << id << ",\n"
       << "  \"name\": \"" << escapeJson(name) << "\",\n"
       << "  \"osType\": \"" << escapeJson(osType) << "\",\n"
       << "  \"platform\": \"" << escapeJson(platform) << "\",\n"
       << "  \"userAgent\": \"" << escapeJson(userAgent) << "\",\n"
       << "  \"hardwareConcurrency\": " << hardwareConcurrency << ",\n"
       << "  \"deviceMemory\": " << deviceMemory << ",\n"
       << "  \"screenWidth\": " << screenWidth << ",\n"
       << "  \"screenHeight\": " << screenHeight << ",\n"
       << "  \"webglVendor\": \"" << escapeJson(webglVendor) << "\",\n"
       << "  \"webglRenderer\": \"" << escapeJson(webglRenderer) << "\",\n"
       << "  \"canvasSeed\": " << canvasNoiseSeed << ",\n"
       << "  \"audioNoise\": " << audioNoiseShift << ",\n"
       << "  \"storage\": \"" << escapeJson(storageDirectory) << "\",\n"
       << "  \"isActive\": " << (isActive ? "true" : "false") << "\n"
       << "}";
    return ss.str();
}

} // namespace BrowserCore
