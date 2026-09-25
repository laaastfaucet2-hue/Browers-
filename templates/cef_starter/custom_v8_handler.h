#pragma once

#include "include/cef_v8.h"

class CustomV8Handler : public CefV8Handler {
public:
    CustomV8Handler() = default;

    bool Execute(const CefString& name,
                 CefRefPtr<CefV8Value> object,
                 const CefV8ValueList& arguments,
                 CefRefPtr<CefV8Value>& retval,
                 CefString& exception) override;

private:
    IMPLEMENT_REFCOUNTING(CustomV8Handler);
};
