/**************************************************************************/
/*! 
    @file     mokaddict.ino
    @author   Matthieu Cattin
    @license  GPL V3

    TODO : add some description here.    
*/
/**************************************************************************/
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <Bridge.h>
#include <Process.h>
#include "mokaddict.h"

/**************************************************************************
 * Pin assigment
 **************************************************************************/
const int nfcIrqPin = 6;
const int nfcRstPin = 3; // Not connected by default on the NFC Shield

const int ledPin1A =  A2;
const int ledPin1B =  A3;
const int ledPin2A =  A0;
const int ledPin2B =  A1;
const int buzPin = 5;
const int m1CtrlPin = 12;
const int m2CtrlPin = 4;
const int m1StatPin = 8;
const int m2StatPin = 9;
const int b1Pin = 11;
const int b2Pin = 10;

/**************************************************************************
 * Buzzer
 **************************************************************************/
const long buzFreq = 1568;
const long buzDuration = 100;

/**************************************************************************
 * NFC shield
 **************************************************************************/
Adafruit_NFCShield_I2C nfc(nfcIrqPin, nfcRstPin);

void initNfcShield(){
  nfc.begin();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(F("Didn't find PN53x board"));
    setLed(M1_LED, LED_RED);
    setLed(M1_LED, LED_RED);
    while (1); // halt
  }
  Serial.print(F("Found chip PN5")); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print(F("Firmware ver. ")); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print(F(".")); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  nfc.SAMConfig(); // configure board to read RFID tags
}

String readUid(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  String rfid_uid = "";
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println(F("Found an ISO14443A card"));
    Serial.print(F("  UID Length: "));Serial.print(uidLength, DEC);Serial.println(F(" bytes"));
    
    if (uidLength == 4 || uidLength == 7)
    {
      rfid_uid = uid2string(uid, uidLength);
      Serial.print(F("  UID Value: ")); Serial.println(rfid_uid);
    }
  }
  /*
  else {
     Serial.println(F("Ooops ... failed to read the card!"));
     setLed(M1_LED, LED_RED);
     setLed(M1_LED, LED_RED);
  }
  */
  return rfid_uid;
}

/**************************************************************************
 * Main state machine
 **************************************************************************/
mainStates_t m1MainState, m2MainState = MAIN_IDLE;

/**************************************************************************
 * Coffee machine state machine
 **************************************************************************/
 coffeeStates_t m1CoffeeState, m2CoffeeState = COFFEE_IDLE;

/**************************************************************************
 * Time management
 **************************************************************************/
unsigned long m1PreviousMillis, m2PreviousMillis = 0;
unsigned long m1LedPreviousMillis, m2LedPreviousMillis = 0;

/**************************************************************************
 * LEDs
 **************************************************************************/
ledStates_t m1LedState, m2LedState = LED_OFF;

bool m1LedBlink, m2LedBlink = false;

void setLed(leds_t led, ledStates_t ledState){
  switch(ledState){
    case LED_OFF:
      if(led == M1_LED){
        //Serial.println("M1 LED OFF");
        digitalWrite(ledPin1A, LOW);
        digitalWrite(ledPin1B, LOW);
      }
      if(led == M2_LED){
        digitalWrite(ledPin2A, LOW);
        digitalWrite(ledPin2B, LOW);
      }
      break;
    case LED_RED:
      if(led == M1_LED){
        //Serial.println("M1 LED RED");
        digitalWrite(ledPin1A, LOW);
        digitalWrite(ledPin1B, HIGH);
      }
      if(led == M2_LED){
        digitalWrite(ledPin2A, LOW);
        digitalWrite(ledPin2B, HIGH);
      }
      break;
    case LED_GREEN:
      if(led == M1_LED){
        //Serial.println("M1 LED GREEN");
        digitalWrite(ledPin1A, HIGH);
        digitalWrite(ledPin1B, LOW);
      }
      if(led == M2_LED){
        digitalWrite(ledPin2A, HIGH);
        digitalWrite(ledPin2B, LOW);
      }
      break;
    default:
      digitalWrite(ledPin1A, LOW);
      digitalWrite(ledPin1B, LOW);
      digitalWrite(ledPin2A, LOW);
      digitalWrite(ledPin2B, LOW);
      break;
  }
}

