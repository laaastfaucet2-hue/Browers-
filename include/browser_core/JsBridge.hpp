#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <memory>

namespace BrowserCore {

struct JsValue {
    enum class Type { Null, Boolean, Number, String, Array, Object };
    Type type = Type::Null;
    bool boolVal = false;
    double numVal = 0.0;
    std::string strVal;
    std::vector<JsValue> arrVal;
    std::map<std::string, JsValue> objVal;

    static JsValue makeString(const std::string& s) {
        JsValue v; v.type = Type::String; v.strVal = s; return v;
    }
    static JsValue makeNumber(double n) {
        JsValue v; v.type = Type::Number; v.numVal = n; return v;
    }
    static JsValue makeBool(bool b) {
        JsValue v; v.type = Type::Boolean; v.boolVal = b; return v;
    }
    static JsValue makeObject() {
        JsValue v; v.type = Type::Object; return v;
    }

    std::string toJson() const;
};

using NativeJsFunction = std::function<JsValue(const std::vector<JsValue>& args)>;

class JsBridge {
public:
    JsBridge();
    ~JsBridge() = default;

    // Register a C++ function exposed to JavaScript under window.myBrowser.<name>
    void registerFunction(const std::string& name, NativeJsFunction func);

    // Call native C++ function from JS dispatch
    JsValue callFunction(const std::string& name, const std::vector<JsValue>& args);

    // Generate the JS wrapper script that sets up window.myBrowser
    std::string generateWrapperScript() const;

    std::vector<std::string> getRegisteredFunctionNames() const;

private:
    std::map<std::string, NativeJsFunction> m_functions;
};

} // namespace BrowserCore
