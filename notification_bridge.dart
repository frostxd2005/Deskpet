import 'dart:convert';
import 'package:notification_listener_service/notification_listener_service.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'ble_connection.dart';

class NotificationBridge {
  static bool _initialized = false;
  static bool _listening = false;

  static Future<void> init() async {
    if (_initialized) return;
    _initialized = true;

    try {
      // 1️⃣ Check permission
      final granted =
          await NotificationListenerService.isPermissionGranted();

      if (!granted) {
        await NotificationListenerService.requestPermission();
        return; // ⛔ WAIT for user to reopen app
      }

      // 2️⃣ Start listening ONLY once
      if (_listening) return;
      _listening = true;

      NotificationListenerService.notificationsStream.listen(
        (event) async {
          // SAFETY: BLE must be connected
          final device = BleConnection.device;
          if (device == null) return;

          // SAFETY: Some notifications are empty
          final app = event.packageName;
          final title = event.title;
          final text = event.content;

          if (app == null && title == null && text == null) return;

          final payload = jsonEncode({
            "app": app ?? "Unknown",
            "title": title ?? "",
            "text": text ?? "",
          });

          try {
            final services = await device.discoverServices();

            for (final service in services) {
              for (final char in service.characteristics) {
                if (char.uuid
                    .toString()
                    .toUpperCase()
                    .contains("90AC")) {
                  await char.write(
                    payload.codeUnits,
                    withoutResponse: false,
                  );
                  return;
                }
              }
            }
          } catch (_) {
            // BLE errors are normal — never crash
          }
        },
        onError: (_) {
          // Ignore stream errors safely
        },
      );
    } catch (_) {
      // Absolute safety net — never crash app
    }
  }
}
