import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'ble_connection.dart';
import 'weather_service.dart';

class TimeWeatherSender {
  static const String timeWeatherCharSuffix = "90AE";

  static Future<void> send(String city) async {
    final device = BleConnection.device;
    if (device == null) return;

    final now = DateTime.now();
    final time =
        "${now.hour.toString().padLeft(2, '0')}:${now.minute.toString().padLeft(2, '0')}";

    final weather = await WeatherService.fetchWeatherString();

    final payload = "$time|$weather";

    final services = await device.discoverServices();
    for (final service in services) {
      for (final char in service.characteristics) {
        if (char.uuid.toString().toUpperCase().contains(timeWeatherCharSuffix)) {
          await char.write(payload.codeUnits);
          return;
        }
      }
    }
  }
}
