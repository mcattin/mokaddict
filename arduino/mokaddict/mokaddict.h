/**************************************************************************
 * Types definition
 **************************************************************************/

typedef enum {
  MAIN_IDLE,
  SEND_UID,
  AUTH_USER,
  REJECTED,
  MAKE_COFFEE,
  BILL
} mainStates_t;

typedef enum {
  COFFEE_IDLE,
  COFFEE_1,
  COFFEE_2,
  COFFEE_3,
  COFFEE_4,
  COFFEE_END,
  COFFEE_RESET
} coffeeStates_t;

typedef enum {
  M1_LED,
  M2_LED
} leds_t;

typedef enum {
  LED_OFF,
  LED_RED,
  LED_GREEN
} ledStates_t;