/**************************************************************************
 * Inputs, pin change interrupts (buttons and machines status)
 **************************************************************************/
 typedef struct inputState{
  int m1Stat;
  int m2Stat;
  int b1;
  int b2;
}inputState;

inputState newInputState;
inputState oldInputState;

int m1StatCnt, m2StatCnt, b1Cnt, b2Cnt = 0;

void InitPinChangeInterrupt(){
  noInterrupts();
  PCMSK0 = 0xF0;
  PCICR = 0x1;
  interrupts();
}
/*
void print_pcicfg(){
  Serial.print("PCMSK0: 0x");Serial.println(PCMSK0, HEX);
  Serial.print("PCICR : 0x");Serial.println(PCICR, HEX);
  Serial.print("PCIFR : 0x");Serial.println(PCIFR, HEX);   
}
*/
void readInputState(){
  newInputState.m1Stat = digitalRead(m1StatPin);
  newInputState.m2Stat = digitalRead(m2StatPin);
  newInputState.b1 = digitalRead(b1Pin);
  newInputState.b2 = digitalRead(b2Pin);
}

ISR(PCINT0_vect){
  readInputState();
  
  if(newInputState.b1 != oldInputState.b1){
    b1Cnt++;
    oldInputState.b1 = newInputState.b1;
    //Serial.print("Button1 cnt: ");Serial.println(b1Cnt, DEC);
  }
  
  if(newInputState.b2 != oldInputState.b2){
    b2Cnt++;
    oldInputState.b2 = newInputState.b2;
    //Serial.print("Button2 cnt: ");Serial.println(b2Cnt, DEC);
  }
  
  if(newInputState.m1Stat != oldInputState.m1Stat){
    m1StatCnt++;
    oldInputState.m1Stat = newInputState.m1Stat;
    //Serial.print("Machine1 cnt: ");Serial.println(m1StatCnt, DEC);
  }
  
  if(newInputState.m2Stat != oldInputState.m2Stat){
    m2StatCnt++;
    oldInputState.m2Stat = newInputState.m2Stat;
    //Serial.print("Machine2 cnt: ");Serial.println(m2StatCnt, DEC);
  }
}

/**************************************************************************
 * arduino setup
 **************************************************************************/
void setup(void) {

  // Set pins direction
  pinMode(ledPin1A, OUTPUT);
  pinMode(ledPin1B, OUTPUT);
  pinMode(ledPin2A, OUTPUT);
  pinMode(ledPin2B, OUTPUT);
  pinMode(m1CtrlPin, OUTPUT);
  pinMode(m2CtrlPin, OUTPUT);
  pinMode(m1StatPin, INPUT);
  pinMode(m2StatPin, INPUT);
  pinMode(b1Pin, INPUT);
  pinMode(b2Pin, INPUT);
  pinMode(buzPin, OUTPUT);
  
  digitalWrite(m1CtrlPin, LOW);
  digitalWrite(m2CtrlPin, LOW);
  
  digitalWrite(buzPin, LOW);
  
  //tone(buzPin, buzFreq, buzDuration);
  setLed(M1_LED, LED_RED);
  setLed(M2_LED, LED_RED);
  delay(1000);
  //tone(buzPin, buzFreq, buzDuration);
  setLed(M1_LED, LED_GREEN);
  setLed(M2_LED, LED_GREEN);
  delay(1000);
  //tone(buzPin, buzFreq, buzDuration);
  setLed(M1_LED, LED_OFF);
  setLed(M2_LED, LED_OFF);
  
  // Setup serial port
  Serial.begin(115200);
  Serial.println(F("Welcome to MokAddict!"));
  
  // Setup communication with Atheros AR9331
  Bridge.begin();
  Serial.println(F("Linux ready"));

  // Setup NFC shield
  initNfcShield();
  
  // Initialise input states
  readInputState();
  oldInputState = newInputState;
  
  // Initialise pin change interrupts
  InitPinChangeInterrupt();
  
}

