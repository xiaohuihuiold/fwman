import 'package:flutter/material.dart';
import 'package:fwman/fwman.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      home: FirewallPage(),
    );
  }
}

class FirewallPage extends StatefulWidget {
  const FirewallPage({super.key});

  @override
  State<FirewallPage> createState() => _FirewallPageState();
}

class _FirewallPageState extends State<FirewallPage> {
  static const appid = 'com.example.app';

  Future<void> _check() async {
    final result =
        await Fwman.checkAndRequest(name: appid, description: '测试添加防火墙');
    String message = switch (result.code) {
      0 => '错误',
      1 => '已添加到防火墙',
      2 => '用户取消',
      3 => '检查到防火墙问题并尝试修复',
      int() => '',
    };
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      content:
          Text(result.code == 0 ? '$message:${result.errorCode}' : message),
    ));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Firewall Example'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            ElevatedButton.icon(
              onPressed: _check,
              icon: const Icon(Icons.security),
              label: const Text('CHECK'),
            ),
          ],
        ),
      ),
    );
  }
}
