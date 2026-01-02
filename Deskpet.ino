/*************************************************
 * DESKPET – ESP32 BLE + OLED + TOUCH
 * FINAL: TIRED with blinking + notif confused + happy on touch
 * Added icons to notifications, timer, and time/weather screens.
 *************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* -------- RoboEyes -------- */
#include <FluxGarage_RoboEyes.h>
#undef N
#undef S
#undef E
#undef W

#include <ArduinoJson.h>

/* -------- BLE -------- */
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

/* ================= OLED ================= */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
#define I2C_ADDR     0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

/* ================= TOUCH ================= */
#define TOUCH_PIN 19
#define LONG_PRESS_MS 1000

bool touchPrev = false;
unsigned long touchStartTime = 0;

/* ================= SCREENS ================= */
enum ScreenState {
  SCREEN_EYES = 0,
  SCREEN_TIME_WEATHER,
  SCREEN_NOTIFICATIONS,
  SCREEN_TIMER
};

ScreenState currentScreen = SCREEN_EYES;

/* ================= TIME ================= */
int currentHour = 12;
int currentMinute = 34;
unsigned long lastMinuteTick = 0;

/* ================= WEATHER ================= */
String currentWeather = "Clear 28C";

/* ================= TIMER ================= */
bool timerActive = false;
int timerSecondsLeft = 0;
unsigned long lastTimerTick = 0;

/* ================= NOTIFICATIONS ================= */
String lastNotifApp   = "";
String lastNotifTitle = "";

/* ================= IDLE / SLEEP / ANIM ================= */
unsigned long idleAnimTimer = 0;
unsigned long idleAnimInterval = 0;
bool idleBlinkMode = true;

unsigned long lastInteractionMillis = 0;
const unsigned long SLEEP_TIMEOUT_MS = 30UL * 1000UL; // 30 seconds
bool sleepyMode = false;

bool notifAnimActive = false;
unsigned long notifAnimStart = 0;
const unsigned long NOTIF_ANIM_MS = 10UL * 1000UL; // 10 seconds

bool happyAnimActive = false;
unsigned long happyAnimStart = 0;
const unsigned long HAPPY_RETURN_MS = 10UL * 1000UL; // 10 seconds

/* ================= BLE UUIDs ================= */
#define DESKPET_SERVICE_UUID       "12345678-1234-1234-1234-1234567890AB"
#define NOTIFY_CHAR_UUID           "12345678-1234-1234-1234-1234567890AC"
#define TIMER_CHAR_UUID            "12345678-1234-1234-1234-1234567890AD"
#define TIME_WEATHER_CHAR_UUID     "12345678-1234-1234-1234-1234567890AE"

/* ================ ICON BITMAPS (from your message, named uniquely) ================ */
/* Note: bitmaps are stored in PROGMEM and used with Adafruit_SSD1306::drawBitmap */

/* 'spotify', 16x16px */
const unsigned char bmp_spotify [] PROGMEM = {
  0x07, 0xe0, 0x1c, 0x38, 0x30, 0x0c, 0x60, 0x06, 0x40, 0x02, 0xcf, 0xf3,
  0x80, 0x11, 0x8f, 0xc1, 0x88, 0xf1, 0x80, 0x01, 0xcf, 0xe3, 0x40, 0x02,
  0x60, 0x06, 0x30, 0x0c, 0x1c, 0x38, 0x07, 0xe0
};


/* 'whatsapp', 16x16px */
const unsigned char bmp_whatsapp [] PROGMEM = {
  0x03, 0xc0, 0x0f, 0xf0, 0x18, 0x1c, 0x20, 0x04, 0x64, 0x02, 0x4c, 0x02,
  0xce, 0x03, 0xcc, 0x03, 0xc6, 0x03, 0xc3, 0x73, 0x41, 0xf2, 0x60, 0x66,
  0x60, 0x04, 0x60, 0x18, 0x7f, 0xf0, 0x03, 0xc0
};


