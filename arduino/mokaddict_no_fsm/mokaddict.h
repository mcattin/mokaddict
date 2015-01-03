/**************************************************************************
 * Types definition
 **************************************************************************/

typedef enum {
  MAIN_IDLE,
  SEND_UID,
  AUTH_USER,
  REJECTED,
  MAKE_COFFEE,
  BILL,
  RESET
} mainStates_t;

/*
typedef enum {
  COFFEE_IDLE,
  COFFEE_1,
  COFFEE_2,
  COFFEE_3,
  COFFEE_4,
  COFFEE_END,
  COFFEE_RESET
} coffeeStates_t;
*/

typedef enum {
  M1_LED,
  M2_LED
} leds_t;

typedef enum {
  LED_OFF,
  LED_RED,
  LED_GREEN
} ledStates_t;

/**************************************************************************
 * Constants
 **************************************************************************/
// Buffer
const int timeBufferSize = 12;

// Time
const unsigned long ledBlinkInterval = 200;
const unsigned long rejectedInterval = 4000;
const unsigned long rfidWaitTimeout = 8000;
const unsigned long bridgeTimeout = 3000;
const unsigned long coffeeWaitTimeout = 30000;
const unsigned long coffeeWaitLongTimeout = 180000; // 3 minutes

const unsigned long coffee_1_min = 10000;//10000;
const unsigned long coffee_2_min = 5400;//5400;
const unsigned long coffee_2_max = 6400;//6400;
const unsigned long coffee_3_min = 1000;//1000
const unsigned long coffee_3_max = 1500;//1500;
const unsigned long coffee_4_min = 2500;//2500;
const unsigned long coffee_4_max = 3000;//3000;
/*
//======================================
// Values for manual test
const unsigned long coffee_1_min = 2000;
const unsigned long coffee_2_min = 2000;
const unsigned long coffee_2_max = 5000;
const unsigned long coffee_3_min = 2000;
const unsigned long coffee_3_max = 5000;
const unsigned long coffee_4_min = 2000;
const unsigned long coffee_4_max = 5000;
//======================================
*/
