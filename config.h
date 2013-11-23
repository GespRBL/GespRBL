/*
  config.h - compile time configuration
  Part of Grbl

  Copyright (c) 2011-2013 Sungeun K. Jeon
  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

// This file contains compile-time configurations for Grbl's internal system. For the most part,
// users will not need to directly modify these, but they are here for specific needs, i.e.
// performance tuning or adjusting to non-typical machines.

// IMPORTANT: Any changes here requires a full re-compiling of the source code to propagate them.

#ifndef config_h
#define config_h

// Default settings. Used when resetting EEPROM. Change to desired name in defaults.h
#define DEFAULTS_ZEN_TOOLWORKS_7x7

// Serial baud rate
#define BAUD_RATE 115200

// Default pin mappings. Grbl officially supports the Arduino Uno only. Other processor types
// may exist from user-supplied templates or directly user-defined in pin_map.h
#define PIN_MAP_ARDUINO_UNO

// Define runtime command special characters. These characters are 'picked-off' directly from the
// serial read data stream and are not passed to the grbl line execution parser. Select characters
// that do not and must not exist in the streamed g-code program. ASCII control characters may be 
// used, if they are available per user setup. Also, extended ASCII codes (>127), which are never in 
// g-code programs, maybe selected for interface programs.
// NOTE: If changed, manually update help message in report.c.
#define CMD_STATUS_REPORT '?'
#define CMD_FEED_HOLD '!'
#define CMD_CYCLE_START '~'
#define CMD_RESET 0x18 // ctrl-x

// The "Stepper Driver Interrupt" employs an inverse time algorithm to manage the Bresenham line 
// stepping algorithm. The value ISR_TICKS_PER_SECOND is the frequency(Hz) at which the inverse time
// algorithm ticks at. Recommended step frequencies are limited by the inverse time frequency by
// approximately 0.75-0.9 * ISR_TICK_PER_SECOND. Meaning for 30kHz, the max step frequency is roughly
// 22.5-27kHz, but 30kHz is still possible, just not optimal. An Arduino can safely complete a single
// interrupt of the current stepper driver algorithm theoretically up to a frequency of 35-40kHz, but 
// CPU overhead increases exponentially as this frequency goes up. So there will be little left for 
// other processes like arcs.  
#define ISR_TICKS_PER_SECOND 30000L  // Integer (Hz)

// The temporal resolution of the acceleration management subsystem. Higher number give smoother
// acceleration but may impact performance. If you run at very high feedrates (>15kHz or so) and 
// very high accelerations, this will reduce the error between how the planner plans the velocity
// profiles and how the stepper program actually performs them. The correct value for this parameter
// is machine dependent, so it's advised to set this only as high as needed. Approximate successful
// values can widely range from 50 to 200 or more. Cannot be greater than ISR_TICKS_PER_SECOND/2.
// NOTE: Ramp count variable type in stepper module may need to be updated if changed.
#define ACCELERATION_TICKS_PER_SECOND 120L 

// NOTE: Make sure this value is less than 256, when adjusting both dependent parameters.
#define ISR_TICKS_PER_ACCELERATION_TICK (ISR_TICKS_PER_SECOND/ACCELERATION_TICKS_PER_SECOND)

// The inverse time algorithm can use either floating point or long integers for its counters (usually
// very small values ~10^-6), but with integers, the counter values must be scaled to be greater than
// one. This multiplier value scales the floating point counter values for use in a long integer, which 
// are significantly faster to compute with a slightly higher precision ceiling than floats. Long 
// integers are finite so select the multiplier value high enough to avoid any numerical round-off 
// issues and still have enough range to account for all motion types. However, in most all imaginable 
// CNC applications, the following multiplier value will work more than well enough. If you do have
// happened to weird stepper motion issues, try modifying this value by adding or subtracting a 
// zero and report it to the Grbl administrators. 
#define INV_TIME_MULTIPLIER 100000

// Minimum stepper rate for the "Stepper Driver Interrupt". Sets the absolute minimum stepper rate 
// in the stepper program and never runs slower than this value. If the INVE_TIME_MULTIPLIER value
// changes, it will affect how this value works. So, if a zero is add/subtracted from the
// INV_TIME_MULTIPLIER value, do the same to this value if you want to same response. 
// NOTE: Compute by (desired_step_rate/60) * INV_TIME_MULTIPLIER/ISR_TICKS_PER_SECOND. (mm/min)
// #define MINIMUM_STEP_RATE 1000L // Integer (mult*mm/isr_tic)

// Minimum stepper rate. Only used by homing at this point. May be removed in later releases.
#define MINIMUM_STEPS_PER_MINUTE 800 // (steps/min) - Integer value only

// Minimum planner junction speed. Sets the default minimum junction speed the planner plans to at
// every buffer block junction, except for starting from rest and end of the buffer, which are always
// zero. This value controls how fast the machine moves through junctions with no regard for acceleration
// limits or angle between neighboring block line move directions. This is useful for machines that can't
// tolerate the tool dwelling for a split second, i.e. 3d printers or laser cutters. If used, this value
// should not be much greater than zero or to the minimum value necessary for the machine to work.
#define MINIMUM_JUNCTION_SPEED 0.0 // (mm/min)

// Time delay increments performed during a dwell. The default value is set at 50ms, which provides
// a maximum time delay of roughly 55 minutes, more than enough for most any application. Increasing
// this delay will increase the maximum dwell time linearly, but also reduces the responsiveness of 
// run-time command executions, like status reports, since these are performed between each dwell 
// time step. Also, keep in mind that the Arduino delay timer is not very accurate for long delays.
#define DWELL_TIME_STEP 50 // Integer (1-255) (milliseconds)

// If homing is enabled, homing init lock sets Grbl into an alarm state upon power up. This forces
// the user to perform the homing cycle (or override the locks) before doing anything else. This is
// mainly a safety feature to remind the user to home, since position is unknown to Grbl.
#define HOMING_INIT_LOCK // Comment to disable

// The homing cycle seek and feed rates will adjust so all axes independently move at the homing
// seek and feed rates regardless of how many axes are in motion simultaneously. If disabled, rates
// are point-to-point rates, as done in normal operation. For example in an XY diagonal motion, the
// diagonal motion moves at the intended rate, but the individual axes move at 70% speed. This option
// just moves them all at 100% speed.
#define HOMING_RATE_ADJUST // Comment to disable

// Define the homing cycle search patterns with bitmasks. The homing cycle first performs a search
// to engage the limit switches. HOMING_SEARCH_CYCLE_x are executed in order starting with suffix 0 
// and searches the enabled axes in the bitmask. This allows for users with non-standard cartesian 
// machines, such as a lathe (x then z), to configure the homing cycle behavior to their needs. 
// Search cycle 0 is required, but cycles 1 and 2 are both optional and may be commented to disable.
// After the search cycle, homing then performs a series of locating about the limit switches to hone
// in on machine zero, followed by a pull-off maneuver. HOMING_LOCATE_CYCLE governs these final moves,
// and this mask must contain all axes in the search.
// NOTE: Later versions may have this installed in settings.
#define HOMING_SEARCH_CYCLE_0 (1<<Z_AXIS)                // First move Z to clear workspace.
#define HOMING_SEARCH_CYCLE_1 ((1<<X_AXIS)|(1<<Y_AXIS))  // Then move X,Y at the same time.
// #define HOMING_SEARCH_CYCLE_2                         // Uncomment and add axes mask to enable
#define HOMING_LOCATE_CYCLE   ((1<<X_AXIS)|(1<<Y_AXIS)|(1<<Z_AXIS)) // Must contain ALL search axes

// Number of homing cycles performed after when the machine initially jogs to limit switches.
// This help in preventing overshoot and should improve repeatability. This value should be one or 
// greater.
#define N_HOMING_LOCATE_CYCLE 2 // Integer (1-128)

// Number of blocks Grbl executes upon startup. These blocks are stored in EEPROM, where the size
// and addresses are defined in settings.h. With the current settings, up to 5 startup blocks may
// be stored and executed in order. These startup blocks would typically be used to set the g-code
// parser state depending on user preferences.
#define N_STARTUP_LINE 2 // Integer (1-5)

// Number of arc generation iterations by small angle approximation before exact arc trajectory 
// correction. This parameter maybe decreased if there are issues with the accuracy of the arc
// generations. In general, the default value is more than enough for the intended CNC applications
// of grbl, and should be on the order or greater than the size of the buffer to help with the 
// computational efficiency of generating arcs.
#define N_ARC_CORRECTION 20 // Integer (1-255)

// ---------------------------------------------------------------------------------------
// FOR ADVANCED USERS ONLY: 

// The number of linear motions in the planner buffer to be planned at any give time. The vast
// majority of RAM that Grbl uses is based on this buffer size. Only increase if there is extra 
// available RAM, like when re-compiling for a Mega or Sanguino. Or decrease if the Arduino
// begins to crash due to the lack of available RAM or if the CPU is having trouble keeping
// up with planning new incoming motions as they are executed. 
// #define BLOCK_BUFFER_SIZE 18  // Uncomment to override default in planner.h.

// Governs the size of the intermediary step segment buffer between the step execution algorithm
// and the planner blocks. Each segment is set of steps executed at a constant velocity over a
// fixed time defined by ACCELERATION_TICKS_PER_SECOND. They are computed such that the planner
// block velocity profile is traced exactly. The size of this buffer governs how much step 
// execution lead time there is for other Grbl processes have to compute and do their thing 
// before having to come back and refill this buffer, currently at ~50msec of step moves.
// #define SEGMENT_BUFFER_SIZE 7 // Uncomment to override default in stepper.h.

// Line buffer size from the serial input stream to be executed. Also, governs the size of 
// each of the startup blocks, as they are each stored as a string of this size. Make sure
// to account for the available EEPROM at the defined memory address in settings.h and for
// the number of desired startup blocks.
// NOTE: 70 characters is not a problem except for extreme cases, but the line buffer size 
// can be too small and g-code blocks can get truncated. Officially, the g-code standards 
// support up to 256 characters. In future versions, this default will be increased, when 
// we know how much extra memory space we can re-invest into this.
// #define LINE_BUFFER_SIZE 70  // Uncomment to override default in protocol.h
  
// Serial send and receive buffer size. The receive buffer is often used as another streaming
// buffer to store incoming blocks to be processed by Grbl when its ready. Most streaming
// interfaces will character count and track each block send to each block response. So, 
// increase the receive buffer if a deeper receive buffer is needed for streaming and avaiable
// memory allows. The send buffer primarily handles messages in Grbl. Only increase if large
// messages are sent and Grbl begins to stall, waiting to send the rest of the message.
// #define RX_BUFFER_SIZE 128 // Uncomment to override defaults in serial.h
// #define TX_BUFFER_SIZE 64
  
// Toggles XON/XOFF software flow control for serial communications. Not officially supported
// due to problems involving the Atmega8U2 USB-to-serial chips on current Arduinos. The firmware
// on these chips do not support XON/XOFF flow control characters and the intermediate buffer 
// in the chips cause latency and overflow problems with standard terminal programs. However, 
// using specifically-programmed UI's to manage this latency problem has been confirmed to work.
// As well as, older FTDI FT232RL-based Arduinos(Duemilanove) are known to work with standard
// terminal programs since their firmware correctly manage these XON/XOFF characters. In any
// case, please report any successes to grbl administrators!
// #define ENABLE_XONXOFF // Default disabled. Uncomment to enable.

// ---------------------------------------------------------------------------------------

// TODO: Install compile-time option to send numeric status codes rather than strings.

// ---------------------------------------------------------------------------------------
// COMPILE-TIME ERROR CHECKING OF DEFINE VALUES:

#if (ISR_TICKS_PER_ACCELERATION_TICK > 255)
#error Parameters ACCELERATION_TICKS / ISR_TICKS must be < 256 to prevent integer overflow.
#endif

// ---------------------------------------------------------------------------------------
#endif
