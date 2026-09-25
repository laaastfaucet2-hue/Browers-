#include "custom_v8_handler.h"
#include <iostream>

bool CustomV8Handler::Execute(const CefString& name,
                              CefRefPtr<CefV8Value> object,
                              const CefV8ValueList& arguments,
                              CefRefPtr<CefV8Value>& retval,
                              CefString& exception) {
    if (name == "getVersion") {
        retval = CefV8Value::CreateString("AtlasBrowser v1.0.0 (C++ CEF Edition)");
        return true;
    }

    if (name == "getAdblockStats") {
        CefRefPtr<CefV8Value> stats = CefV8Value::CreateObject(nullptr, nullptr);
        stats->SetValue("adsBlocked", CefV8Value::CreateInt(142), V8_PROPERTY_ATTRIBUTE_NONE);
        stats->SetValue("trackersBlocked", CefV8Value::CreateInt(89), V8_PROPERTY_ATTRIBUTE_NONE);
        stats->SetValue("httpsUpgraded", CefV8Value::CreateInt(24), V8_PROPERTY_ATTRIBUTE_NONE);
        retval = stats;
        return true;
    }

    if (name == "togglePrivacyShield") {
        bool enabled = true;
        if (!arguments.empty() && arguments[0]->IsBool()) {
            enabled = arguments[0]->GetBoolValue();
        }
        std::cout << "[Native C++] Privacy shield toggled: " << (enabled ? "ON" : "OFF") << std::endl;
        retval = CefV8Value::CreateBool(enabled);
        return true;
    }

    exception = "Function not implemented in C++ backend";
    return false;
}
