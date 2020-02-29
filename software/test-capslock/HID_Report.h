/* Copyright 2019 Bernd Lehmann
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __HID_REPORT_H__
#define __HID_REPORT_H__

/*
 * References:
 * [1] Device Class Definition for Human Interface Devices (HID) (6/27/01) Version 1.11
 * [2] HID Usage Tables (10/28/2004) Version 1.12
 */

/*
 * bType definitions for Short Items
 * see [1] section 6.2.2.2
 */
#define MAIN     0
#define GLOBAL   1
#define LOCAL    2
#define RESERVED 3

/*
 * bSize definitions for Short Items
 * see [1] section 6.2.2.2
 */
#define BSIZE_0 0
#define BSIZE_1 1 
#define BSIZE_2 2
#define BSIZE_4 3


/* 
 * constructor for the first byte of a Short Item 
 * see [1] section 6.2.2.2
 */
#define ITEM(bTag, bType, bSize) ((bTag << 4) | (bType << 2) | bSize)


/*
 * TODO: constructor Long Item
 * We skip long items, since i didn't saw a long item until now.
 */


/*
 * definitions of main items
 * see [1] section 6.2.2.4
 *
 * TODO: In theory, main items might have more than one byte data, but i didn't saw
 * an example with it until now and therefor it is not supported.
 */
#define INPUT(val)       ITEM(0x8, MAIN, BSIZE_1), (val)
#define OUTPUT(val)      ITEM(0x9, MAIN, BSIZE_1), (val)
#define FEATURE(val)     ITEM(0xB, MAIN, BSIZE_1), (val)
#define COLLECTION(type) ITEM(0xA, MAIN, BSIZE_1), (type)
#define END_COLLECTION    ITEM(0xC, MAIN, BSIZE_0)


/*
 * definitions of the attributes for the main items INPUT, OUTPUT and FEATURE.
 * see [1] section 6.2.2.5
 *
 * Note that the attributes NON_VOLATILE and VOLATILE are not defined for INPUT
 * and therefore should not be used in combination with INPUT.
 */
#define DATA             (0)
#define CONSTANT         (1)
#define ARRAY            (0)
#define VARIABLE         (1<<1)
#define ABSOLUTE         (0)
#define RELATIVE         (1<<2)
#define NOWRAP           (0)
#define WRAP             (1<<3)
#define LINEAR           (0)
#define NOLINEAR         (1<<4)
#define PREFERRED_STATE  (0)
#define NO_PREFERRED     (1<<5)
#define NO_NULL_POSITION (0)
#define NULL_STATE       (1<<6)
#define NON_VOLATILE     (0)
#define VOLATILE         (1<<7)
#define BIT_FIELD        (0)
#define BUFFERED_BYTES   (1<<8)


/*
 * definitions of the collection types for the main item COLLECTION.
 * see [1] section 6.2.2.6
 *
 * Note: Values from 0x07 to 0x7F are reserved for future use.
 * Note: Values from 0x80 to 0xFF can be used for vendor spcific applications
 */
#define PHYSICAL       (0x00)
#define APPLICATION    (0x01)
#define LOGICAL        (0x02)
#define REPORT         (0x03)
#define NAMED_ARRAY    (0x04)
#define USAGE_SWITCH   (0x05)
#define USAGE_MODIFIER (0x06)


/*
 * definitions of global items
 * see [1] section 6.2.2.7
 *
 * TODO: In theory, global items might have more than one byte data, but i didn't saw
 * an example with it until now and therefor it is not supported.
 */
#define USAGE_PAGE(val)       ITEM(0x0, GLOBAL, BSIZE_1), (val)
#define LOGICAL_MINIMUM(val)  ITEM(0x1, GLOBAL, BSIZE_1), (val)
#define LOGICAL_MAXIMUM(val)  ITEM(0x2, GLOBAL, BSIZE_1), (val)
/*
#define PHYSICAL_MINIMUM ITEM(0x3, GLOBAL, BSIZE_0)
#define PHYSICAL_MAXIMUM ITEM(0x4, GLOBAL, BSIZE_0)
#define UNIT_EXPONENT    ITEM(0x5, GLOBAL, BSIZE_0)
#define UNIT             ITEM(0x6, GLOBAL, BSIZE_0)
*/
#define REPORT_SIZE(val)      ITEM(0x7, GLOBAL, BSIZE_1), (val)
/*
#define REPORT_ID        ITEM(0x8, GLOBAL, BSIZE_0)
*/
#define REPORT_COUNT(val)     ITEM(0x9, GLOBAL, BSIZE_1), (val)
/*
#define PUSH             ITEM(0xA, GLOBAL, BSIZE_0)
#define POP              ITEM(0xB, GLOBAL, BSIZE_0)
*/

/*
 * definitions of local items
 * see [1] section 6.2.2.8
 *
 * TODO: In theory, local items might have more than one byte data, but i didn't saw
 * an example with it until now and therefor it is not supported.
 */
#define USAGE(val)         ITEM(0x0, LOCAL, BSIZE_1), (val)
#define USAGE_MINIMUM(val) ITEM(0x1, LOCAL, BSIZE_1), (val)
#define USAGE_MAXIMUM(val) ITEM(0x2, LOCAL, BSIZE_1), (val)

/*
 * definitions of usage page values for USAGE_PAGE
 * see [2] section 3 (table 1)
 */
#define UNDEFINED                (0x00)
#define GENERIC_DESKTOP_CONTROLS (0x01)
#define SIMULATION_CONTROLS      (0x02)
#define VR_CONTROLS              (0x03)
#define SPORT_CONTROLS           (0x04)
#define GAME_CONTROLS            (0x05)
#define GENERIC_DEVICE CONTROLS  (0x06)
#define KEYBOARD_KEYPAD          (0x07)
#define LEDS                     (0x08)
#define BUTTON                   (0x09)
#define ORDINAL                  (0x0A)
/* there are more usage pages defined, but to lazy right now */

/*
 * usage definitions of usage page GENERIC_DESKTOP_CONTROLS
 * see [2] section 3 (table 1)
 */
//#define UNDEFINED                 (0x00) // already defined
#define POINTER                   (0x01)
#define MOUSE                     (0x02)
// reserved                       (0x03)
#define JOYSTICK                  (0x04)
#define GAME_PAD                  (0x05)
#define KEYBOARD                  (0x06)
#define KEYPAD                    (0x07)
#define MULTI_AXIS_CONTEOLLER     (0x08)
#define TABLET_PC_SYSTEM_CONTROLS (0x09)
// reserved                       (0x0A - 0x2f)
#define X                         (0x30)
#define Y                         (0x31)
#define Z                         (0x32)
#define RX                        (0x33)
#define RY                        (0x34)
#define RZ                        (0x35)
/* there are more usages defined, but to lazy right now */

#endif
