#include <Arduino.h>
#include <SPI.h>
#include <Keyboard.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

// Mode
#define BLUETOOTH 1
#define WIRED 0

// PIN I/O
#define btn_A A11
#define btn_B A0
#define btn_X A3
#define btn_Y A2

#define joy_UD A4
#define joy_LR A10

// Button Threshold
#define BTN_HIGH 1010

// Joystick Threshold values
#define T_UP 230
#define T_DOWN 860
#define T_LEFT 200
#define T_RIGHT 800

/*
#define T_UP 450
#define T_DOWN 600
#define T_LEFT 400
#define T_RIGHT 600
*/

#define delay_debounce 80
#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"


// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  // Setup pins on the breadboard
  pinMode(btn_A, INPUT);
  pinMode(btn_B, INPUT);
  pinMode(btn_X, INPUT);
  pinMode(btn_Y, INPUT);

  pinMode(joy_UD, INPUT);
  pinMode(joy_LR, INPUT);
  
  //while (!Serial);  // required for Flora & Micro
  //delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit HID Keyboard Example"));
  Serial.println(F("---------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  /* Change the device name to make it easier to find */
  Serial.println(F("Setting device name to 'Bluefruit Keyboard': "));
  if (! ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Bluefruit Keyboard" )) ) {
    error(F("Could not set device name?"));
  }

  /* Enable HID Service */
  Serial.println(F("Enable HID Service (including Keyboard): "));
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    if ( !ble.sendCommandCheckOK(F( "AT+BleHIDEn=On" ))) {
      error(F("Could not enable Keyboard"));
    }
  }else
  {
    if (! ble.sendCommandCheckOK(F( "AT+BleKeyboardEn=On"  ))) {
      error(F("Could not enable Keyboard"));
    }
  }

  /* Add or remove service requires a reset */
  Serial.println(F("Performing a SW reset (service changes require a reset): "));
  if (! ble.reset() ) {
    error(F("Couldn't reset??"));
  }

  Serial.println();
  Serial.println(F("Go to your phone's Bluetooth settings to pair your device"));
  Serial.println(F("then open an application that accepts keyboard input"));

  Serial.println();
  Serial.println(F("Enter the character(s) to send:"));
  Serial.println(F("- \\r for Enter"));
  Serial.println(F("- \\n for newline"));
  Serial.println(F("- \\t for tab"));
  Serial.println(F("- \\b for backspace"));

  Serial.println();
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  /* int mode = BLUETOOTH; */
  int mode = BLUETOOTH;
  
  if(mode == BLUETOOTH){
    // Convert button strokes into BT keyboard strokes 
    if(analogRead(btn_A) >= BTN_HIGH){
      ble.print("AT+BleKeyboard=");
      ble.println("A");
      delay(delay_debounce); // This delay is to prevent repeated button presses after being pressed once
    }
    
    if(analogRead(btn_B) >= BTN_HIGH){
      ble.print("AT+BleKeyboard=");
      ble.println("B");
      delay(delay_debounce); // This delay is to prevent repeated button presses after being pressed once
    }
    
    if(analogRead(btn_X) >= BTN_HIGH){
      ble.print("AT+BleKeyboard=");
      ble.println("X");
      delay(delay_debounce); // This delay is to prevent repeated button presses after being pressed once
    }
    
    if(analogRead(btn_Y) >= BTN_HIGH){
      ble.print("AT+BleKeyboard=");
      ble.println("Y");
      delay(delay_debounce); // This delay is to prevent repeated button presses after being pressed once
    }
    
    // Reading input of Joystick
    // Sending commands through serial if joystick is activated
    if(analogRead(joy_UD) <= T_UP){
      ble.print("AT+BleKeyboard=");
      ble.println("U");
      delay(delay_debounce); // This delay is to prevent repeated button presses after being pressed once
    }
    
    if(analogRead(joy_UD) >= T_DOWN){
      ble.print("AT+BleKeyboard=");
      ble.println("D");
      delay(delay_debounce); // This delay is to prevent repeated button presses after being pressed once
    }
    
    if(analogRead(joy_LR) <= T_LEFT){
      ble.print("AT+BleKeyboard=");
      ble.println("L");
      delay(delay_debounce); // This delay is to prevent repeated button presses after being pressed once
    }
    
    if(analogRead(joy_LR) >= T_RIGHT){
      ble.print("AT+BleKeyboard=");
      ble.println("R");
      delay(delay_debounce); // This delay is to prevent repeated button presses after being pressed once
    } 
    
    //delay(1); // The lower the delay of this, the faster the BT transmission delay
    
    /*
    // Display prompt
    Serial.print(F("keyboard > "));
  
    // Check for user input and echo it back if anything was found
    char keys[BUFSIZE+1];
    getUserInput(keys, BUFSIZE);
  
    Serial.print("\nSending ");
    Serial.println(keys);
  
    ble.print("AT+BleKeyboard=");
    ble.println(keys);
  
    if( ble.waitForOK() )
    {
      Serial.println( F("OK!") );
    }else
    {
      Serial.println( F("FAILED!") );
    }
    */
  }

  if(mode == WIRED){
    
    if(analogRead(btn_A) >= BTN_HIGH){
      Keyboard.write('A');
      delay(delay_debounce);
    }

    if(analogRead(btn_B) >= BTN_HIGH){
      Keyboard.write('B');
      delay(delay_debounce);
    }
    if(analogRead(btn_X) >= BTN_HIGH){
      Keyboard.write('X');
      delay(delay_debounce);
    }
    if(analogRead(btn_Y) >= BTN_HIGH){
      Keyboard.write('Y');
      delay(delay_debounce);
    }
    // Reading input of Joystick
    // Sending commands through serial if joystick is activated
    if(analogRead(joy_UD) <= T_UP){
      Keyboard.write('U');
      delay(delay_debounce);
    }
    if(analogRead(joy_UD) >= T_DOWN){
      Keyboard.write('D');
      delay(delay_debounce);
    }
    if(analogRead(joy_LR) <= T_LEFT){
      Keyboard.write('L');
      delay(delay_debounce);
    }
    if(analogRead(joy_LR) >= T_RIGHT){
      Keyboard.write('R');
      delay(delay_debounce);
    }
  }
    
}

/**************************************************************************/
/*!
    @brief  Checks for user input (via the Serial Monitor)
*/
/**************************************************************************/
void getUserInput(char buffer[], uint8_t maxSize)
{
  memset(buffer, 0, maxSize);
  while( Serial.available() == 0 ) {
    delay(1);
  }

  uint8_t count=0;

  do
  {
    count += Serial.readBytes(buffer+count, maxSize);
    delay(2);
  } while( (count < maxSize) && !(Serial.available() == 0) );
}
