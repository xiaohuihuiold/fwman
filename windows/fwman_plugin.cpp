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
        flutter::PluginRegistrarWindows* registrar) {
        auto channel =
            std::make_unique < flutter::MethodChannel < flutter::EncodableValue >>(
                registrar->messenger(), "fwman",
                &flutter::StandardMethodCodec::GetInstance());

        auto plugin = std::make_unique<FwmanPlugin>();

        channel->SetMethodCallHandler(
            [plugin_pointer = plugin.get()](const auto& call, auto result) {
                plugin_pointer->HandleMethodCall(call, std::move(result));
            });

        registrar->AddPlugin(std::move(plugin));
    }

    std::wstring Utf8ToWide(const std::string& utf8Str) {
        if (utf8Str.empty())
            return std::wstring();

        int wideStrLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
        std::wstring wideStr(wideStrLen, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideStrLen);

        return wideStr;
    }

    FwmanPlugin::FwmanPlugin() {}

    FwmanPlugin::~FwmanPlugin() {}

    void FwmanPlugin::HandleMethodCall(
        const flutter::MethodCall <flutter::EncodableValue>& method_call,
        std::unique_ptr <flutter::MethodResult<flutter::EncodableValue>> result) {
        if (method_call.method_name().compare("request") == 0) {
            // arguments
            const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
            if (!arguments) {
                result->Error("INVALID_ARGUMENTS", "Expected map arguments.");
                return;
            }

            // name
            auto name_value = arguments->find(flutter::EncodableValue("name"));
            if (name_value == arguments->end()) {
                result->Error("MISSING_ARGUMENT", "Expected 'name' argument.");
                return;
            }
            const auto* name_ptr = std::get_if<std::string>(&name_value->second);
            if (!name_ptr) {
                result->Error("INVALID_ARGUMENT_TYPE", "'name' argument should be a string.");
                return;
            }

            // description
            auto description_value = arguments->find(flutter::EncodableValue("description"));
            if (description_value == arguments->end()) {
                result->Error("MISSING_ARGUMENT", "Expected 'description' argument.");
                return;
            }
            const auto* description_ptr = std::get_if<std::string>(&description_value->second);
            if (!description_ptr) {
                result->Error("INVALID_ARGUMENT_TYPE",
                    "'description' argument should be a string.");
                return;
            }
            std::string name = *name_ptr;
            std::string description = *description_ptr;
            std::wstring wname = Utf8ToWide(name);
            std::wstring wdescription = Utf8ToWide(description);

            // firewall uac
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
                result->Success(flutter::EncodableValue(hr));
                return;
            }

            // application path
            std::vector<wchar_t> buf(1024);
            GetModuleFileNameW(nullptr, buf.data(), (DWORD)buf.size());
            std::filesystem::path exe_path(buf.data());

            // firewall rule
            auto pFwRule = wil::CoCreateInstance<INetFwRule3>(__uuidof(NetFwRule));
            pFwRule->put_Name(_bstr_t(wname.c_str()));
            pFwRule->put_Description(_bstr_t(wdescription.c_str()));
            pFwRule->put_ApplicationName(_bstr_t(exe_path.c_str()));
            pFwRule->put_Action(NET_FW_ACTION_ALLOW);
            pFwRule->put_Profiles(NET_FW_PROFILE2_ALL);
            pFwRule->put_Enabled(VARIANT_TRUE);

            // add rule
            INetFwRules* pFwRules = nullptr;
            hr = pNetFwPolicy2->get_Rules(&pFwRules);
            if (SUCCEEDED(hr)) {
                hr = pFwRules->Add(pFwRule.get());
                if (SUCCEEDED(hr)) {
                    result->Success(flutter::EncodableValue(0));
                }
                else {
                    result->Success(flutter::EncodableValue(hr));
                }
            }
            else {
                result->Success(flutter::EncodableValue(hr));
            }
            pNetFwPolicy2->Release();
            pFwRules->Release();
            //pFwRule->Release();
        }
        else if (method_call.method_name().compare("check") == 0) {
            const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
            if (!arguments) {
                result->Error("INVALID_ARGUMENTS", "Expected map arguments.");
                return;
            }

            // name
            auto name_value = arguments->find(flutter::EncodableValue("name"));
            if (name_value == arguments->end()) {
                result->Error("MISSING_ARGUMENT", "Expected 'name' argument.");
                return;
            }
            const auto* name_ptr = std::get_if<std::string>(&name_value->second);
            if (!name_ptr) {
                result->Error("INVALID_ARGUMENT_TYPE", "'name' argument should be a string.");
                return;
            }
            std::string name = *name_ptr;
            std::wstring wname = Utf8ToWide(name);

            // firewall uac
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
                result->Success(flutter::EncodableValue(hr));
                return;
            }

            // application path
            std::vector<wchar_t> buf(1024);
            GetModuleFileNameW(nullptr, buf.data(), (DWORD)buf.size());
            std::filesystem::path exe_path(buf.data());

            INetFwRules* pFwRules = nullptr;
            hr = pNetFwPolicy2->get_Rules(&pFwRules);
            if (FAILED(hr))
            {
                result->Success(flutter::EncodableValue(hr));
                pNetFwPolicy2->Release();
                return;
            }

            IUnknown* pEnumerator = nullptr;
            hr = pFwRules->get__NewEnum(&pEnumerator);
            if (FAILED(hr)) {
                result->Success(flutter::EncodableValue(hr));
                pFwRules->Release();
                pNetFwPolicy2->Release();
                CoUninitialize();
                return;
            }

            IEnumVARIANT* pVariant = nullptr;
            hr = pEnumerator->QueryInterface(IID_PPV_ARGS(&pVariant));
            if (FAILED(hr)) {
                result->Success(flutter::EncodableValue(hr));
                pEnumerator->Release();
                pFwRules->Release();
                pNetFwPolicy2->Release();
                CoUninitialize();
                return;
            }
            bool finded = false;

            while (true) {
                VARIANT var;
                VariantInit(&var);
                ULONG cFetched;
                hr = pVariant->Next(1, &var, &cFetched);
                if (FAILED(hr) || cFetched == 0) {
                    VariantClear(&var);
                    break;
                }

                INetFwRule* pFwRule = nullptr;
                hr = V_DISPATCH(&var)->QueryInterface(IID_PPV_ARGS(&pFwRule));
                if (SUCCEEDED(hr)) {
                    BSTR bstrRuleName;
                    pFwRule->get_ApplicationName(&bstrRuleName);
                    if (bstrRuleName != nullptr && wcscmp(bstrRuleName, exe_path.c_str()) == 0) {
                        result->Success(flutter::EncodableValue(0));
                        SysFreeString(bstrRuleName);
                        pFwRule->Release();
                        finded = true;
                        break;
                    }
                    SysFreeString(bstrRuleName);

                    pFwRule->Release();
                }
                VariantClear(&var);
            }

            pVariant->Release();
            pEnumerator->Release();
            pFwRules->Release();
            pNetFwPolicy2->Release();
            if (!finded){
                result->Success(flutter::EncodableValue(1));
            }
        }
        else {
            result->NotImplemented();
        }
    }
} // namespace fwman
