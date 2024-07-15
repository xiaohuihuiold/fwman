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
  Future<void> _request() async {
    final result =
        await Fwman.request(name: 'com.example.app', description: '防火墙请求示例');
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      content: Text('$result'),
    ));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Firewall Example'),
      ),
      body: Center(
        child: ElevatedButton.icon(
          onPressed: _request,
          icon: const Icon(Icons.security),
          label: const Text('REQUEST'),
        ),
      ),
    );
  }
}
