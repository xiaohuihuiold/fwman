import 'package:flutter/services.dart';

class Fwman {
  static const methodChannel = MethodChannel('fwman');

  static const success = 0;

  static Future<int?> request({
    required String name,
    required String description,
  }) async {
    final result = await methodChannel.invokeMethod<int>('request', {
      'name': name,
      'description': description,
    });
    return result;
  }
}
