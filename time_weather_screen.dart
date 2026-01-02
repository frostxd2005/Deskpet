import 'package:flutter/material.dart';
import 'time_weather_sender.dart';

class TimeWeatherScreen extends StatelessWidget {
  const TimeWeatherScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Time & Weather"),
        centerTitle: true,
      ),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(72),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              const Icon(
                Icons.wb_sunny_rounded,
                size: 72,
                color: Color(0xFF64B5F6),
              ),

              const SizedBox(height: 16),

              const Text(
                "Sync time & weather\nwith DeskPet ☁️",
                textAlign: TextAlign.center,
                style: TextStyle(
                  fontSize: 18,
                  fontWeight: FontWeight.w600,
                ),
              ),

              const SizedBox(height: 32),

              SizedBox(
                width: 240,
                height: 52,
                child: ElevatedButton.icon(
                  icon: const Icon(Icons.sync),
                  label: const Text("Sync Now"),
                  onPressed: () async {
                    await TimeWeatherSender.send("Bangalore");

                    if (!context.mounted) return;
                    ScaffoldMessenger.of(context).showSnackBar(
                      const SnackBar(
                        content: Text("Time & weather sent 🌤️"),
                      ),
                    );
                  },
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
