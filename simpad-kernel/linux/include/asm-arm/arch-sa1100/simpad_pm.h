/*
* Abstraction interface for microcontroller connection to rest of system
*
* Copyright 2003 Peter Pregler
* Copyright 2000,1 Compaq Computer Corporation.
*
* Use consistent with the GNU GPL is permitted,
* provided that this copyright notice is
* preserved in its entirety in all copies and derived works.
*
* COMPAQ COMPUTER CORPORATION MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
* AS TO THE USEFULNESS OR CORRECTNESS OF THIS CODE OR ITS
* FITNESS FOR ANY PARTICULAR PURPOSE.
*
* Author: Peter Pregler (based on work for ipaq by Andrew Christian)
*
*/

#ifndef __SIMPAD_HAL_H
#define __SIMPAD_HAL_H

extern int simpad_apm_get_power_status(unsigned char *ac_line_status, unsigned char *battery_status, 
				      unsigned char *battery_flag, unsigned char *battery_percentage, 
				      unsigned short *battery_life);


struct simpad_battery {
  unsigned char ac_status;	/* line connected yes/no */
  unsigned char status;		/* battery loading yes/no */
  unsigned char percentage;	/* percentage loaded */
  unsigned short life;		/* life till empty */
};

extern int simpad_get_battery(struct simpad_battery *bstat);

/* These should match the apm_bios.h definitions */
#define SIMPAD_AC_STATUS_AC_OFFLINE      0x00
#define SIMPAD_AC_STATUS_AC_ONLINE       0x01
#define SIMPAD_AC_STATUS_AC_BACKUP       0x02   /* What does this mean? */
#define SIMPAD_AC_STATUS_AC_UNKNOWN      0xff
                                                                                                                                           
/* These bitfields are rarely "or'd" together */
#define SIMPAD_BATT_STATUS_HIGH          0x01
#define SIMPAD_BATT_STATUS_LOW           0x02
#define SIMPAD_BATT_STATUS_CRITICAL      0x04
#define SIMPAD_BATT_STATUS_CHARGING      0x08
#define SIMPAD_BATT_STATUS_CHARGE_MAIN   0x10
#define SIMPAD_BATT_STATUS_DEAD          0x20   /* Battery will not charge */
#define SIMPAD_BATT_NOT_INSTALLED        0x20   /* For expansion pack batteries */
#define SIMPAD_BATT_STATUS_FULL          0x40   /* Battery fully charged (and connected to AC) */
#define SIMPAD_BATT_STATUS_NOBATT        0x80
#define SIMPAD_BATT_STATUS_UNKNOWN       0xff
                                                                                                                                           
#if FIXME
#include <linux/simpad_ts.h>

enum simpad_asset_type {
	ASSET_TCHAR = 0,
	ASSET_SHORT,
	ASSET_LONG
};

#define TTYPE(_type)           (((unsigned int)_type) << 8)
#define TCHAR(_len)            (TTYPE(ASSET_TCHAR) | (_len))
#define TSHORT                 TTYPE(ASSET_SHORT)
#define TLONG                  TTYPE(ASSET_LONG)
#define ASSET(_type,_num)      ((((unsigned int)_type)<<16) | (_num))

#define ASSET_HM_VERSION        ASSET( TCHAR(10), 0 )   /* 1.1, 1.2 */
#define ASSET_SERIAL_NUMBER     ASSET( TCHAR(40), 1 )   /* Unique iPAQ serial number */
#define ASSET_MODULE_ID         ASSET( TCHAR(20), 2 )   /* E.g., "iPAQ 3700" */    
#define ASSET_PRODUCT_REVISION  ASSET( TCHAR(10), 3 )   /* 1.0, 2.0 */
#define ASSET_PRODUCT_ID        ASSET( TSHORT,    4 )   /* 2 = Palm-sized computer */
#define ASSET_FRAME_RATE        ASSET( TSHORT,    5 )
#define ASSET_PAGE_MODE         ASSET( TSHORT,    6 )   /* 0 = Flash memory */
#define ASSET_COUNTRY_ID        ASSET( TSHORT,    7 )   /* 0 = USA */
#define ASSET_IS_COLOR_DISPLAY  ASSET( TSHORT,    8 )   /* Boolean, 1 = yes */
#define ASSET_ROM_SIZE          ASSET( TSHORT,    9 )   /* 16, 32 */
#define ASSET_RAM_SIZE          ASSET( TSHORT,   10 )   /* 32768 */
#define ASSET_HORIZONTAL_PIXELS ASSET( TSHORT,   11 )   /* 240 */
#define ASSET_VERTICAL_PIXELS   ASSET( TSHORT,   12 )   /* 320 */

