#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "EEPROM.h"

#define EEPROM_SIZE 48
int pulse_address = 0;
int in_address = 4;
int out_address = 8;
int delay_address = 12;
int start_address = 16;
int end_address = 20;
int photo_address = 24;

int in_suction_address = 28;
int out_suction_address = 32;
int delay_suction_address = 36;
int start_suction_address = 40;
int end_suction_address = 44;

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define up_sw 4    // Pin for up button
#define down_sw 5  // Pin for down button
#define menu_sw 15 // Pin for menu button

// Check values every 200 milliseconds
#define UPDATE_TIME 400

#define hall_pin 33             // 19
#define photo_pin 34            // 18
#define solenoid_pin 25         // 2
#define suction_solenoid_pin 26 // 27

int pulse = 0;
bool pulse_flag = false;

bool photo = 0;
bool photo_flag = false;

bool solenoid_flag = false;
bool suction_solenoid_flag = false;

bool downflag = LOW;
bool upflag = LOW;
bool menuflag = LOW;

bool start_cut = LOW;
bool end_cut = LOW;

int in_count = 0;
int out_count = 0;
int cutter_delay = 0;
int menu_count = 0;

/***************SUCTION**********************/
bool start_suction = LOW;
bool end_suction = LOW;

int in_count_suction = 0;
int out_count_suction = 0;
int suction_delay = 0;
/***************SUCTION**********************/

int up = 0;
int down = 0;
int menu = 0;

void IRAM_ATTR pulseCOUNT()
{
  pulse_flag = true;
}

// Function declarations (prototypes)
void oled_display();
void oled_display_1();
void button_function();
void photo_hall_function();
void cutter_function();
void suction_function();
void pinmode_function();
void eeprom_function();
void oled_init_function();

void setup()
{
  
  // Serial.begin(115200);

  pinmode_function();
  eeprom_function();
  oled_init_function();

}
void loop()
{

  button_function();
  photo_hall_function();
  cutter_function();
  suction_function();

}

// Function definitions
void oled_init_function()
{

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  oled.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  oled.clearDisplay();
}

void eeprom_function()
{

  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  // EEPROM.writeInt(pulse_address, 0);
  // EEPROM.writeInt(in_address, 0);
  // EEPROM.writeInt(out_address, 0);
  // EEPROM.writeInt(delay_address, 0);
  // EEPROM.writeInt(start_address, 0);
  // EEPROM.writeInt(end_address, 0);
  // EEPROM.writeInt(photo_address, 0);

  // EEPROM.writeInt(in_suction_address, 0);
  // EEPROM.writeInt(out_suction_address, 0);
  // EEPROM.writeInt(delay_suction_address, 0);
  // EEPROM.writeInt(start_suction_address, 0);
  // EEPROM.writeInt(end_suction_address, 0);

  // EEPROM.commit();

  pulse = EEPROM.readInt(pulse_address);
  in_count = EEPROM.readInt(in_address);
  out_count = EEPROM.readInt(out_address);
  cutter_delay = EEPROM.readInt(delay_address);
  start_cut = EEPROM.readInt(start_address);
  end_cut = EEPROM.readInt(end_address);
  photo_flag = EEPROM.readInt(photo_address);

  in_count_suction = EEPROM.readInt(in_suction_address);
  out_count_suction = EEPROM.readInt(out_suction_address);
  suction_delay = EEPROM.readInt(delay_suction_address);
  start_suction = EEPROM.readInt(start_suction_address);
  end_suction = EEPROM.readInt(end_suction_address);
}

void pinmode_function()
{

  pinMode(up_sw, INPUT);
  pinMode(down_sw, INPUT);
  pinMode(menu_sw, INPUT);
  pinMode(solenoid_pin, OUTPUT);
  pinMode(suction_solenoid_pin, OUTPUT);
  pinMode(hall_pin, INPUT);
  pinMode(photo_pin, INPUT);

  attachInterrupt(hall_pin, pulseCOUNT, RISING);
}

