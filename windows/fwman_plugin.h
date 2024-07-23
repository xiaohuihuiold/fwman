#ifndef FLUTTER_PLUGIN_FWMAN_PLUGIN_H_
#define FLUTTER_PLUGIN_FWMAN_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>
#include "fwman.g.h"

namespace fwman {
    static const int error = 0;
    static const int added = 1;
    static const int canceled = 2;
    static const int fixed = 3;

    class FwmanPlugin : public flutter::Plugin, public FwmanApi {
    public:
        static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

        FwmanPlugin();

        virtual ~FwmanPlugin();

        ErrorOr<FwmanResult> CheckAndRequest(
                const std::string &name,
                const std::string &description) override;
    };

}  // namespace fwman

#endif  // FLUTTER_PLUGIN_FWMAN_PLUGIN_H_
