// Autogenerated from Pigeon (v21.0.0), do not edit directly.
// See also: https://pub.dev/packages/pigeon

#ifndef PIGEON_FWMAN_G_H_
#define PIGEON_FWMAN_G_H_
#include <flutter/basic_message_channel.h>
#include <flutter/binary_messenger.h>
#include <flutter/encodable_value.h>
#include <flutter/standard_message_codec.h>

#include <map>
#include <optional>
#include <string>

namespace fwman {


// Generated class from Pigeon.

class FlutterError {
 public:
  explicit FlutterError(const std::string& code)
    : code_(code) {}
  explicit FlutterError(const std::string& code, const std::string& message)
    : code_(code), message_(message) {}
  explicit FlutterError(const std::string& code, const std::string& message, const flutter::EncodableValue& details)
    : code_(code), message_(message), details_(details) {}

  const std::string& code() const { return code_; }
  const std::string& message() const { return message_; }
  const flutter::EncodableValue& details() const { return details_; }

 private:
  std::string code_;
  std::string message_;
  flutter::EncodableValue details_;
};

template<class T> class ErrorOr {
 public:
  ErrorOr(const T& rhs) : v_(rhs) {}
  ErrorOr(const T&& rhs) : v_(std::move(rhs)) {}
  ErrorOr(const FlutterError& rhs) : v_(rhs) {}
  ErrorOr(const FlutterError&& rhs) : v_(std::move(rhs)) {}

  bool has_error() const { return std::holds_alternative<FlutterError>(v_); }
  const T& value() const { return std::get<T>(v_); };
  const FlutterError& error() const { return std::get<FlutterError>(v_); };

 private:
  friend class FwmanApi;
  ErrorOr() = default;
  T TakeValue() && { return std::get<T>(std::move(v_)); }

  std::variant<T, FlutterError> v_;
};


// Generated class from Pigeon that represents data sent in messages.
class FwmanResult {
 public:
  // Constructs an object setting all fields.
  explicit FwmanResult(
    int64_t code,
    int64_t error_code);

  int64_t code() const;
  void set_code(int64_t value_arg);

  int64_t error_code() const;
  void set_error_code(int64_t value_arg);


 private:
  static FwmanResult FromEncodableList(const flutter::EncodableList& list);
  flutter::EncodableList ToEncodableList() const;
  friend class FwmanApi;
  friend class PigeonCodecSerializer;
  int64_t code_;
  int64_t error_code_;

};

class PigeonCodecSerializer : public flutter::StandardCodecSerializer {
 public:
  PigeonCodecSerializer();
  inline static PigeonCodecSerializer& GetInstance() {
    static PigeonCodecSerializer sInstance;
    return sInstance;
  }

  void WriteValue(
    const flutter::EncodableValue& value,
    flutter::ByteStreamWriter* stream) const override;

 protected:
  flutter::EncodableValue ReadValueOfType(
    uint8_t type,
    flutter::ByteStreamReader* stream) const override;

};

// Generated interface from Pigeon that represents a handler of messages from Flutter.
class FwmanApi {
 public:
  FwmanApi(const FwmanApi&) = delete;
  FwmanApi& operator=(const FwmanApi&) = delete;
  virtual ~FwmanApi() {}
  virtual ErrorOr<FwmanResult> CheckAndRequest(
    const std::string& name,
    const std::string& description) = 0;

  // The codec used by FwmanApi.
  static const flutter::StandardMessageCodec& GetCodec();
  // Sets up an instance of `FwmanApi` to handle messages through the `binary_messenger`.
  static void SetUp(
    flutter::BinaryMessenger* binary_messenger,
    FwmanApi* api);
  static void SetUp(
    flutter::BinaryMessenger* binary_messenger,
    FwmanApi* api,
    const std::string& message_channel_suffix);
  static flutter::EncodableValue WrapError(std::string_view error_message);
  static flutter::EncodableValue WrapError(const FlutterError& error);

 protected:
  FwmanApi() = default;

};
}  // namespace fwman
#endif  // PIGEON_FWMAN_G_H_