void suction_function()
{

  if (pulse == in_count_suction && suction_solenoid_flag == false && start_suction == HIGH && photo_flag == true)
  {
    suction_solenoid_flag = true;
    digitalWrite(suction_solenoid_pin, HIGH);
    delay(suction_delay);
    digitalWrite(suction_solenoid_pin, LOW);
    delay(2);
  }

  if (pulse == out_count_suction && suction_solenoid_flag == false && end_suction == HIGH && photo_flag == false)
  {
    suction_solenoid_flag = true;
    digitalWrite(suction_solenoid_pin, HIGH);
    delay(suction_delay);
    digitalWrite(suction_solenoid_pin, LOW);
    delay(2);
  }

  if (pulse == 0)
  {
    suction_solenoid_flag = false;
  }
}

void cutter_function()
{

  if (pulse == in_count && solenoid_flag == false && start_cut == HIGH && photo_flag == true)
  {
    solenoid_flag = true;
    digitalWrite(solenoid_pin, HIGH);
    delay(cutter_delay);
    digitalWrite(solenoid_pin, LOW);
    delay(2);
  }

  if (pulse == out_count && solenoid_flag == false && end_cut == HIGH && photo_flag == false)
  {
    solenoid_flag = true;
    digitalWrite(solenoid_pin, HIGH);
    delay(cutter_delay);
    digitalWrite(solenoid_pin, LOW);
    delay(2);
  }

  if (pulse == 0)
  {
    solenoid_flag = false;
  }
}

void photo_hall_function()
{

  photo = digitalRead(photo_pin); // read the input pin
  if (pulse_flag == true)
  {
    pulse_flag = false;
    pulse++;
    EEPROM.writeInt(pulse_address, pulse); // -2^31
    EEPROM.commit();

    // Serial.print(pulse);
    // Serial.println(" pulse");
  }
  if (photo == 1 && photo_flag == false)
  {
    photo_flag = true;

    EEPROM.writeInt(photo_address, photo_flag); // -2^31
    EEPROM.commit();

    pulse = 0;
    // Serial.println("object detected");
  }
  if (photo == 0 && photo_flag == true)
  {
    photo_flag = false;

    EEPROM.writeInt(photo_address, photo_flag); // -2^31
    EEPROM.commit();

    pulse = 0;
    // Serial.println("object not detected");
  }
}

