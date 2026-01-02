import 'package:flutter/material.dart';

import 'ble_pair_screen.dart';
import 'ble_connection.dart';
import 'connected_home_screen.dart';
import 'notification_bridge.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  bool _notificationInitDone = false;

  @override
  void initState() {
    super.initState();

    // Initialize notification bridge AFTER first frame
    WidgetsBinding.instance.addPostFrameCallback((_) async {
      if (!mounted) return;

      if (!_notificationInitDone) {
        _notificationInitDone = true;
        await NotificationBridge.init(); // ✅ correct place
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    // If DeskPet is already connected, show connected home
    if (BleConnection.isConnected) {
      return const ConnectedHomeScreen();
    }

    // Otherwise show pairing screen
    return Scaffold(
      appBar: AppBar(
        title: const Text("DeskPet"),
        centerTitle: true,
      ),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.symmetric(horizontal: 24),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              const Icon(
                Icons.auto_awesome,
                size: 72,
                color: Color(0xFF64B5F6),
              ),

              const SizedBox(height: 16),

              const Text(
                "Your little desk companion 🎇",
                textAlign: TextAlign.center,
                style: TextStyle(
                  fontSize: 18,
                  fontWeight: FontWeight.w600,
                ),
              ),

              const SizedBox(height: 32),

              ElevatedButton.icon(
                icon: const Icon(Icons.bluetooth),
                label: const Text("Pair DeskPet"),
                style: ElevatedButton.styleFrom(
                  padding: const EdgeInsets.symmetric(
                    horizontal: 32,
                    vertical: 14,
                  ),
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(20),
                  ),
                ),
                onPressed: () {
                  Navigator.push(
                    context,
                    MaterialPageRoute(
                      builder: (_) => const BlePairScreen(),
                    ),
                  );
                },
              ),

              const SizedBox(height: 16),

              const Text(
                "Turn on DeskPet and bring it close",
                textAlign: TextAlign.center,
                style: TextStyle(
                  fontSize: 14,
                  color: Colors.black54,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}