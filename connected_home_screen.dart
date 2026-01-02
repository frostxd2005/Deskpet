import 'dart:async';

import 'package:flutter/material.dart';
import 'timer_screen.dart';
import 'time_weather_screen.dart';
import 'time_weather_sender.dart';

class ConnectedHomeScreen extends StatefulWidget {
  const ConnectedHomeScreen({super.key});

  @override
  State<ConnectedHomeScreen> createState() => _ConnectedHomeScreenState();
}

class _ConnectedHomeScreenState extends State<ConnectedHomeScreen> {
  Timer? _hourlySyncTimer;

  @override
  void initState() {
    super.initState();

    // 🔵 Send time + weather immediately when connected
    TimeWeatherSender.sendNow();

    // ⏰ Auto sync every 1 hour
    _hourlySyncTimer = Timer.periodic(
      const Duration(hours: 1),
      (_) => TimeWeatherSender.sendNow(),
    );
  }

  @override
  void dispose() {
    _hourlySyncTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("DeskPet"),
        centerTitle: true,
      ),
      body: Padding(
        padding: const EdgeInsets.all(72),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            const Spacer(),

            const Icon(
              Icons.android_rounded,
              size: 72,
              color: Color(0xFF64B5F6),
            ),

            const SizedBox(height: 16),

            const Text(
              "DeskPet is connected ✨",
              textAlign: TextAlign.center,
              style: TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.w600,
              ),
            ),

            const SizedBox(height: 32),

            _actionButton(
              icon: Icons.timer,
              text: "Set Timer",
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (_) => const TimerScreen(),
                  ),
                );
              },
            ),

            const SizedBox(height: 16),

            _actionButton(
              icon: Icons.cloud,
              text: "Sync DeskPet",
              emphasized: true,
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (_) => const TimeWeatherScreen(),
                  ),
                );
              },
            ),

            const SizedBox(height: 24),

            const Text(
              "Your DeskPet is happy to see you 💙",
              textAlign: TextAlign.center,
              style: TextStyle(
                fontSize: 14,
                color: Colors.black54,
              ),
            ),

            const Spacer(),
          ],
        ),
      ),
    );
  }

  // ---------- Shared button style ----------
  Widget _actionButton({
    required IconData icon,
    required String text,
    required VoidCallback onPressed,
    bool emphasized = false,
  }) {
    return SizedBox(
      width: 260,
      height: 54,
      child: ElevatedButton.icon(
        icon: Icon(icon),
        label: Text(
          text,
          textAlign: TextAlign.center,
          style: TextStyle(
            fontSize: emphasized ? 16 : 15,
            fontWeight: emphasized ? FontWeight.w600 : FontWeight.w500,
          ),
        ),
        onPressed: onPressed,
        style: ElevatedButton.styleFrom(
          elevation: emphasized ? 4 : 2,
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(28),
          ),
        ),
      ),
    );
  }
}
