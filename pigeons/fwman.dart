import 'package:pigeon/pigeon.dart';

@ConfigurePigeon(PigeonOptions(
  dartOut: 'lib/src/fwman.g.dart',
  dartPackageName: 'fwman',
  cppOptions: CppOptions(namespace: 'fwman'),
  cppHeaderOut: 'windows/fwman.g.h',
  cppSourceOut: 'windows/fwman.g.cpp',
))
class FwmanResult {
  final int code;
  final int errorCode;

  FwmanResult({
    required this.code,
    required this.errorCode,
  });
}

@HostApi()
abstract class FwmanApi {
  FwmanResult checkAndRequest(String name, String description);
}