/* 'instagram-new', 15x16px */
const unsigned char bmp_instagram [] PROGMEM = {
  0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x1f, 0xf1, 0x10, 0x11, 0x11, 0x91,
  0x12, 0x51, 0x14, 0x51, 0x14, 0x51, 0x13, 0x91, 0x10, 0x11, 0x1f, 0xf1,
  0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01
};


/* 'settings', 16x16px */
const unsigned char bmp_settings [] PROGMEM = {
  0x01, 0x80, 0x02, 0x40, 0x3e, 0x7c, 0x20, 0x24, 0x20, 0x04, 0x23, 0xcc,
  0x64, 0x26, 0x84, 0x21, 0x84, 0x21, 0x64, 0x26, 0x23, 0xc4, 0x20, 0x04,
  0x20, 0x04, 0x3e, 0x7c, 0x02, 0x40, 0x01, 0x80
};


/* 'notification' (general), 16x16px */
const unsigned char bmp_notification [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xb0, 0x04, 0x00, 0x08, 0x30,
  0x08, 0x00, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0, 0x00, 0x00,
  0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/* Timer icon, 16x16px */
const unsigned char bmp_timer [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x60, 0x00, 0x70, 0x01, 0x9c,
  0x00, 0x34, 0x0e, 0x3a, 0x00, 0x3a, 0x3e, 0x3e, 0x1e, 0x7a, 0x00, 0x74,
  0x01, 0x0c, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00
};


/* Clock icon, 16x16px */
const unsigned char bmp_clock [] PROGMEM = {
  0x07, 0xe0, 0x1f, 0xf8, 0x38, 0x1c, 0x70, 0x0e, 0x61, 0x06, 0xc1, 0x03,
  0xc1, 0x03, 0xc1, 0x03, 0xc1, 0xf3, 0xc1, 0xf3, 0xc0, 0x03, 0x60, 0x06,
  0x70, 0x0e, 0x38, 0x1c, 0x1f, 0xf8, 0x07, 0xe0
};


/* Bluetooth icon, 8x8px */
const unsigned char bmp_bluetooth [] PROGMEM = {
  0x00, 0x0c, 0x24, 0x18, 0x18, 0x24, 0x0c, 0x00
};


/* Weather icon, 16x16px */
const unsigned char bmp_weather [] PROGMEM = {
  0x00, 0x00, 0x01, 0x00, 0x09, 0x20, 0x04, 0x40, 0x04, 0x40, 0x11, 0xf0,
  0x06, 0x10, 0x04, 0x0c, 0x08, 0x00, 0x00, 0x0a, 0x08, 0x00, 0x04, 0x0c,
  0x00, 0x00, 0x3d, 0x00, 0x00, 0x00, 0x38, 0x00
};


/* ================= HELPERS: drawing icons and mapping app names ================= */

/*
  drawIconAt - wrapper around display.drawBitmap.
  We use drawBitmap which accepts PROGMEM arrays with Adafruit_SSD1306.
*/
void drawIconAt(int16_t x, int16_t y, const unsigned char *bmp, uint8_t w, uint8_t h) {
  // Adafruit_SSD1306::drawBitmap expects PROGMEM style bitmaps; the library handles pgm_read.
  display.drawBitmap(x, y, bmp, w, h, SSD1306_WHITE);
}

/*
  mapAppAndDrawIcon:
    - takes original app identifier (package name or app string),
    - draws corresponding icon at (x,y),
    - returns the display-friendly app name to show.
*/
String mapAppAndDrawIcon(const String &app, int16_t x, int16_t y) {
  String lowerApp = app;
  lowerApp.toLowerCase();

  if (lowerApp.indexOf("whatsapp") >= 0) {
    drawIconAt(x, y, bmp_whatsapp, 16, 16);
    return "WhatsApp";
  } else if (lowerApp.indexOf("instagram") >= 0) {
    // instagram bitmap is 15x16; center it in 16px box
    drawIconAt(x, y, bmp_instagram, 15, 16);
    return "Instagram";
  } else if (lowerApp.indexOf("spotify") >= 0) {
    drawIconAt(x, y, bmp_spotify, 16, 16);
    return "Spotify";
  } else if (lowerApp.indexOf("setting") >= 0 || lowerApp.indexOf("android.settings") >= 0) {
    drawIconAt(x, y, bmp_settings, 16, 16);
    return "Settings";
  } else {
    // Default/general notification icon
    drawIconAt(x, y, bmp_notification, 16, 16);
    // Return the original app label but shortened to 18 chars if too long
    String displayName = app;
    if (displayName.length() > 18) displayName = displayName.substring(0, 18) + "...";
    return displayName;
  }
}

/* ================ FORWARD DECLARATIONS ================ */
void drawCurrentScreen();
void drawTimeWeather();
void drawNotifications();
void drawTimer();
void scheduleNextIdleAnim();
void runIdleEyes();
void enterSleepyMode();
void exitSleepyMode();
void triggerHappyAnim();
void setupBLE();

/* ================= BLE CALLBACKS ================= */

class NotifyCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *c) override {
    // Parse JSON value
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, c->getValue().c_str());
    if (err) {
      // Bad JSON: ignore
      return;
    }

    const char* a = doc["app"];
    const char* t = doc["title"];
    lastNotifApp   = a ? String(a) : String("");
    lastNotifTitle = t ? String(t) : String("");

    // Show confused animation for NOTIF_ANIM_MS on eyes screen
    notifAnimActive = true;
    notifAnimStart = millis();

    // Ensure eyes screen is visible so user sees animation
    currentScreen = SCREEN_EYES;

    // Play confused animation (library-provided)
    roboEyes.anim_confused();
  }
};

class TimerCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *c) override {
    timerSecondsLeft = String(c->getValue().c_str()).toInt();
    timerActive = timerSecondsLeft > 0;
    lastTimerTick = millis();
    // if timer screen visible, refresh
    if (currentScreen == SCREEN_TIMER) drawCurrentScreen();
  }
};

class TimeWeatherCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *c) override {
    String data = c->getValue().c_str();
    int sep = data.indexOf('|');
    if (sep > 0) {
      String t = data.substring(0, sep);
      currentWeather = data.substring(sep + 1);
      if (t.length() >= 5) {
        currentHour   = t.substring(0, 2).toInt();
        currentMinute = t.substring(3, 5).toInt();
      }
      lastMinuteTick = millis();
      if (currentScreen == SCREEN_TIME_WEATHER) drawCurrentScreen();
    }
  }
};

/* ================= SETUP ================= */

void setup() {
  pinMode(TOUCH_PIN, INPUT);

  Serial.begin(115200);
  delay(1200);                 // allow power rails to stabilize
  Wire.begin(21, 22);
  Wire.setClock(100000);       // slower I2C for reliability with Blue OLED
  delay(200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR)) {
    // OLED init failed — halt to avoid unpredictable behavior
    while (true) { delay(1000); }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();

  // RoboEyes uses the Adafruit_SSD1306 instance you already have
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 60);
  roboEyes.setMood(DEFAULT);
  roboEyes.setAutoblinker(OFF);
  roboEyes.setIdleMode(OFF);

  scheduleNextIdleAnim();

  lastInteractionMillis = millis();

  // BLE
  setupBLE();

  // draw initial screen (eyes screen does not draw)
  drawCurrentScreen();
}

/* ================= LOOP ================= */

