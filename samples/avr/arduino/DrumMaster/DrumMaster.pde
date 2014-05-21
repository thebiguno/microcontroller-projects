/*
 * Drum Master - controller for up to 32 + 8 sensors.
 * Copyright 2008 - 2009 Wyatt Olson <wyatt.olson@gmail.com>
 * 
 * At a very high level, Drum Master will listen to a series of sensors (both analog, via piezo
 * transducers, and digital, via grounding pullup resistors), and report the values back to the
 * computer via the serial port.  It does this in packets, after a certain maximum time delay,
 * using the ASCII protocol "<channel 1>:<velocity 1>;<channel 2>:<velocity 2>;...<channel x:velocity x>;"
 * where <channel> is the channel number, between 0 and 39, and <velocity> is the velocity of the
 * analog strike, between 0 and 1023 (for channels 0 - 31), or velocity is the state of the digital
 * channel 0 (off) or 1 (on), (for channels 32 - 39).
 *
 * Slave software, running on the computer, must take these data packets and map them to digital
 * audio samples, based on the channel, velocity, and current state of the program.
 *
 * For more information, please visit http://drummaster.digitalcave.ca.
 * 
 * Changelog:
 * 1.1.2.0 - Sept 12 2009:  -Fixed a bug with active channels which did not send data if the value
 *                          was below the trigger threshold.
 *                          -Adjusted active channel values to be more verbose in sending data, so
 *                          that the slave program has better data to work with.  This has resulted
 *                          in substantially more realistic hi hat behaviour.
 * 1.1.1.0 - July 2 2009:   -Adjusted #define values to better suit hardware.
 *                          -Added more comments for all #define values to indicate more clearly
 *                          what each does.
 *
 * 1.1.0.0 - June ? 2009:   -Added the concept of 'active channels'; channels that report
 *                          a value X number of times in a row are assumed to be active;
 *                          this is used for things like analog hi-hat controllers, where
 *                          we want to have a continuous report of changes, but not
 *                          constantly waste time on the serial port if there are no changes.
 *                          -Combine multiple simultaneous strikes into a single data packet
 *                          to reduce the number of expensive (~20ms each) serial writes.  
 *                          This has successfully decreased latency to unnoticable levels
 *                          when there are multiple simultaneous strikes.
 *
 * 1.0.0.0 - May ? 2008:    -Initial version.  Works fine for basic drumming requirements.
 */

//Send debug signals as well as data.  This will likely make slave software 
// stop working; only set to true when you are debugging via Arduino serial
// console view.
#define DEBUG false

//Analog Pins offset and count
#define A_OFFSET 0
#define A_COUNT 4

//Trigger Pins offset; count is the same as Analog
#define T_OFFSET 2

//Digital Pins offset and count
#define D_OFFSET 6

//Multiplexer selector pins.  S2 is MSB; S0 is LSB.
#define S0 10
#define S1 9
#define S2 8

//How long to wait after selecting a new sensor in a bank (us).
//#define SELECTION_DELAY 2

//EXTERNAL or DEFAULT.  This specifies whether to use the default 5v reference voltage for
// the ADC (DEFAULT), or use the AREF pin (which on the Drum Master board is shunted to
// 3.3v) (EXTERNAL).  Leave this as EXTERNAL unless you really know what you are doing.
#define ANALOG_REFERENCE EXTERNAL 

//How many times to read the input for each hit; this can help to ensure a good reading 
// is obtained.  A good default to this is 3; only adjust this if you find you need more
// accuracy, or you are noticing a great deal of lag.  Note that each analog read will
// take approximately 1ms to complete, and this is relatively expensive to perform many.
#define ANALOG_SAMPLES 3
//After a hit, don't report the same sensor for this long (ms).  This is the absolute 
// shortest 'double hit' which the hardware will report.  Since the slave software will
// (or at least should) also allow you to pick the double trigger threshold for each zone,
// this setting is more to ensure that the serial link is not overloaded with constant 
// data, rather than to actually add to the usability by limiting double triggers.
// A good default is 25; this is 1/40 second, and even very fast snare rolls should be 
// longer than this.
#define ANALOG_BOUNCE_PERIOD 25  
//After a digital change of state, don't report changes for this long (ms).  A good default
// is 75; any less than this tends to (at least on my hardware) make hi hat chics sound 
// multiple times.
#define DIGITAL_BOUNCE_PERIOD 75  