/**************************************************************************
 * arduino main loop
 **************************************************************************/
void loop(void) {
  static bool rfid_reader_token = false;
  static bool m1CycleStarted, m2CycleStarted = false;
  static String m1Uid, m2Uid = "";
  unsigned long m1DiffMillis, m2DiffMillis = 0;
  
  unsigned long currentMillis = millis();
  m1DiffMillis = currentMillis - m1PreviousMillis;
  m2DiffMillis = currentMillis - m2PreviousMillis;
  
  /*-------------------------------------------------------------------------
   - Machine A
   -------------------------------------------------------------------------*/
  switch(m1MainState){
    
    case MAIN_IDLE:
    
      // Button has been pressed and released  
      if(1 <= b1Cnt){
        if(1 == newInputState.b1 && false == rfid_reader_token){
          rfid_reader_token = true;
          b1Cnt = 0;
          m1PreviousMillis = currentMillis;
          Serial.println(F("m1 state: AUTH_USER"));
          m1MainState = AUTH_USER;
        }
        if(0 == newInputState.b1 && 0 == newInputState.b2 && false == rfid_reader_token && MAIN_IDLE == m2MainState){
          rfid_reader_token = true;
          b1Cnt = 0;
          m1PreviousMillis = currentMillis;
          Serial.println(F("m1 state: SEND_UID"));
          m1MainState = SEND_UID;
        }
        else{
          m1PreviousMillis = currentMillis;
          b1Cnt = 0;
        }
      }
      break;
      
    case SEND_UID:
    
      // Make LED blink red/green
      if(currentMillis - m1LedPreviousMillis >= ledBlinkInterval) {
        m1LedPreviousMillis = currentMillis;
        if(m1LedBlink){
          m1LedBlink = false;
          setLed(M1_LED, LED_GREEN);
          setLed(M2_LED, LED_RED);
        }
        else{
          m1LedBlink = true;
          setLed(M1_LED, LED_RED);
          setLed(M2_LED, LED_GREEN);
        }
      }
    
      // Timeout if no card detected
      if(m1DiffMillis > rfidWaitTimeout){
        rfid_reader_token = false;
        setLed(M1_LED, LED_OFF);
        setLed(M2_LED, LED_OFF);
        b1Cnt = 0;
        Serial.println(F("m1: TIMEOUT"));
        Serial.println(F("m1 state: MAIN_IDLE"));
        m1PreviousMillis = currentMillis;
        m1MainState = MAIN_IDLE;
      }
      else{
        // Tries to read card's UID
        m1Uid = readUid();
        
        // A card has been detected
        if(m1Uid != ""){
          tone(buzPin, buzFreq, buzDuration);
          if(send_uid(m1Uid)){
            rfid_reader_token = false;
            setLed(M1_LED, LED_OFF);
            setLed(M2_LED, LED_OFF);
            b1Cnt = 0;
            Serial.println(F("m1 state: MAIN_IDLE"));
            m1PreviousMillis = currentMillis;
            m1MainState = MAIN_IDLE;
          }
        }
      }
      break;
      
    case AUTH_USER:
    
      // Make LED blink red/green
      if(currentMillis - m1LedPreviousMillis >= ledBlinkInterval) {
        m1LedPreviousMillis = currentMillis;
        if(m1LedBlink){
          m1LedBlink = false;
          setLed(M1_LED, LED_GREEN);
        }
        else{
          m1LedBlink = true;
          setLed(M1_LED, LED_RED);
        }
      }
    
      // Timeout if no card detected
      if(m1DiffMillis > rfidWaitTimeout){
        rfid_reader_token = false;
        setLed(M1_LED, LED_OFF);
        b1Cnt = 0;
        Serial.println(F("m1: TIMEOUT"));
        Serial.println(F("m1 state: MAIN_IDLE"));
        m1MainState = MAIN_IDLE;
      }
      else{
        // Tries to read card's UID
        m1Uid = readUid();
        
        // A card has been detected
        if(m1Uid != ""){
          rfid_reader_token = false;
          setLed(M1_LED, LED_OFF);
          tone(buzPin, buzFreq, buzDuration);
          
          // Check if the user is registered
          if(check_rfid_uid(m1Uid)){
            // User is registered
            Serial.println(F("m1 state: MAKE_COFFEE"));
            m1StatCnt = 0;
            m1CycleStarted = false;
            m1CoffeeState = COFFEE_IDLE;
            setLed(M1_LED, LED_GREEN);
            digitalWrite(m1CtrlPin, HIGH); // Enable machine's buttons
            m1PreviousMillis = currentMillis;
            m1MainState = MAKE_COFFEE;
          }
          else{
            // User is not registered
            setLed(M1_LED, LED_RED);
            log_user(m1Uid, "FALSE");  // Log user's UID, with authenticated=FALSE
            Serial.println(F("m1 state: REJECTED"));
            m1PreviousMillis = currentMillis;
            m1MainState = REJECTED;
          }
        }
      }
      break;
      
    case REJECTED:
    
      // LED in red for N seconds
      if(m1DiffMillis >= rejectedInterval) {
        setLed(M1_LED, LED_OFF);
        b1Cnt = 0;
        Serial.println(F("m1 state: MAIN_IDLE"));
        m1MainState = MAIN_IDLE;
      }
      break;
      
    case MAKE_COFFEE:
    
      // Timeout if no coffee is made (bigger timeout if cycle as strated)
      if((m1DiffMillis > coffeeWaitTimeout && false == m1CycleStarted) ||
        (m1DiffMillis > (coffeeWaitLongTimeout) && true == m1CycleStarted)){
        digitalWrite(m1CtrlPin, LOW);  // Disable machine's buttons
        setLed(M1_LED, LED_OFF);
        b1Cnt = 0;
        Serial.print(F("m1: TIMEOUT: "));Serial.println(m1DiffMillis, DEC);
        Serial.println(F("m1 state: MAIN_IDLE"));
        m1PreviousMillis = currentMillis;
        m1MainState = MAIN_IDLE;
      }
      else{
    
        switch(m1CoffeeState){
      
          case COFFEE_IDLE:
            
            if(1 <= m1StatCnt){
              if(0 == newInputState.m1Stat){
                m1CycleStarted = true;
                m1PreviousMillis = currentMillis;
                Serial.print("m1: COFFEE_1  ");Serial.println(m1DiffMillis, DEC);
                m1CoffeeState = COFFEE_1;
              }
              else{
                Serial.println("m1: COFFEE_RESET");
                m1CoffeeState = COFFEE_RESET;
              }
            }
            break;
          
          case COFFEE_1:
            
            if((2 == m1StatCnt) && (1 == newInputState.m1Stat)){
              if(m1DiffMillis > coffee_1_min){
                m1PreviousMillis = currentMillis;
                Serial.print("m1: COFFEE_2  ");Serial.println(m1DiffMillis, DEC);
                m1CoffeeState = COFFEE_2;
              }
              else{
                Serial.println("m1: COFFEE_RESET");
                m1CoffeeState = COFFEE_RESET;
              }
            }
            
            break;
          
          case COFFEE_2:
            
            if((3 == m1StatCnt) && (0 == newInputState.m1Stat)){
              if((m1DiffMillis > coffee_2_min) && (m1DiffMillis < coffee_2_max)){
                m1PreviousMillis = currentMillis;
                Serial.print("m1: COFFEE_3  ");Serial.println(m1DiffMillis, DEC);
                m1CoffeeState = COFFEE_3;
              }
              else{
                Serial.println("m1: COFFEE_RESET");
                m1CoffeeState = COFFEE_RESET;
              }
            }
            break;
          
          case COFFEE_3:
            
            if((4 == m1StatCnt) && (1 == newInputState.m1Stat)){
              if((m1DiffMillis > coffee_3_min) && (m1DiffMillis < coffee_3_max)){
                m1PreviousMillis = currentMillis;
                Serial.print("m1: COFFEE_4  ");Serial.println(m1DiffMillis, DEC);
                m1CoffeeState = COFFEE_4;
              }
              else{
                Serial.println("m1: COFFEE_RESET");
                m1CoffeeState = COFFEE_RESET;
              }
            }
            break;
          
          case COFFEE_4:
            
            if((5 == m1StatCnt) && (0 == newInputState.m1Stat)){
              if((m1DiffMillis > coffee_4_min) && (m1DiffMillis < coffee_4_max)){
                m1PreviousMillis = currentMillis;
                Serial.print("m1: COFFEE_END  ");Serial.println(m1DiffMillis, DEC);
                m1CoffeeState = COFFEE_END;
              }
              else{
                Serial.println("m1: COFFEE_RESET");
                m1CoffeeState = COFFEE_RESET;
              }
            }
            break;
          
          case COFFEE_END:
            
            Serial.println("m1 state: BILL");
            m1MainState = BILL;
            break;
          
          case COFFEE_RESET:
            
            Serial.print("m1: COFFEE_IDLE  ");Serial.println(m1DiffMillis, DEC);
            m1StatCnt = 0;
            m1PreviousMillis = currentMillis;
            m1CoffeeState = COFFEE_IDLE;
            break;
            
          default:
          
            m1CoffeeState = COFFEE_RESET;
            break;
          }
        }
        break;
        
      case BILL:
      
        b1Cnt = 0;
        m1StatCnt = 0;
        digitalWrite(m1CtrlPin, LOW);  // Disable machine's buttons
        log_user(m1Uid, "TRUE");  // Log user's UID, with authenticated=TRUE
        setLed(M1_LED, LED_OFF);
        Serial.println(F("m1 state: MAIN_IDLE"));
        m1PreviousMillis = currentMillis;
        m1MainState = MAIN_IDLE;
        break;
        
      default:
      
        m1MainState = MAIN_IDLE;
        break;
  }
  
  /*-------------------------------------------------------------------------
   - Machine B
   -------------------------------------------------------------------------*/
  switch(m2MainState){
    
    case MAIN_IDLE:
    
      // Button has been pressed and released  
      if(1 <= b2Cnt){
        if(1 == newInputState.b2 && false == rfid_reader_token && SEND_UID != m1MainState){
          rfid_reader_token = true;
          b2Cnt = 0;
          m2PreviousMillis = currentMillis;
          Serial.println(F("m2 state: AUTH_USER"));
          m2MainState = AUTH_USER;
        }
        else{
          b2Cnt = 0;
        }
      }
      break;
      
    case AUTH_USER:
    
      // Make LED blink red/green
      if(currentMillis - m2LedPreviousMillis >= ledBlinkInterval) {
        m2LedPreviousMillis = currentMillis;
        if(m2LedBlink){
          m2LedBlink = false;
          setLed(M2_LED, LED_GREEN);
        }
        else{
          m2LedBlink = true;
          setLed(M2_LED, LED_RED);
        }
      }
    
      // Timeout if no card detected
      if(m2DiffMillis > rfidWaitTimeout){
        rfid_reader_token = false;
        setLed(M2_LED, LED_OFF);
        b2Cnt = 0;
        Serial.println(F("m2: TIMEOUT"));
        Serial.println(F("m2 state: MAIN_IDLE"));
        m2MainState = MAIN_IDLE;
      }
      else{
        // Tries to read card's UID
        m2Uid = readUid();
        
        // A card has been detected
        if(m2Uid != ""){
          rfid_reader_token = false;
          setLed(M2_LED, LED_OFF);
          tone(buzPin, buzFreq, buzDuration);
          
          // Check if the user is registered
          if(check_rfid_uid(m2Uid)){
            // User is registered
            Serial.println(F("m2 state: MAKE_COFFEE"));
            m2StatCnt = 0;
            m2CycleStarted = false;
            m2CoffeeState = COFFEE_IDLE;
            setLed(M2_LED, LED_GREEN);
            digitalWrite(m2CtrlPin, HIGH); // Enable machine's buttons
            m2PreviousMillis = currentMillis;
            m2MainState = MAKE_COFFEE;
          }
          else{
            // User is not registered
            setLed(M2_LED, LED_RED);
            log_user(m2Uid, "FALSE");  // Log user's UID, with authenticated=FALSE
            Serial.println(F("m2 state: REJECTED"));
            m2PreviousMillis = currentMillis;
            m2MainState = REJECTED;
          }
        }
      }
      break;
      
    case REJECTED:
    
      // LED in red for N seconds
      if(m2DiffMillis >= rejectedInterval) {
        setLed(M2_LED, LED_OFF);
        b2Cnt = 0;
        Serial.println(F("m2 state: MAIN_IDLE"));
        m2MainState = MAIN_IDLE;
      }
      break;
      
    case MAKE_COFFEE:
      // Timeout if no coffee is made (bigger timeout if cycle as strated)
      if((m2DiffMillis > coffeeWaitTimeout && false == m2CycleStarted) ||
        (m2DiffMillis > (coffeeWaitLongTimeout) && true == m2CycleStarted)){
        digitalWrite(m2CtrlPin, LOW);  // Disable machine's buttons
        setLed(M2_LED, LED_OFF);
        b2Cnt = 0;
        Serial.print(F("m2: TIMEOUT: "));Serial.println(m2DiffMillis, DEC);
        Serial.println(F("m2 state: MAIN_IDLE"));
        m2PreviousMillis = currentMillis;
        m2MainState = MAIN_IDLE;
      }
      else{
    
        switch(m2CoffeeState){
      
          case COFFEE_IDLE:
            
            if(1 <= m2StatCnt){
              if(0 == newInputState.m2Stat){
                m2CycleStarted = true;
                m2PreviousMillis = currentMillis;
                Serial.print("m2: COFFEE_1  ");Serial.println(m2DiffMillis, DEC);
                m2CoffeeState = COFFEE_1;
              }
              else{
                Serial.println("m2: COFFEE_RESET");
                m2CoffeeState = COFFEE_RESET;
              }
            }
            break;
          
          case COFFEE_1:
            
            if((2 == m2StatCnt) && (1 == newInputState.m2Stat)){
              if(m2DiffMillis > coffee_1_min){
                m2PreviousMillis = currentMillis;
                Serial.print("m2: COFFEE_2  ");Serial.println(m2DiffMillis, DEC);
                m2CoffeeState = COFFEE_2;
              }
              else{
                Serial.println("m2: COFFEE_RESET");
                m2CoffeeState = COFFEE_RESET;
              }
            }
            
            break;
          
          case COFFEE_2:
            
            if((3 == m2StatCnt) && (0 == newInputState.m2Stat)){
              if((m2DiffMillis > coffee_2_min) && (m2DiffMillis < coffee_2_max)){
                m2PreviousMillis = currentMillis;
                Serial.print("m2: COFFEE_3  ");Serial.println(m2DiffMillis, DEC);
                m2CoffeeState = COFFEE_3;
              }
              else{
                Serial.println("m2: COFFEE_RESET");
                m2CoffeeState = COFFEE_RESET;
              }
            }
            break;
          
          case COFFEE_3:
            
            if((4 == m2StatCnt) && (1 == newInputState.m2Stat)){
              if((m2DiffMillis > coffee_3_min) && (m2DiffMillis < coffee_3_max)){
                m2PreviousMillis = currentMillis;
                Serial.print("m2: COFFEE_4  ");Serial.println(m2DiffMillis, DEC);
                m2CoffeeState = COFFEE_4;
              }
              else{
                Serial.println("m2: COFFEE_RESET");
                m2CoffeeState = COFFEE_RESET;
              }
            }
            break;
          
          case COFFEE_4:
            
            if((5 == m2StatCnt) && (0 == newInputState.m2Stat)){
              if((m2DiffMillis > coffee_4_min) && (m2DiffMillis < coffee_4_max)){
                m2PreviousMillis = currentMillis;
                Serial.print("m2: COFFEE_END  ");Serial.println(m2DiffMillis, DEC);
                m2CoffeeState = COFFEE_END;
              }
              else{
                Serial.println("m2: COFFEE_RESET");
                m2CoffeeState = COFFEE_RESET;
              }
            }
            break;
          
          case COFFEE_END:
            
            Serial.println("m2 state: BILL");
            m2MainState = BILL;
            break;
          
          case COFFEE_RESET:
            
            Serial.print("m2: COFFEE_IDLE  ");Serial.println(m2DiffMillis, DEC);
            m2StatCnt = 0;
            m2PreviousMillis = currentMillis;
            m2CoffeeState = COFFEE_IDLE;
            break;
            
          default:
          
            m2CoffeeState = COFFEE_RESET;
            break;
        }
      }
      break;
      
    case BILL:
    
      b2Cnt = 0;
      m2StatCnt = 0;
      digitalWrite(m2CtrlPin, LOW);  // Disable machine's buttons
      log_user(m2Uid, "TRUE");  // Log user's UID, with authenticated=TRUE
      setLed(M2_LED, LED_OFF);
      Serial.println(F("m2 state: MAIN_IDLE"));
      m2PreviousMillis = currentMillis;
      m2MainState = MAIN_IDLE;
      break;
      
    default:
    
      m2MainState = MAIN_IDLE;
      break;
  }
  
}

