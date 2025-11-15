/*
 *   
 *   Board used:
 *    Sparkfun Pro Micro, 5V, 16 MHz
 *    
 *  Libraries used:
 *   arduino-timer              2.0.1 https://github.com/contrem/arduino-timer
 *   ArduinoJoystickLibrary     2.0   https://github.com/MHeironimus/ArduinoJoystickLibrary/tree/version-2.0
 *   
 *    
 *  For channel / function and joystick axis / button assignment, see below in void updateRC()
 *  
 */

#include <Joystick.h>
#include <timer.h>


#define PPM_PIN 3                  /* PIN 3 used with interrupts and pullups enabled */
#define SERIAL_PRINT_INTERVAL 1000 /* delay for printing out rxChannels to the serial console, 0 for disabling prints */
#define CHANNEL_COUNT 8            /* channels used in PPM, leave to 16*/
#define CHANNEL_MIN 0              /* minimum of a channel value (caution, uint used) */
#define CHANNEL_MAX 2000           /* maximum of a channel value */

/* Create Joystick */
Joystick_ Joystick; /* joystick HID driver */

/* When true, send in each loop call; if set to false, sent only when RC signal is updated (latter works fine) */
const bool joyAutoSendMode = false;

/* variables for evaluating the PPM signal */
uint8_t curChannel = 255;           /* channel used to assign next valid pulse, 0 - 15 for channels 1-16, 255 for any other state */
uint32_t edgeTime = 0;              /* time in micro seconds of last signal edge */
uint16_t channels[CHANNEL_COUNT];   /* array of channel values used for RX'ing, may be incomplete when frame errors occur */
uint16_t rxChannels[CHANNEL_COUNT]; /* latest valid array of channel values */

/* task scheduler */
Timer<1> scheduler; /* scheduler, currently only for serial printing */

void setup() {
  Serial.begin(9600);
  /* clear channel values to 0 */
  for (int i = 0; i < CHANNEL_COUNT; ++i) {
    channels[i] = 0;
    rxChannels[i] = 0;
  }

  /* Set axes range (defined in the header, 0 - 1000) */
  Joystick.setXAxisRange(CHANNEL_MIN, CHANNEL_MAX);
  Joystick.setYAxisRange(CHANNEL_MIN, CHANNEL_MAX);
  Joystick.setZAxisRange(CHANNEL_MIN, CHANNEL_MAX);
  Joystick.setRxAxisRange(CHANNEL_MIN, CHANNEL_MAX);
  Joystick.setRyAxisRange(CHANNEL_MIN, CHANNEL_MAX);
  Joystick.setRzAxisRange(CHANNEL_MIN, CHANNEL_MAX);
  Joystick.setThrottleRange(CHANNEL_MIN, CHANNEL_MAX);
  Joystick.setRudderRange(CHANNEL_MIN, CHANNEL_MAX);

  if (joyAutoSendMode) {
    Joystick.begin();
  } else {
    Joystick.begin(false);
  }
  Serial.println("USB started");
  /* init PPM input pin */
  pinMode(PPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PPM_PIN), PPM_Pin_Changed, FALLING);

  /* init scheduler */
  if (SERIAL_PRINT_INTERVAL > 0) {
    scheduler.every(SERIAL_PRINT_INTERVAL, [](void*) -> bool {
      SerialPrintChannels();
      return true;
    });
  }
}


void PPM_Pin_Changed() {
  uint32_t curTime = micros();
  uint32_t pulseLength = curTime - edgeTime;

  //Serial.print("PT: ");
  //Serial.print(pulseLength);
  //Serial.print("CurrState: ");
  //Serial.println(curState);
  if (pulseLength > 3000) {
    /* start pulse was detected, next valid channel value will be assigned to channel 0 */
    curChannel = 0;
  } else {
    //Serial.print("PT: ");
    //Serial.print(pulseLength);
    //Serial.println("");
      if (curChannel < CHANNEL_COUNT) {

      /* valid channel pulse length */
      uint16_t rxLength = pulseLength;

      rxChannels[curChannel] = rxLength - 600;
      //Serial.print("Got data. Ch: ");
      //Serial.print(curChannel);
      //Serial.print(" Data ");
      //Serial.println(rxLength);

      /* increase the channel counter */
      curChannel++;

      if (curChannel >= CHANNEL_COUNT) {
        updateRC();
        curChannel = 0;
      }
    }  //curChannel
  }

  /* store time of last edge for evaluating the next pulse's length */
  edgeTime = curTime;
}

void updateRC() {
  /* copy rx values to actual channel buffer */
  for (int i = 0; i < CHANNEL_COUNT; ++i) {
    channels[i] = rxChannels[i];
  }

  /* assign channel values to joystick */
  /* combine and assign buttons, axes to channels as you wish */
  /* axis inversion can be done here, in the TX or in FSX */

  /*  CHANNEL / Function Assignment and Joystick assignment
   *   1 Throttle     Axis Throttle
   *   2 Aileron      Axis X
   *   3 Elevator     Axis Y
   *   4 Rudder       Axis RX       
   *   5 Flaps        Axis RY         
   *   6 Brake        Axis Z          tweeked by FSUIPC (assigned to both left and right brakes; however, two channels or axes can be used for differential braking, with mixing either in the TX or in the arduino)
   *   7 Gear         Button0         tweeked by FSUIPC (separate Gear up/down events)
   *   8
   *   9
   *  10
   *  11
   *  12
   *  13
   *  14
   *  15
   *  16
   */

  /* axes */
  //Joystick.setThrottle(channels[0]);
  Joystick.setXAxis(channels[5]);
  Joystick.setYAxis(channels[4]);
  //Joystick.setRxAxis(channels[3]);
  //Joystick.setRyAxis(channels[4]);
  // Joystick.setZAxis(1000 - channels[5]);

  /* buttons */
  //Joystick.setButton(0, (channels[6] < 500 ? 1 : 0));

  Joystick.sendState();
}

void SerialPrintChannels() {
  /* quick and dirty debug printing of the received channels */
  Serial.print("Channels:   ");
  for (int i = 0; i < CHANNEL_COUNT; ++i) {
    Serial.print(channels[i]);
    Serial.print("   ");
  }
  Serial.println("");
}

void loop() {
  scheduler.tick();
}