void loop() {
  // Touch handling (detect presses/releases)
  bool touchNow = digitalRead(TOUCH_PIN);
  if (touchNow && !touchPrev) touchStartTime = millis();

  if (!touchNow && touchPrev) {
    unsigned long d = millis() - touchStartTime;
    lastInteractionMillis = millis();

    // If waking from sleepy mode, any touch triggers the happy laugh animation
    if (sleepyMode) {
      triggerHappyAnim(); // triggerHappyAnim calls exitSleepyMode internally
      // ensure eyes screen is visible
      currentScreen = SCREEN_EYES;
    } else {
      // Not sleepy: long-press = happy, short press = cycle screens
      if (d >= LONG_PRESS_MS) {
        triggerHappyAnim();
      } else {
        currentScreen = (ScreenState)((currentScreen + 1) % 4);
        drawCurrentScreen();
      }
    }
  }
  touchPrev = touchNow;

  // update clock every minute
  if (millis() - lastMinuteTick >= 60000) {
    lastMinuteTick += 60000;
    currentMinute++;
    if (currentMinute >= 60) {
      currentMinute = 0;
      currentHour = (currentHour + 1) % 24;
    }
    if (currentScreen == SCREEN_TIME_WEATHER) drawCurrentScreen();
  }

  // update timer every second
  if (timerActive && millis() - lastTimerTick >= 1000) {
    lastTimerTick += 1000;
    timerSecondsLeft--;
    if (timerSecondsLeft <= 0) timerActive = false;
    if (currentScreen == SCREEN_TIMER) drawCurrentScreen();
  }

  // happy animation timeout -> restore mood after HAPPY_RETURN_MS
  if (happyAnimActive && millis() - happyAnimStart >= HAPPY_RETURN_MS) {
    happyAnimActive = false;
    if (sleepyMode) {
      // if somehow sleepyMode is still true, restore tired mood
      roboEyes.setMood(TIRED);
      roboEyes.setAutoblinker(ON, 2, 1);
      roboEyes.setIdleMode(ON, 1, 0);
    } else {
      // normal restore to default idle
      roboEyes.setMood(DEFAULT);
      roboEyes.setAutoblinker(OFF);
      roboEyes.setIdleMode(OFF);
      scheduleNextIdleAnim();
    }
  }

  // notification animation timeout -> restore mood after NOTIF_ANIM_MS
  if (notifAnimActive && millis() - notifAnimStart >= NOTIF_ANIM_MS) {
    notifAnimActive = false;
    if (sleepyMode) {
      // go back to TIRED blinking/movement
      roboEyes.setMood(TIRED);
      roboEyes.setAutoblinker(ON, 2, 1);
      roboEyes.setIdleMode(ON, 1, 0);
    } else {
      roboEyes.setMood(DEFAULT);
      roboEyes.setAutoblinker(OFF);
      roboEyes.setIdleMode(OFF);
      scheduleNextIdleAnim();
    }
  }

  // if idle for SLEEP_TIMEOUT_MS, enter sleepy mode
  if (!sleepyMode && (millis() - lastInteractionMillis >= SLEEP_TIMEOUT_MS)) {
    enterSleepyMode();
  }

  // update eyes only when on eyes screen
  if (currentScreen == SCREEN_EYES) {
    runIdleEyes();
    roboEyes.update();
  }

  delay(5); // small yield
}

/* ================= BLE SETUP ================= */

void setupBLE() {
  BLEDevice::init("DeskPet");
  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(DESKPET_SERVICE_UUID);

  service->createCharacteristic(NOTIFY_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE)
         ->setCallbacks(new NotifyCallback());
  service->createCharacteristic(TIMER_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE)
         ->setCallbacks(new TimerCallback());
  service->createCharacteristic(TIME_WEATHER_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE)
         ->setCallbacks(new TimeWeatherCallback());

  service->start();
  BLEDevice::getAdvertising()->start();
}

/* ================= HELPERS ================= */

void enterSleepyMode() {
  sleepyMode = true;
  // set TIRED mood but still blink and move a little
  roboEyes.setMood(TIRED);
  // allow gentle blinking and idle movement in tired mode
  roboEyes.setAutoblinker(ON, 2, 1);
  roboEyes.setIdleMode(ON, 1, 0);
  // do not schedule the normal (lively) idle animations
}

void exitSleepyMode() {
  if (!sleepyMode) return;
  sleepyMode = false;
  // restore default idle after waking
  roboEyes.setMood(DEFAULT);
  roboEyes.setAutoblinker(OFF);
  roboEyes.setIdleMode(OFF);
  scheduleNextIdleAnim();
}

