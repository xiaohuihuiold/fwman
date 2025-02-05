# The Flutter tooling requires that developers have a version of Visual Studio
# installed that includes CMake 3.14 or later. You should not increase this
# version, as doing so will cause the plugin to fail to compile for some
# customers of the plugin.
cmake_minimum_required(VERSION 3.14)

# Project-level configuration.
set(PROJECT_NAME "fwman")
project(${PROJECT_NAME} LANGUAGES CXX)

# Explicitly opt in to modern CMake behaviors to avoid warnings with recent
# versions of CMake.
cmake_policy(VERSION 3.14...3.25)

# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "fwman_plugin")

# 设置NuGet包的名称和版本
set(NUGET_PACKAGE_NAME "Microsoft.Windows.ImplementationLibrary")
set(NUGET_PACKAGE_VERSION "1.0.210803.1")
# 查找NuGet CLI
find_program(NUGET_EXE NAMES nuget)
if(NOT NUGET_EXE)
  message(FATAL_ERROR "NuGet CLI not found. Please ensure it is installed and in your PATH.")
endif()

# 设置NuGet包的下载目录
set(NUGET_PACKAGE_DIR "${CMAKE_BINARY_DIR}/nuget-packages")

# 使用NuGet CLI安装包
execute_process(COMMAND ${NUGET_EXE} install ${NUGET_PACKAGE_NAME} -Version ${NUGET_PACKAGE_VERSION} -OutputDirectory ${NUGET_PACKAGE_DIR})

# 设置包的根目录
set(PACKAGE_ROOT "${NUGET_PACKAGE_DIR}/${NUGET_PACKAGE_NAME}.${NUGET_PACKAGE_VERSION}")

# 假设NuGet包的库和头文件位于特定目录，根据实际情况调整
include_directories("${PACKAGE_ROOT}/include")

# Any new source files that you add to the plugin should be added here.
list(APPEND PLUGIN_SOURCES
  "fwman.g.cpp"
  "fwman.g.h"
  "fwman_plugin.cpp"
  "fwman_plugin.h"
)

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
add_library(${PLUGIN_NAME} SHARED
  "include/fwman/fwman_plugin_c_api.h"
  "fwman_plugin_c_api.cpp"
  ${PLUGIN_SOURCES}
)

# Apply a standard set of build settings that are configured in the
# application-level CMakeLists.txt. This can be removed for plugins that want
# full control over build settings.
apply_standard_settings(${PLUGIN_NAME})

# Symbols are hidden by default to reduce the chance of accidental conflicts
# between plugins. This should not be removed; any symbols that should be
# exported should be explicitly exported with the FLUTTER_PLUGIN_EXPORT macro.
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)

# Source include directories and library dependencies. Add any plugin-specific
# dependencies here.
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin)

# List of absolute paths to libraries that should be bundled with the plugin.
# This list could contain prebuilt libraries, or libraries created by an
# external build triggered from this build file.
set(fwman_bundled_libraries
  ""
  PARENT_SCOPE
)

# === Tests ===
# These unit tests can be run from a terminal after building the example, or
# from Visual Studio after opening the generated solution file.

# Only enable test builds when building the example (which sets this variable)
# so that plugin clients aren't building the tests.