//After this many consecutive polling iterations which contain data, we will assume that the channel 
// is an 'active channel', and will poll less frequently.  Furthermore, we will only send a new event 
// when the state changes by more than a given percentage, to reduce useless chatter on the
// serial port.  (Active channels are used, for instance, on a hi hat, or other device where 
// there will always be some voltage present).  We only use this for analog sensors, as digital
// sensors will report all the time anyway, for any state changes (and there is almost no overhead
// incurred in reading a digital sensor, while there is about 1ms delay when reading analog).
#define MIN_ACTIVE_CHANNEL_POLL_COUNT 10
//How much of a change (in absolute values, based on a full data range of 0-1023) must happen 
// before we report a change over the serial port
#define MIN_ACTIVE_CHANNEL_REQUIRED_CHANGE 50
//We must wait at least this long between polling
#define MIN_ACTIVE_CHANNEL_POLL_INTERVAL 10
//We must not wait any longer than this between polling, even if a large change has not happened.
// This helps to avoid the situation where the change was missed (either from the slave software
// or the master), but we still want to know what the current state is.  Larger values here will
// keep the rest of the system responsive; a good place to start is 1000 or so (1 second).
#define MAX_ACTIVE_CHANNEL_POLL_INTERVAL 500


//This is used for a number of data buffers.  It should be set to the number of channels (this is
// 40 in default hardware).  You will only want to change this if you have modified / custom hardware.
#define BUFFER_SIZE 40

//If there are this many loops without reading any new data, but there is data still waiting in 
// the buffer, we just go ahead and send the waiting data right away.  This ensures that we don't
// wait for the max amount of time unless we are actually still reading sensor data.
#define MAX_LOOPS_WITHOUT_DATA 6
//In ms, how soon after the first reading is seen do we send a data burst, if there is still data 
// coming in.  Basically this acts as an upper limit on the latency between the first hit and the time
// that we start the serial output process.  If we see a single hit, and we loop for MAX_LOOPS_WITHOUT_DATA
// without seeing any more hits (which is the most common scenario), we will send the data anyway, even
// before we reach this time limit.
#define MAX_DATA_PACKET_INTERVAL 20




//Temp variables; i and j are iterators for port and bank respectively; s and v are the selector 
// address (channel) and velocity respectively;  x is truly a temp variable, and can be used for
// anything you wish.  There are no guarantees that it will keep its value for any length of time,
// so make sure nothing else stores to it before you read your value.
int i, j, s, v;
long x;


//Have we read any data in the last loop?  Set to false at the beginning of the loop, and marked as
// true whenever we read any data.
boolean read_data_this_loop;

//Is it time to read active channels?  Set to true at the beginning of a loop if this is the case, and
// reset at the end.
boolean read_active_channels;

//Counts how many consecutive loops have not resulted in any data being read.  Once this goes over 
// MAX_LOOPS_WITHOUT_DATA, we will send a data packet (if there is data waiting in the buffer).
int loops_without_reading_data;

//Active counter.  Increments each cell by one each time a value for the corresponding channel is
// read; if there is no value read, it will reset the cell to 0.  Once a given cell gets above
// MIN_ACTIVE_CHANNEL_POLL_COUNT, we will poll on a less frequent basis, and will only report large
// changes over the serial port.
int consecutive_reads[BUFFER_SIZE];

//Keep track of active channels, to reduce computations.  Once a channel is set to be active,
// only a reset will change it back.
boolean active_channel[BUFFER_SIZE];

//Set to the current time at the beginning of each loop, to reduce the number of expensive 
// calls to millis().
long time;

//When was the last time that each channel was read?  Used for debounce.
long last_read_time[BUFFER_SIZE];
//What was the last value read from each channel?
int last_value[BUFFER_SIZE];
//When was each bank last read?
long bank_last_read_time[8];
//When did we last read the active channels?
long last_read_active_channels;

//When we first sense something, we mark the time; once MAX_DATA_PACKET_INTERVAL ms
// have passed (or we have reached the max number of loops without seeing any new data)
// will we send the data burst.
long start_data_packet_time = -1; 

