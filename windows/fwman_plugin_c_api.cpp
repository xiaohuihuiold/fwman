#include "include/fwman/fwman_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "fwman_plugin.h"

void FwmanPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  fwman::FwmanPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
