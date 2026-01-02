import 'dart:convert';
import 'package:http/http.dart' as http;

class WeatherService {
  // 🔑 Your WeatherAPI key
  static const String _apiKey = "253ae765f2b94d21aec181547252512";

  // 📍 HARD-CODED CITY
  static const String _city = "Pune";

  static Future<String> fetchWeatherString() async {
    final url = Uri.parse(
      "https://api.weatherapi.com/v1/current.json"
      "?key=$_apiKey"
      "&q=$_city"
      "&aqi=no",
    );

    try {
      final response = await http.get(url);

      if (response.statusCode != 200) {
        throw Exception("Weather API error: ${response.statusCode}");
      }

      final data = json.decode(response.body);

      final double temp = data["current"]["temp_c"];
      final String condition = data["current"]["condition"]["text"];

      // 🧾 Example result:
      // "Pune · 29°C Clear"
      return "$_city · ${temp.round()}°C $condition";
    } catch (e) {
      // Fallback if API fails
      return "$_city · Weather unavailable";
    }
  }
}