//This is where we buffer data until we have read enough to send a (very expensive) data
// burst over the serial port.
int data_buffer[BUFFER_SIZE]; 


//Setup method is called once at the beginning of execution.
void setup() {
  //Setup 3.3v reference pin.  This allows us to run the opamps on 5v rail
  // voltage, and still be able to obtain full 10 bit readings from the
  // analog inputs.
  analogReference(ANALOG_REFERENCE);

  //Set the analog triggers (Digital inputs) to input mode.  Note that we don't need to set 
  // analog sensors to input mode at all.
  for (i = 0; i < A_COUNT; i++){
    pinMode(i + T_OFFSET, INPUT);
  }
  
  //Set the digital input pin to input mode.
  pinMode(D_OFFSET, INPUT);

  //The three MUX selector switches need to be set to output.
  pinMode(S2, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S0, OUTPUT);

  //Initialize array counters
  for (i = 0; i < BUFFER_SIZE; i++){
    //Initialize data buffer to all -1's
    data_buffer[i] = -1; //-1 is the marker for 'no data', since it is possible that the data be '0'. 

    //Initialize the active consecutive reads counter
    consecutive_reads[i] = 0;

    //Reset the active channels to all false
    active_channel[i] = false;
  }

  //Start the serial link
  Serial.begin(115200);
}

//Loop is called repeatedly after setup has completed.
void loop() {
  //We cache the current time to avoid needing to call millis() each time throughout the loop.
  time = millis();
  read_data_this_loop = false;

  //If ACTIVE_CHANNEL_MIN_POLL_INTERVAL ms have passed since the last time we read active channels,
  // go ahead and read them now.
  if (time - last_read_active_channels > MIN_ACTIVE_CHANNEL_POLL_INTERVAL){
    read_active_channels = true;
    last_read_active_channels = time;
  }

  //We read pin 0 on each multiplexer in turn, then select pin 1 and read from each in turn,
  // etc.  This reduces the number of switches we need to make on the multiplexers.  While 
  // in theory these switches are not very expensive (the spec sheets indicate that they are
  // almost instantaneous), doing it this way can possibly allow people to use slower MUXs
  // if desired; you could add a couple Microsecond delay after selecting the next port, and
  // give the MUX time to settle out.
  for (i = 0; i < 0x8; i++){  //i == port (one channel on a multiplexer)
    set_mux_selectors(i);

    for (j = 0; j < A_COUNT; j++){ // j == bank
      s = get_channel(j, i);      

      //If the channel is defined to be 'active' (i.e., connected to a device which always
      // reports its state, such as a hi hat pedal, rather than a device which only reports
      // state when struck, like a piezo), then we poll less frequently.
      if (consecutive_reads[s] > MIN_ACTIVE_CHANNEL_POLL_COUNT && !active_channel[s]){
        active_channel[s] = true;
        if (DEBUG) {
          Serial.print("Switching channel ");
          Serial.print(s);
          Serial.println(" to be 'active'");
        }
      }

      //Check the last read time, and whether the trigger is active.  Triggers are used to determine
      // whether an analog sensor has any data waiting to be read; reading a digital signal 
      // (Trigger) is much less expensive than reading an analog sensor, (an analog sensor 
      // seems to take upwards of 1ms to read, while a digital sensor is a fraction of that).
      // Triggers are active if there is a voltage greater than about 0.3v on the corresponding 
      // analog pin.
//      if (!active_channel[s] || read_active_channels){
        if (time - last_read_time[s] > ANALOG_BOUNCE_PERIOD){
          if (digitalRead(j + T_OFFSET) || active_channel[s]){
            v = get_velocity(j + A_OFFSET);
            if (!active_channel[s] 
                  || abs(v - last_value[s]) > MIN_ACTIVE_CHANNEL_REQUIRED_CHANGE
                  || (active_channel[s] && time - last_read_time[s] > MAX_ACTIVE_CHANNEL_POLL_INTERVAL)){
              if (DEBUG){
                if (active_channel[s]){
                  Serial.print("Large change of value; old value is ");
                  Serial.print(last_value[s]);
                  Serial.print("; new value is ");
                  Serial.println(v);
                } 
              }
              if (start_data_packet_time == -1)
                start_data_packet_time = time;

              if (data_buffer[s] < v){
                data_buffer[s] = v;
                last_value[s] = v;
              }
              last_read_time[s] = time;
              bank_last_read_time[i] = time;
              read_data_this_loop = true;

              //Only bother incrementing consecutive reads for channels not already defined as active.
              if (!active_channel[s]){
                consecutive_reads[s] = consecutive_reads[s] + 1;
              }
            }
          }
          else {
            if (DEBUG){
              //	            Serial.print("Resetting consecutive reads for channel ");
              //	            Serial.println(s);
            }
            consecutive_reads[s] = 0;
          }  
        }   
//      }
    } //for j ...

    j = 4; //Digital pin.  Change this to a loop if we add another.
    s = get_channel(j, i);
    if (time - last_read_time[s] > DIGITAL_BOUNCE_PERIOD){
      //Remember that digital switches in drum master are reversed, since they 
      // use pull up resisitors.  Logic 1 is open, logic 0 is closed.  We invert
      // all digital readings to make this easy to keep straight.
      v = !digitalRead(D_OFFSET);
      if (v != last_value[s]){
        if (start_data_packet_time == -1)
          start_data_packet_time = time;
        data_buffer[s] = v;
        last_read_time[s] = time;
        last_value[s] = v;
        read_data_this_loop = true;
      }
    }
  }


  //If there is data in the buffer (start_data_packet_time > -1) and either there
  // was no data read this last iteration OR the max packet time has expired, 
  // we will send the data burst
  if (start_data_packet_time > -1){
    if (!read_data_this_loop){
      loops_without_reading_data++;
      if (DEBUG){
        //	        Serial.print("loops without reading data:");
        //	        Serial.println(loops_without_reading_data);
      }
    }
    else {
      loops_without_reading_data = 0; 
    }

    if (loops_without_reading_data > MAX_LOOPS_WITHOUT_DATA 
      || start_data_packet_time + MAX_DATA_PACKET_INTERVAL < time){
      if (DEBUG){
        //	        Serial.print("Sending data burst ");
        //	        Serial.print(time - start_data_packet_time);
        //	        Serial.println("ms after first hit detected"); 
      }
      send_data_burst();
    }
  }

  //We will read active channels again in ACTIVE_CHANNEL_POLL_INTERVAL ms.
  read_active_channels = false;
}