void button_function()
{

  int up = digitalRead(up_sw);
  int down = digitalRead(down_sw);
  int menu = digitalRead(menu_sw);

  if (menu == LOW && menuflag == LOW)
  {

    menu_count++;
    if (menu_count >= 11)
      menu_count = 0;

    menuflag = HIGH;
  }
  if (menu == HIGH)
    menuflag = LOW;

  switch (menu_count)
  {
  case 0:
    oled_display();
    break;
  case 1:
    oled_display();

    if (up == LOW && upflag == LOW)
    {

      in_count++;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      in_count--;
      if (in_count == -1)
        in_count = 0;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(in_address, in_count); // -2^31
    EEPROM.commit();

    static unsigned long last1 = millis();

    if ((millis() - last1) > UPDATE_TIME)
    {
      last1 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(80, 5); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;
  case 2:
    oled_display();
    if (up == LOW && upflag == LOW)
    {

      out_count++;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      out_count--;
      if (out_count == -1)
        out_count = 0;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(out_address, out_count); // -2^31
    EEPROM.commit();

    static unsigned long last2 = millis();

    if ((millis() - last2) > UPDATE_TIME)
    {
      last2 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(80, 17); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;
  case 3:
    oled_display();
    if (up == LOW && upflag == LOW)
    {

      cutter_delay = cutter_delay + 50;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      cutter_delay = cutter_delay - 50;
      if (cutter_delay == -50)
        cutter_delay = 0;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(delay_address, cutter_delay); // -2^31
    EEPROM.commit();

    static unsigned long last3 = millis();

    if ((millis() - last3) > UPDATE_TIME)
    {
      last3 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(80, 29); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;

  case 4:
    oled_display();
    if (up == LOW && upflag == LOW)
    {

      start_cut = HIGH;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      start_cut = LOW;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(start_address, start_cut); // -2^31
    EEPROM.commit();

    static unsigned long last4 = millis();

    if ((millis() - last4) > UPDATE_TIME)
    {
      last4 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(80, 41); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;

  case 5:
    oled_display();
    if (up == LOW && upflag == LOW)
    {

      end_cut = HIGH;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      end_cut = LOW;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(end_address, end_cut); // -2^31
    EEPROM.commit();

    static unsigned long last5 = millis();

    if ((millis() - last5) > UPDATE_TIME)
    {
      last5 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(80, 53); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;

  case 6:
    oled_display_1();
    if (up == LOW && upflag == LOW)
    {

      in_count_suction++;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      in_count_suction--;
      if (in_count_suction == -1)
        in_count_suction = 0;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(in_suction_address, in_count_suction);
    EEPROM.commit();

    static unsigned long last6 = millis();

    if ((millis() - last6) > UPDATE_TIME)
    {
      last6 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(96, 5); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;

  case 7:
    oled_display_1();

    if (up == LOW && upflag == LOW)
    {

      out_count_suction++;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      out_count_suction--;
      if (out_count_suction == -1)
        out_count_suction = 0;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(out_suction_address, out_count_suction); // -2^31
    EEPROM.commit();

    static unsigned long last7 = millis();

    if ((millis() - last7) > UPDATE_TIME)
    {
      last7 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(96, 17); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;

  case 8:
    oled_display_1();

    if (up == LOW && upflag == LOW)
    {

      suction_delay = suction_delay + 100;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      suction_delay = suction_delay - 100;
      if (suction_delay == -100)
        suction_delay = 0;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(delay_suction_address, suction_delay); // -2^31
    EEPROM.commit();

    static unsigned long last8 = millis();

    if ((millis() - last8) > UPDATE_TIME)
    {
      last8 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(80, 29); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;

  case 9:
    oled_display_1();

    if (up == LOW && upflag == LOW)
    {

      start_suction = HIGH;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      start_suction = LOW;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(start_suction_address, start_suction); // -2^31
    EEPROM.commit();

    static unsigned long last9 = millis();

    if ((millis() - last9) > UPDATE_TIME)
    {
      last9 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(80, 41); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;

  case 10:
    oled_display_1();

    if (up == LOW && upflag == LOW)
    {

      end_suction = HIGH;

      upflag = HIGH;
    }
    if (up == HIGH)
      upflag = LOW;

    if (down == LOW && downflag == LOW)
    {

      end_suction = LOW;

      downflag = HIGH;
    }
    if (down == HIGH)
      downflag = LOW;

    EEPROM.writeInt(end_suction_address, end_suction); // -2^31
    EEPROM.commit();

    static unsigned long last10 = millis();

    if ((millis() - last10) > UPDATE_TIME)
    {
      last10 = millis();
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(80, 53); // x y set position to display
      oled.print("_____");
      oled.display();
      delay(2);
    }

    break;
  }
}

void oled_display()
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(1);

  oled.setCursor(2, 4); // x y set position to display
  oled.print("START COUNT - ");
  oled.print(in_count);

  oled.setCursor(2, 16); // x y set position to display
  oled.print("END COUNT   - ");
  oled.print(out_count);

  oled.setCursor(2, 28); // x y set position to display
  oled.print("CUT DELAY   - ");
  oled.print(cutter_delay);
  oled.print(" ms");

  oled.setCursor(2, 40); // x y set position to display
  oled.print("START CUT   - ");
  if (start_cut == LOW)
    oled.print("OFF");
  if (start_cut == HIGH)
    oled.print("ON");

  oled.setCursor(2, 52); // x y set position to display
  oled.print("END CUT     - ");
  if (end_cut == LOW)
    oled.print("OFF");
  if (end_cut == HIGH)
    oled.print("ON");

  oled.display();
  delay(1);
}

void oled_display_1()
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(1);

  oled.setCursor(2, 4); // x y set position to display
  oled.print("SUC START COUNT- ");
  oled.print(in_count_suction);

  oled.setCursor(2, 16); // x y set position to display
  oled.print("SUC END COUNT  - ");
  oled.print(out_count_suction);

  oled.setCursor(2, 28); // x y set position to display
  oled.print("SUC DELAY   - ");
  oled.print(suction_delay);
  oled.print(" ms");

  oled.setCursor(2, 40); // x y set position to display
  oled.print("SUC START   - ");
  if (start_suction == LOW)
    oled.print("OFF");
  if (start_suction == HIGH)
    oled.print("ON");

  oled.setCursor(2, 52); // x y set position to display
  oled.print("SUC END     - ");
  if (end_suction == LOW)
    oled.print("OFF");
  if (end_suction == HIGH)
    oled.print("ON");

  oled.display();
  delay(1);
}