#include "../../include/browser_core/JsBridge.hpp"
#include <sstream>

namespace BrowserCore {

std::string JsValue::toJson() const {
    switch (type) {
        case Type::Null: return "null";
        case Type::Boolean: return boolVal ? "true" : "false";
        case Type::Number: {
            std::ostringstream ss;
            ss << numVal;
            return ss.str();
        }
        case Type::String: {
            std::string esc;
            esc.reserve(strVal.size() + 2);
            esc += '"';
            for (char c : strVal) {
                if (c == '"') esc += "\\\"";
                else if (c == '\\') esc += "\\\\";
                else if (c == '\n') esc += "\\n";
                else if (c == '\r') esc += "\\r";
                else if (c == '\t') esc += "\\t";
                else esc += c;
            }
            esc += '"';
            return esc;
        }
        case Type::Array: {
            std::string res = "[";
            for (size_t i = 0; i < arrVal.size(); ++i) {
                if (i > 0) res += ",";
                res += arrVal[i].toJson();
            }
            res += "]";
            return res;
        }
        case Type::Object: {
            std::string res = "{";
            bool first = true;
            for (const auto& kv : objVal) {
                if (!first) res += ",";
                first = false;
                res += "\"" + kv.first + "\":" + kv.second.toJson();
            }
            res += "}";
            return res;
        }
    }
    return "null";
}

JsBridge::JsBridge() {}

void JsBridge::registerFunction(const std::string& name, NativeJsFunction func) {
    m_functions[name] = std::move(func);
}

JsValue JsBridge::callFunction(const std::string& name, const std::vector<JsValue>& args) {
    auto it = m_functions.find(name);
    if (it != m_functions.end()) {
        try {
            return it->second(args);
        } catch (...) {
            JsValue err;
            err.type = JsValue::Type::Object;
            err.objVal["error"] = JsValue::makeString("Exception occurred during C++ native function execution");
            return err;
        }
    }
    JsValue notFound;
    notFound.type = JsValue::Type::Object;
    notFound.objVal["error"] = JsValue::makeString("Function not registered: " + name);
    return notFound;
}

std::vector<std::string> JsBridge::getRegisteredFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& kv : m_functions) {
        names.push_back(kv.first);
    }
    return names;
}

std::string JsBridge::generateWrapperScript() const {
    std::ostringstream ss;
    ss << "(function() {\n"
       << "  if (window.myBrowser) return;\n"
       << "  window.myBrowser = {\n";

    bool first = true;
    for (const auto& kv : m_functions) {
        if (!first) ss << ",\n";
        first = false;
        ss << "    " << kv.first << ": function(...args) {\n"
           << "      console.log('[NativeBridge] Invoking C++ function: " << kv.first << "', args);\n"
           << "      if (window.__cefQuery) {\n"
           << "        return new Promise((resolve, reject) => {\n"
           << "          window.__cefQuery({\n"
           << "            request: JSON.stringify({ func: '" << kv.first << "', args: args }),\n"
           << "            onSuccess: resolve,\n"
           << "            onFailure: (code, msg) => reject(new Error(msg))\n"
           << "          });\n"
           << "        });\n"
           << "      } else if (window.qt && window.qt.webChannelTransport) {\n"
           << "        return window.backendBridge ? window.backendBridge." << kv.first << "(...args) : Promise.reject('No Qt bridge');\n"
           << "      }\n"
           << "      return Promise.resolve({ status: 'Mock dispatch', func: '" << kv.first << "' });\n"
           << "    }";
    }

    ss << "\n  };\n"
       << "  console.log('[NativeBridge] Custom C++ Browser API injected successfully.');\n"
       << "})();\n";

    return ss.str();
}

} // namespace BrowserCore