/**************************************************************************
 * Converts UID to string (upper case)
 **************************************************************************/
String uid2string(const uint8_t * data, const uint8_t numBytes) {
  uint8_t b;
  String ret = "";
  for(b=0; b < numBytes; b++) {
    if (data[b] <= 0xF) // Append leading 0 for small values
      ret += "0";
    ret += String(data[b]&0xFF, HEX);
  }
  ret.toUpperCase();
  return ret;
}

/**************************************************************************
 * Check if UID is in the authorised user database
 **************************************************************************/
bool check_rfid_uid(String uid){
  bool ret = false;
  Process check_user;
  
  Serial.println(F("Check UID in registered user database"));
  
  check_user.begin("python");
  check_user.addParameter("/mnt/sda1/python/check_user.py");
  check_user.addParameter(uid);
  check_user.run();
  
  while (check_user.available()){
    String py_result = check_user.readString();
    py_result.trim(); // remove end of line
    if(py_result == "1"){
      Serial.println(F("Registered user"));
      ret = true;
    }
    else if(py_result == "2"){
      Serial.println(F("Unknown user"));
      ret = false;
    }
    else {
      Serial.println(F("An error occured:"));
      Serial.print("  ");Serial.println(py_result);
      ret = false;
    }
  }
  return ret;
}

