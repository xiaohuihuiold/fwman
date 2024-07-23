#include "fwman_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <filesystem>
#include <string>
#include <locale>
#include <codecvt>
#include <netfw.h>
#include <comdef.h>
#include <wil/resource.h>
#include <wil/com.h>

namespace fwman {

    // static
    void FwmanPlugin::RegisterWithRegistrar(
            flutter::PluginRegistrarWindows *registrar) {
        auto plugin = std::make_unique<FwmanPlugin>();
        FwmanApi::SetUp(registrar->messenger(), plugin.get());
        registrar->AddPlugin(std::move(plugin));
    }

    std::wstring Utf8ToWide(const std::string &utf8Str) {
        if (utf8Str.empty())
            return std::wstring();

        int wideStrLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
        std::wstring wideStr(wideStrLen, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideStrLen);

        return wideStr;
    }

    FwmanPlugin::FwmanPlugin() {}

    FwmanPlugin::~FwmanPlugin() {}

    ErrorOr<FwmanResult> FwmanPlugin::CheckAndRequest(const std::string &name,
                                                      const std::string &description) {
        std::wstring wname = Utf8ToWide(name);
        std::wstring wdescription = Utf8ToWide(description);

        // uac
        auto pNetFwPolicy2 = wil::CoCreateInstance<INetFwPolicy2>(__uuidof(NetFwPolicy2));
        wil::unique_cotaskmem_string clsid;
        StringFromCLSID(__uuidof(NetFwPolicy2), &clsid);
        BIND_OPTS3 bo{};
        bo.cbStruct = sizeof(bo);
        bo.hwnd = GetConsoleWindow();
        bo.dwClassContext = CLSCTX_LOCAL_SERVER;
        std::wstring moniker = L"Elevation:Administrator!new:";
        moniker += clsid.get();
        auto hr = CoGetObject(moniker.c_str(), &bo, IID_PPV_ARGS(&pNetFwPolicy2));
        if (FAILED(hr)) {
            return FwmanResult(canceled, hr);
        }
        INetFwRules *pFwRules = nullptr;
        hr = pNetFwPolicy2->get_Rules(&pFwRules);
        if (FAILED(hr)) {
            pNetFwPolicy2->Release();
            return FwmanResult(error, hr);
        }

        // get path
        std::vector<wchar_t> buf(1024);
        GetModuleFileNameW(nullptr, buf.data(), (DWORD) buf.size());
        std::filesystem::path exe_path(buf.data());

        // check
        {
            IUnknown *pEnumerator = nullptr;
            hr = pFwRules->get__NewEnum(&pEnumerator);
            if (FAILED(hr)) {
                pFwRules->Release();
                pNetFwPolicy2->Release();
                return FwmanResult(error, hr);
            }

            IEnumVARIANT *pVariant = nullptr;
            hr = pEnumerator->QueryInterface(IID_PPV_ARGS(&pVariant));
            if (FAILED(hr)) {
                pEnumerator->Release();
                pFwRules->Release();
                pNetFwPolicy2->Release();
                return FwmanResult(error, hr);
            }

            bool dirIn = false;
            bool dirOut = false;

            while (true) {
                VARIANT var;
                VariantInit(&var);
                ULONG cFetched;
                hr = pVariant->Next(1, &var, &cFetched);
                if (FAILED(hr) || cFetched == 0) {
                    VariantClear(&var);
                    break;
                }

                INetFwRule *pFwRule = nullptr;
                hr = V_DISPATCH(&var)->QueryInterface(IID_PPV_ARGS(&pFwRule));
                if (FAILED(hr)) {
                    VariantClear(&var);
                    continue;
                }

                BSTR bstrAppName;
                NET_FW_RULE_DIRECTION ruleDirection;
                pFwRule->get_ApplicationName(&bstrAppName);
                pFwRule->get_Direction(&ruleDirection);
                if (bstrAppName == nullptr) {
                    pFwRule->Release();
                    VariantClear(&var);
                    continue;
                }

                if (_wcsicmp(bstrAppName, exe_path.c_str()) == 0) {
                    if (ruleDirection == NET_FW_RULE_DIR_IN) {
                        dirIn = true;
                    } else if (ruleDirection == NET_FW_RULE_DIR_OUT) {
                        dirOut = true;
                    }
                }

                SysFreeString(bstrAppName);
                pFwRule->Release();
                VariantClear(&var);
                if (dirIn && dirOut) {
                    break;
                }
            }

            if (dirIn && dirOut) {
                pVariant->Release();
                pEnumerator->Release();
                pFwRules->Release();
                pNetFwPolicy2->Release();
                return FwmanResult(added, 0);
            }
            pVariant->Release();
            pEnumerator->Release();
        }

        // request
        {
            // in
            {
                auto pFwRule = wil::CoCreateInstance<INetFwRule3>(__uuidof(NetFwRule));
                pFwRule->put_Name(_bstr_t(wname.c_str()));
                pFwRule->put_Description(_bstr_t(wdescription.c_str()));
                pFwRule->put_ApplicationName(_bstr_t(exe_path.c_str()));
                pFwRule->put_Action(NET_FW_ACTION_ALLOW);
                pFwRule->put_Profiles(NET_FW_PROFILE2_ALL);
                pFwRule->put_Direction(NET_FW_RULE_DIR_IN);
                pFwRule->put_Enabled(VARIANT_TRUE);
                hr = pFwRules->Add(pFwRule.get());
                if (FAILED(hr)) {
                    pFwRules->Release();
                    pNetFwPolicy2->Release();
                    return FwmanResult(error, 0);
                }
            }
            // out
            {
                auto pFwRule = wil::CoCreateInstance<INetFwRule3>(__uuidof(NetFwRule));
                pFwRule->put_Name(_bstr_t(wname.c_str()));
                pFwRule->put_Description(_bstr_t(wdescription.c_str()));
                pFwRule->put_ApplicationName(_bstr_t(exe_path.c_str()));
                pFwRule->put_Action(NET_FW_ACTION_ALLOW);
                pFwRule->put_Profiles(NET_FW_PROFILE2_ALL);
                pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
                pFwRule->put_Enabled(VARIANT_TRUE);
                hr = pFwRules->Add(pFwRule.get());
                if (FAILED(hr)) {
                    pFwRules->Release();
                    pNetFwPolicy2->Release();
                    return FwmanResult(error, 0);
                }
            }
            pFwRules->Release();
            pNetFwPolicy2->Release();
            return FwmanResult(fixed, 0);
        }

    }
} // namespace fwman
