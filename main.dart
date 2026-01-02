import 'package:flutter/material.dart';

import 'home_screen.dart';
import 'theme.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const DeskPetApp());
}

class DeskPetApp extends StatelessWidget {
  const DeskPetApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      theme: deskPetTheme,
      home: const HomeScreen(),
    );
  }
}