void triggerHappyAnim() {
  // If we were sleepy, we want to exit sleepy first (so animation looks correct)
  exitSleepyMode();

  happyAnimActive = true;
  happyAnimStart = millis();
  currentScreen = SCREEN_EYES;
  roboEyes.setMood(HAPPY);
  roboEyes.anim_laugh(); // library's laugh/happy animation
}

/* ================= IDLE EYES ================= */

void scheduleNextIdleAnim() {
  idleAnimInterval = random(3000, 6000);
  idleAnimTimer = millis();
  idleBlinkMode = random(0, 2);
}

void runIdleEyes() {
  // If we're in a notification animation or happy animation, don't override it
  if (notifAnimActive || happyAnimActive) return;

  // If sleepyMode: don't run the normal lively idle; RoboEyes is already set for TIRED
  if (sleepyMode) {
    // keep RoboEyes' built-in behavior (we set autoblanker/idle in enterSleepyMode)
    return;
  }

  if (millis() - idleAnimTimer > idleAnimInterval) {
    idleAnimInterval = random(3000, 6000);
    idleAnimTimer = millis();
    idleBlinkMode = !idleBlinkMode;
  }

  if (idleBlinkMode) roboEyes.setAutoblinker(ON, 3, 1);
  else roboEyes.setIdleMode(ON, 1, 0);
}

/* ================= DRAW SCREENS ================= */

void drawCurrentScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (currentScreen == SCREEN_TIME_WEATHER) drawTimeWeather();
  else if (currentScreen == SCREEN_NOTIFICATIONS) drawNotifications();
  else if (currentScreen == SCREEN_TIMER) drawTimer();

  // eyes screen intentionally does not draw text — RoboEyes runs separately
}

void drawTimeWeather() {
  display.clearDisplay();

  // Draw clock icon at left and time to the right
  drawIconAt(0, 0, bmp_clock, 16, 16);

  display.setTextSize(2);
  display.setCursor(20, 0); // 16 px icon + small padding
  if (currentHour < 10) display.print("0");
  display.print(currentHour);
  display.print(":");
  if (currentMinute < 10) display.print("0");
  display.println(currentMinute);

  // Draw Bluetooth icon in top-right (8x8), adjust x to place near corner
  drawIconAt(SCREEN_WIDTH - 8 - 1, 0, bmp_bluetooth, 8, 8);

  // Weather icon left, text on its right
  drawIconAt(0, 32, bmp_weather, 16, 16);
  display.setTextSize(1);
  display.setCursor(20, 32);
  display.println(currentWeather);

  display.display();
}

void drawNotifications() {
  display.clearDisplay();
  display.setTextSize(1);

  if (lastNotifApp.length() == 0) {
    // No notifications -> centered text plus generic icon
    drawIconAt(56 - 8, 8, bmp_notification, 16, 16); // center-ish
    display.setCursor(18, 28);
    display.println("No notifications");
  } else {
    // Draw icon for app at 0,0 and app name at x=18
    String appToShow = mapAppAndDrawIcon(lastNotifApp, 0, 0);
    display.setCursor(18, 0);
    display.setTextSize(1);
    display.println(appToShow);
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    display.setCursor(18, 16);
    // title may be longer; truncate to fit
    String title = lastNotifTitle;
    if (title.length() > 28) title = title.substring(0, 28) + "...";
    display.println(title);
  }
  display.display();
}

void drawTimer() {
  display.clearDisplay();

  // draw timer icon at left and time text to the right
  drawIconAt(0, 20, bmp_timer, 16, 16);

  display.setTextSize(2);
  display.setCursor(20, 20);
  int m = timerSecondsLeft / 60;
  int s = timerSecondsLeft % 60;
  display.print(m);
  display.print(":");
  if (s < 10) display.print("0");
  display.print(s);

  // show active/inactive small text below
  display.setTextSize(1);
  display.setCursor(20, 46);
  if (timerActive) display.println("Running");
  else display.println("Stopped");

  display.display();
}