#define ASSET_TYPE(_asset)       (((_asset)&0xff000000)>>24)
#define ASSET_TCHAR_LEN(_asset)  (((_asset)&0x00ff0000)>>16)
#define ASSET_NUMBER(_asset)     ((_asset)&0x0000ffff)

#define MAX_TCHAR_LEN 40

struct simpad_asset {
	unsigned int type;
	union {
		unsigned char  tchar[ MAX_TCHAR_LEN ];
		unsigned short vshort;
		unsigned long  vlong;
	} a;
};

/********************************************************************
 * Interface to the hardware-type specific functions
 *
 * get_version           Read the version number of the microcontroller on the option pack SPI bus
 * spi_read              Reads from the serial EEPROM memory on the option pack SPI bus
 * spi_write             Write to the serial EEPROM memory on the option pack SPI bus
 * get_option_detect     Returns whether or not an option pack is present
 *
 * get_thermal_sensor    Return measured temperature of the unit, in units of 0.125 deg C
 * set_notify_led        Turns on, off, or blinks the Green LED
 * read_light_sensor     Returns the value of the front light sensor
 * get_battery           Returns the current voltage and charging state of all batteries
 * audio_clock           Sets the audio CODEC to run at a particular rate
 * audio_power           Turns on/off audio CODEC (internally calls audio_clock)
 * audio_mute            Mutes the audio CODEC
 * asset_read            Extracts PocketPC-style asset information from persistent memory
 * backlight_control     Adjusts the backlight level  (only on/off for 3100)
 *
 *
 * iPAQ 3100 only
 * ==============
 * codec_control         Reset/mute/control level of 3100 audio codec
 * contrast_control      Adjusts the contrast level   (only for 3100)
 *
 * iPAQ 3600, 3700 only
 * ====================
 * eeprom_read           Reads from the asset information on the eeprom of a 3600 (deprecated)
 * eeprom_write          Writes to the asset information on the eeprom (deprecated)
 *
 * The interfaces to the EEPROM functions are maintained only because the simpad_ts driver has
 * a deprecated ioctl call for them.  They are used internally by the "asset_read" function.
 *
 * iPAQ 3800, 3900 only
 * ====================
 * set_ebat              Tells enhanced PCMCIA sleeves that this iPAQ can handle 
 *                       a wider voltage range (applies to 3800, 3900)
 *
 *********************************************************************/

struct simpad_hal_ops {
	/* Functions provided by the underlying hardware */
	int (*get_version)( struct simpad_ts_version * );
	int (*eeprom_read)( unsigned short address, unsigned char *data, unsigned short len );
	int (*eeprom_write)( unsigned short address, unsigned char *data, unsigned short len );
	int (*get_thermal_sensor)( unsigned short * );
	int (*set_notify_led)( unsigned char mode, unsigned char duration, 
			       unsigned char ontime, unsigned char offtime );
	int (*read_light_sensor)( unsigned char *result );
	int (*get_battery)( struct simpad_battery * );
	int (*spi_read)( unsigned short address, unsigned char *data, unsigned short len );
	int (*spi_write)( unsigned short address, unsigned char *data, unsigned short len );
	int (*codec_control)( unsigned char, unsigned char );
	int (*get_option_detect)( int *result );
	int (*audio_clock)( long samplerate );
	int (*audio_power)( long samplerate );
	int (*audio_mute)( int mute );
	int (*asset_read)( struct simpad_asset *asset );
	int (*set_ebat)( void );

	/* Functions indirectly provided by the underlying hardware */
	int (*backlight_control)( enum flite_pwr power, unsigned char level );
	int (*contrast_control)( unsigned char level );

        /* for module use counting */ 
        struct module *owner;
};

