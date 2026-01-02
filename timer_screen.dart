import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'ble_connection.dart';

class TimerScreen extends StatefulWidget {
  const TimerScreen({super.key});

  @override
  State<TimerScreen> createState() => _TimerScreenState();
}

class _TimerScreenState extends State<TimerScreen> {
  double minutes = 5;

  Future<void> sendTimer() async {
    final device = BleConnection.device;
    if (device == null) return;

    final services = await device.discoverServices();

    for (final service in services) {
      for (final char in service.characteristics) {
        if (char.uuid.toString().toUpperCase().contains("90AD")) {
          final seconds = (minutes * 60).toInt();
          await char.write(seconds.toString().codeUnits);
        }
      }
    }

    if (!mounted) return;

    ScaffoldMessenger.of(context).showSnackBar(
      const SnackBar(
        content: Text("Timer sent to DeskPet ⏱"),
      ),
    );

    Navigator.pop(context);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Set Timer"),
        centerTitle: true,
      ),
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          children: [
            const Icon(
              Icons.timer,
              size: 72,
              color: Color(0xFF64B5F6),
            ),

            const SizedBox(height: 16),

            Text(
              "${minutes.toInt()} minutes",
              style: const TextStyle(
                fontSize: 24,
                fontWeight: FontWeight.w600,
              ),
            ),

            const SizedBox(height: 24),

            Slider(
              min: 1,
              max: 60,
              divisions: 59,
              value: minutes,
              label: "${minutes.toInt()} min",
              onChanged: (value) {
                setState(() => minutes = value);
              },
            ),

            const SizedBox(height: 32),

            ElevatedButton.icon(
              icon: const Icon(Icons.send),
              label: const Text("Start Timer"),
              onPressed: sendTimer,
            ),
          ],
        ),
      ),
    );
  }
}
