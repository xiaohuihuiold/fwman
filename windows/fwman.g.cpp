// Autogenerated from Pigeon (v21.0.0), do not edit directly.
// See also: https://pub.dev/packages/pigeon

#undef _HAS_EXCEPTIONS

#include "fwman.g.h"

#include <flutter/basic_message_channel.h>
#include <flutter/binary_messenger.h>
#include <flutter/encodable_value.h>
#include <flutter/standard_message_codec.h>

#include <map>
#include <optional>
#include <string>

namespace fwman {
using flutter::BasicMessageChannel;
using flutter::CustomEncodableValue;
using flutter::EncodableList;
using flutter::EncodableMap;
using flutter::EncodableValue;

FlutterError CreateConnectionError(const std::string channel_name) {
    return FlutterError(
        "channel-error",
        "Unable to establish connection on channel: '" + channel_name + "'.",
        EncodableValue(""));
}

// FwmanResult

FwmanResult::FwmanResult(
  int64_t code,
  int64_t error_code)
 : code_(code),
    error_code_(error_code) {}

int64_t FwmanResult::code() const {
  return code_;
}

void FwmanResult::set_code(int64_t value_arg) {
  code_ = value_arg;
}


int64_t FwmanResult::error_code() const {
  return error_code_;
}

void FwmanResult::set_error_code(int64_t value_arg) {
  error_code_ = value_arg;
}


EncodableList FwmanResult::ToEncodableList() const {
  EncodableList list;
  list.reserve(2);
  list.push_back(EncodableValue(code_));
  list.push_back(EncodableValue(error_code_));
  return list;
}

FwmanResult FwmanResult::FromEncodableList(const EncodableList& list) {
  FwmanResult decoded(
    list[0].LongValue(),
    list[1].LongValue());
  return decoded;
}


PigeonCodecSerializer::PigeonCodecSerializer() {}

EncodableValue PigeonCodecSerializer::ReadValueOfType(
  uint8_t type,
  flutter::ByteStreamReader* stream) const {
  switch (type) {
    case 129:
      return CustomEncodableValue(FwmanResult::FromEncodableList(std::get<EncodableList>(ReadValue(stream))));
    default:
      return flutter::StandardCodecSerializer::ReadValueOfType(type, stream);
    }
}

void PigeonCodecSerializer::WriteValue(
  const EncodableValue& value,
  flutter::ByteStreamWriter* stream) const {
  if (const CustomEncodableValue* custom_value = std::get_if<CustomEncodableValue>(&value)) {
    if (custom_value->type() == typeid(FwmanResult)) {
      stream->WriteByte(129);
      WriteValue(EncodableValue(std::any_cast<FwmanResult>(*custom_value).ToEncodableList()), stream);
      return;
    }
  }
  flutter::StandardCodecSerializer::WriteValue(value, stream);
}

/// The codec used by FwmanApi.
const flutter::StandardMessageCodec& FwmanApi::GetCodec() {
  return flutter::StandardMessageCodec::GetInstance(&PigeonCodecSerializer::GetInstance());
}

// Sets up an instance of `FwmanApi` to handle messages through the `binary_messenger`.
void FwmanApi::SetUp(
  flutter::BinaryMessenger* binary_messenger,
  FwmanApi* api) {
  FwmanApi::SetUp(binary_messenger, api, "");
}

void FwmanApi::SetUp(
  flutter::BinaryMessenger* binary_messenger,
  FwmanApi* api,
  const std::string& message_channel_suffix) {
  const std::string prepended_suffix = message_channel_suffix.length() > 0 ? std::string(".") + message_channel_suffix : "";
  {
    BasicMessageChannel<> channel(binary_messenger, "dev.flutter.pigeon.fwman.FwmanApi.checkAndRequest" + prepended_suffix, &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler([api](const EncodableValue& message, const flutter::MessageReply<EncodableValue>& reply) {
        try {
          const auto& args = std::get<EncodableList>(message);
          const auto& encodable_name_arg = args.at(0);
          if (encodable_name_arg.IsNull()) {
            reply(WrapError("name_arg unexpectedly null."));
            return;
          }
          const auto& name_arg = std::get<std::string>(encodable_name_arg);
          const auto& encodable_description_arg = args.at(1);
          if (encodable_description_arg.IsNull()) {
            reply(WrapError("description_arg unexpectedly null."));
            return;
          }
          const auto& description_arg = std::get<std::string>(encodable_description_arg);
          ErrorOr<FwmanResult> output = api->CheckAndRequest(name_arg, description_arg);
          if (output.has_error()) {
            reply(WrapError(output.error()));
            return;
          }
          EncodableList wrapped;
          wrapped.push_back(CustomEncodableValue(std::move(output).TakeValue()));
          reply(EncodableValue(std::move(wrapped)));
        } catch (const std::exception& exception) {
          reply(WrapError(exception.what()));
        }
      });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
}

EncodableValue FwmanApi::WrapError(std::string_view error_message) {
  return EncodableValue(EncodableList{
    EncodableValue(std::string(error_message)),
    EncodableValue("Error"),
    EncodableValue()
  });
}

EncodableValue FwmanApi::WrapError(const FlutterError& error) {
  return EncodableValue(EncodableList{
    EncodableValue(error.code()),
    EncodableValue(error.message()),
    error.details()
  });
}

}  // namespace fwman
