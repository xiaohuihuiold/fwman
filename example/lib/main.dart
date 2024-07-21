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

  Future<void> _request() async {
    final result = await Fwman.request(name: appid, description: '防火墙请求示例');
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      content: Text(result == Fwman.success ? '添加成功' : '添加失败'),
    ));
  }

  Future<void> _check() async {
    final result = await Fwman.check(name: appid);
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      content: Text(result == Fwman.success ? '已添加' : '未添加'),
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
              onPressed: _request,
              icon: const Icon(Icons.security),
              label: const Text('REQUEST'),
            ),
            const SizedBox(height: 12),
            ElevatedButton.icon(
              onPressed: _check,
              icon: const Icon(Icons.refresh),
              label: const Text('CHECK'),
            ),
          ],
        ),
      ),
    );
  }
}
