import 'package:flutter/material.dart';

final deskPetTheme = ThemeData(
  brightness: Brightness.light,
  scaffoldBackgroundColor: const Color(0xFFEAF6FF), // soft light blue
  primaryColor: const Color(0xFF90CAF9),

  fontFamily: 'Nunito',

  colorScheme: const ColorScheme.light(
    primary: Color(0xFF64B5F6),
    secondary: Color(0xFF4FC3F7),
  ),

  appBarTheme: const AppBarTheme(
    backgroundColor: Color(0xFFBBDEFB),
    foregroundColor: Colors.black,
    elevation: 0,
    centerTitle: true,
    titleTextStyle: TextStyle(
      fontSize: 22,
      fontWeight: FontWeight.w700,
      color: Colors.black,
    ),
  ),

  textTheme: const TextTheme(
    bodyMedium: TextStyle(
      fontSize: 16,
      color: Colors.black87,
    ),
  ),

  elevatedButtonTheme: ElevatedButtonThemeData(
    style: ElevatedButton.styleFrom(
      backgroundColor: Color(0xFF90CAF9),
      foregroundColor: Colors.black,
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(24),
      ),
      padding: EdgeInsets.symmetric(horizontal: 32, vertical: 16),
      textStyle: TextStyle(
        fontSize: 18,
        fontWeight: FontWeight.w700,
      ),
    ),
  ),
);