/*
 * Writes all data from the data buffer to the serial port, and finishes with a new line character.
 * Resets everything in the buffer to -1.  Also resets the loops_without_reading_data counter, and 
 * the start time for the data packet.
 */
void send_data_burst(){
  //After all ports / banks have been read, send a data burst.
  for (i = 0; i < BUFFER_SIZE; i++){
    if (data_buffer[i] != -1){
      Serial.print(i);
      Serial.print(":");
      Serial.print(data_buffer[i]);
      Serial.print(";"); 
      data_buffer[i] = -1;
    }
  }
  Serial.println();

  start_data_packet_time = -1;
  loops_without_reading_data = 0;
}

/*
 * Sets pins S2 - S0 to select the multiplexer output.
 */
void set_mux_selectors(int s){
  digitalWrite(S2, s & 0x4);
  digitalWrite(S1, s & 0x2);
  digitalWrite(S0, s & 0x1);
}

/*
 * Convert from a bank / port tuple to single channel number (value from 0 - 39 inclusive)
 * for sending to Drum Slave software.
 */
int get_channel(int bank, int port){
  return (bank << 3) | port;
}

/*
 * Reads the input a number of times, and returns the maximum.  The number 
 * of times to read the input is defined by ANALOG_SAMPLES
 */
int get_velocity(int pin){
  if (DEBUG){
    x = millis(); 
  }
  int max_val = 0;
  for (int t = 0; t < ANALOG_SAMPLES; t++){
    max_val = max(analogRead(pin), max_val); 
  }
  if (DEBUG){
    Serial.print("Read ");
    Serial.print(ANALOG_SAMPLES);
    Serial.print(" samples from pin ");
    Serial.print(pin);
    Serial.print(" in ");
    Serial.print(millis() - x);
    Serial.println("ms");
  }

  return max_val;
}