/* Used by the device-specific hardware module to register itself */
extern int  simpad_hal_register_interface( struct simpad_hal_ops *ops );
extern void simpad_hal_unregister_interface( struct simpad_hal_ops *ops );

/* 
 * Calls into HAL from the device-specific hardware module
 * These run at interrupt time 
 */
extern void simpad_hal_keypress( unsigned char key );
extern void simpad_hal_touchpanel( unsigned short x, unsigned short y, int down );
extern void simpad_hal_option_detect( int present );

/* Callbacks registered by device drivers */
struct simpad_driver_ops {
	void (*keypress)( unsigned char key );
	void (*touchpanel)( unsigned short x, unsigned short y, int down );
	void (*option_detect)( int present );
};

extern int  simpad_hal_register_driver( struct simpad_driver_ops * );
extern void simpad_hal_unregister_driver( struct simpad_driver_ops * );


/* Calls into HAL from device drivers and other kernel modules */
extern void simpad_get_flite( struct simpad_ts_backlight *bl );
extern void simpad_get_contrast( unsigned char *contrast );
extern int  simpad_set_flite( enum flite_pwr pwr, unsigned char brightness );
extern int  simpad_set_contrast( unsigned char contrast );
extern int  simpad_toggle_frontlight( void );

extern int simpad_apm_get_power_status(unsigned char *ac_line_status, unsigned char *battery_status, 
				      unsigned char *battery_flag, unsigned char *battery_percentage, 
				      unsigned short *battery_life);

extern struct simpad_hal_ops *simpad_hal_ops;

/* Do not use this macro in driver files - instead, use the inline functions defined below */
#define CALL_HAL( f, args... ) \
        { int __result = -EIO;                             \
          if ( simpad_hal_ops && simpad_hal_ops->f ) {       \
                __MOD_INC_USE_COUNT(simpad_hal_ops->owner); \
                __result = simpad_hal_ops->f(args);         \
                __MOD_DEC_USE_COUNT(simpad_hal_ops->owner); \
          }                                                \
          return __result; }

#define HFUNC  static __inline__ int

/* The eeprom_read/write address + len has a maximum value of 512.  Both must be even numbers */
HFUNC simpad_eeprom_read( u16 addr, u8 *data, u16 len )  CALL_HAL(eeprom_read,addr,data,len)
HFUNC simpad_eeprom_write( u16 addr, u8 *data, u16 len)  CALL_HAL(eeprom_write,addr,data,len)
HFUNC simpad_spi_read( u8 addr, u8 *data, u16 len) 	CALL_HAL(spi_read,addr,data,len)
HFUNC simpad_spi_write( u8 addr, u8 *data, u16 len) 	CALL_HAL(spi_write,addr,data,len)
HFUNC simpad_get_version( struct simpad_ts_version *v )   CALL_HAL(get_version,v)
HFUNC simpad_get_thermal_sensor( u16 *thermal ) 	        CALL_HAL(get_thermal_sensor,thermal)
HFUNC simpad_set_led( u8 mode, u8 dur, u8 ont, u8 offt ) CALL_HAL(set_notify_led, mode, dur, ont, offt)
HFUNC simpad_get_light_sensor( u8 *result ) 	        CALL_HAL(read_light_sensor,result)
HFUNC simpad_get_battery( struct simpad_battery *bat )	CALL_HAL(get_battery,bat)
HFUNC simpad_get_option_detect( int *result)             CALL_HAL(get_option_detect,result)
HFUNC simpad_audio_clock( long samplerate )              CALL_HAL(audio_clock,samplerate)
HFUNC simpad_audio_power( long samplerate )              CALL_HAL(audio_power,samplerate)
HFUNC simpad_audio_mute( int mute )                      CALL_HAL(audio_mute,mute)
HFUNC simpad_asset_read( struct simpad_asset *asset )     CALL_HAL(asset_read,asset)
HFUNC simpad_set_ebat( void )                            CALL_HAL(set_ebat)

/* Don't use these functions directly - rather, call {get,set}_{flite,contrast} */
	/* Functions indirectly provided by the underlying hardware */
HFUNC simpad_backlight_control( enum flite_pwr p, u8 v ) CALL_HAL(backlight_control,p,v)
HFUNC simpad_contrast_control( u8 level )                CALL_HAL(contrast_control,level)

#endif
#endif
