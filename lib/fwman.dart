import 'package:flutter/services.dart';
import 'package:fwman/src/fwman.g.dart';

class Fwman {
  static const error = 0;
  static const added = 1;
  static const canceled = 2;
  static const fixed = 3;

  static final _api = FwmanApi();

  static Future<FwmanResult> checkAndRequest({
    required String name,
    required String description,
  }) async {
    return await _api.checkAndRequest(name, description);
  }
}