/**************************************************************************
 * Log UID in database
 **************************************************************************/
bool log_user(String uid, String auth){
  bool ret = false;
  Process log_user;
  log_user.begin("python");
  log_user.addParameter("/mnt/sda1/python/log_user.py");
  log_user.addParameter(uid);
  log_user.addParameter(auth);
  log_user.run();
  
  while (log_user.available()){
    String py_result = log_user.readString();
    py_result.trim(); // remove end of line
    if(py_result == "1"){
      Serial.println(F("Entry logged to database."));
      ret = true;
    }
    else {
      Serial.print(F("Error logging entry to database!"));
      Serial.print(F("  "));Serial.println(py_result);
      ret = false;
    }
  }
  return ret;
}

/**************************************************************************
 * Send UID to from
 **************************************************************************/
bool send_uid(String uid){
  bool ret = false;
  Process log_user;
  log_user.begin("python");
  log_user.addParameter("/mnt/sda1/python/send_uid.py");
  log_user.addParameter(uid);
  log_user.run();
  
  while (log_user.available()){
    String py_result = log_user.readString();
    py_result.trim(); // remove end of line
    if(py_result == "1"){
      Serial.println(F("UID sent to form."));
      ret = true;
    }
    else {
      Serial.println(F("Error: invalid UID."));
      ret = false;
    }
  }
  return ret;
}

