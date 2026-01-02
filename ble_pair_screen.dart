import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:permission_handler/permission_handler.dart';
import 'ble_connection.dart';
import 'connected_home_screen.dart';


class BlePairScreen extends StatefulWidget {
  const BlePairScreen({super.key});

  @override
  State<BlePairScreen> createState() => _BlePairScreenState();
}

class _BlePairScreenState extends State<BlePairScreen> {
  List<ScanResult> devices = [];
  bool scanning = false;

  StreamSubscription<List<ScanResult>>? scanSub;

  @override
  void initState() {
    super.initState();
    requestPermissions();
  }

  @override
  void dispose() {
    scanSub?.cancel();
    super.dispose();
  }

  /* ================= PERMISSIONS ================= */
  Future<void> requestPermissions() async {
    await [
      Permission.bluetoothScan,
      Permission.bluetoothConnect,
      Permission.location,
    ].request();
  }

  /* ================= BLE SCAN ================= */
  Future<void> startScan() async {
    devices.clear();
    setState(() => scanning = true);

    await scanSub?.cancel();

    // FIX 3: Android 14-safe scan settings
    await FlutterBluePlus.startScan(
      timeout: const Duration(seconds: 5),
      androidScanMode: AndroidScanMode.lowLatency,
      androidUsesFineLocation: true,
    );

    // FIX 2: DEBUG — show EVERYTHING Flutter sees
    scanSub = FlutterBluePlus.scanResults.listen((results) {
      for (final r in results) {
        debugPrint(
          "BLE FOUND → name='${r.device.platformName}' "
          "id=${r.device.remoteId}",
        );
      }

      setState(() {
        devices = results;
      });
    });

    await Future.delayed(const Duration(seconds: 5));
    await FlutterBluePlus.stopScan();
    await scanSub?.cancel();

    setState(() => scanning = false);
  }

  /* ================= CONNECT ================= */
  Future<void> connectToDevice(BluetoothDevice device) async {
    try {
      await device.connect(autoConnect: false);
    } catch (_) {
      // Already connected or transient error
    }

    if (!mounted) return;

    ScaffoldMessenger.of(context).showSnackBar(
      const SnackBar(
        content: Text("DeskPet connected! ✨"),
        behavior: SnackBarBehavior.floating,
      ),
    );

    BleConnection.device = device;

    Navigator.pushAndRemoveUntil(
      context,
      MaterialPageRoute(
        builder: (_) => const ConnectedHomeScreen(),
      ),
      (route) => false,
    );

  }

  /* ================= UI ================= */
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Find your DeskPet"),
        centerTitle: true,
      ),
      body: Column(
        children: [
          const SizedBox(height: 24),

          const Icon(
            Icons.auto_awesome,
            size: 64,
            color: Color(0xFF64B5F6),
          ),

          const SizedBox(height: 12),

          const Text(
            "Bring your DeskPet close and scan 💙",
            style: TextStyle(fontSize: 16),
          ),

          const SizedBox(height: 24),

          ElevatedButton.icon(
            icon: const Icon(Icons.search),
            label: Text(scanning ? "Scanning..." : "Scan for DeskPet"),
            onPressed: scanning ? null : startScan,
          ),

          const SizedBox(height: 16),

          Expanded(
            child: devices.isEmpty
                ? const Center(
                    child: Text(
                      "No devices found yet\nTap scan to try again",
                      textAlign: TextAlign.center,
                      style: TextStyle(color: Colors.black54),
                    ),
                  )
                : ListView.builder(
                    itemCount: devices.length,
                    itemBuilder: (context, index) {
                      final result = devices[index];
                      final device = result.device;
                      final name = device.platformName.isEmpty
                          ? "Unnamed BLE device"
                          : device.platformName;

                      return ListTile(
                        leading: const Icon(Icons.bluetooth),
                        title: Text(name),
                        subtitle: Text(device.remoteId.toString()),
                        onTap: () => connectToDevice(device),
                      );
                    },
                  ),
          ),
        ],
      ),
    );
  }
}
