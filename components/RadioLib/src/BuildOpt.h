#if !defined(_RADIOLIB_BUILD_OPTIONS_H)
#define _RADIOLIB_BUILD_OPTIONS_H

#include "TypeDef.h"

/* RadioLib build configuration options */

/*
 * Debug output enable.
 * Warning: Debug output will slow down the whole system significantly.
 *          Also, it will result in larger compiled binary.
 * Levels: basic - only main info
 *         protocol - mainly LoRaWAN stuff, but other protocols as well
 *         SPI - full transcript of all SPI communication
 */
#if !defined(RADIOLIB_DEBUG_BASIC)
  #define RADIOLIB_DEBUG_BASIC (0)
#endif
#if !defined(RADIOLIB_DEBUG_PROTOCOL)
  #define RADIOLIB_DEBUG_PROTOCOL (0)
#endif
#if !defined(RADIOLIB_DEBUG_SPI)
  #define RADIOLIB_DEBUG_SPI (0)
#endif
#if !defined(RADIOLIB_VERBOSE_ASSERT)
  #define RADIOLIB_VERBOSE_ASSERT (0)
#endif

// set which output port should be used for debug output
// may be Serial port (on Arduino) or file like stdout or stderr (on generic platforms)
#if !defined(RADIOLIB_DEBUG_PORT)
  #define RADIOLIB_DEBUG_PORT   stdout
#endif

/*
 * Comment to disable "paranoid" SPI mode, or set RADIOLIB_SPI_PARANOID to 0
 * Every write to an SPI register using SPI set function will be verified by a subsequent read operation.
 * This improves reliability, but slightly slows down communication.
 * Note: Enabled by default.
 */
#if !defined(RADIOLIB_SPI_PARANOID)
  #define RADIOLIB_SPI_PARANOID (1)
#endif

/*
 * Comment to disable parameter range checking
 * RadioLib will check provided parameters (such as frequency) against limits determined by the device manufacturer.
 * It is highly advised to keep this macro defined, removing it will allow invalid values to be set,
 * possibly leading to bricked module and/or program crashing.
 * Note: Enabled by default.
 */
#if !defined(RADIOLIB_CHECK_PARAMS)
  #define RADIOLIB_CHECK_PARAMS (1)
#endif

/*
 * God mode enable - all methods and member variables in all classes will be made public, thus making them accessible from Arduino code.
 * Warning: Come on, it's called GOD mode - obviously only use this if you know what you're doing.
 *          Failure to heed the above warning may result in bricked module.
 */
#if !defined(RADIOLIB_GODMODE)
  #define RADIOLIB_GODMODE (0)
#endif

/*
 * Low-level hardware access enable
 * This will make some hardware methods like SPI get/set accessible from the user sketch - think of it as "god mode lite"
 * Warning: RadioLib won't stop you from writing invalid stuff into your device, so it's quite easy to brick your module with this.
 */
#if !defined(RADIOLIB_LOW_LEVEL)
  #define RADIOLIB_LOW_LEVEL (0)
#endif

/*
 * Enable interrupt-based timing control
 * For details, see https://github.com/jgromes/RadioLib/wiki/Interrupt-Based-Timing
 */
#if !defined(RADIOLIB_INTERRUPT_TIMING)
  #define RADIOLIB_INTERRUPT_TIMING  (0)
#endif

/*
 * Enable static-only memory management: no dynamic allocation will be performed.
 * Warning: Large static arrays will be created in some methods. It is not advised to send large packets in this mode.
 */
#if !defined(RADIOLIB_STATIC_ONLY)
  #define RADIOLIB_STATIC_ONLY  (0)
#endif

// set the size of static arrays to use
#if !defined(RADIOLIB_STATIC_ARRAY_SIZE)
  #define RADIOLIB_STATIC_ARRAY_SIZE   (256)
#endif

/*
 * Uncomment on boards whose clock runs too slow or too fast
 * Set the value according to the following scheme:
 * Enable timestamps on your terminal
 * Print something to terminal, wait 1000 milliseconds, print something again
 * If the difference is e.g. 1014 milliseconds between the prints, set this value to 14
 * Or, for more accuracy, wait for 100,000 milliseconds and divide the total drift by 100
 */
#if !defined(RADIOLIB_CLOCK_DRIFT_MS)
  //#define RADIOLIB_CLOCK_DRIFT_MS                         (0)
#endif

#if !defined(RADIOLIB_LINE_FEED)
  #define RADIOLIB_LINE_FEED    "\r\n"
#endif

#include <stdio.h>
#define RADIOLIB_BUILD_GENERIC

#define RADIOLIB_PLATFORM                           "Generic"

#define RADIOLIB_NC                                 (0xFFFFFFFF)
#define RADIOLIB_NONVOLATILE
#define RADIOLIB_NONVOLATILE_READ_BYTE(addr)        (*(reinterpret_cast<uint8_t *>(reinterpret_cast<void *>(addr))))
#define RADIOLIB_NONVOLATILE_READ_DWORD(addr)       (*(reinterpret_cast<uint32_t *>(reinterpret_cast<void *>(addr))))
#define RADIOLIB_TYPE_ALIAS(type, alias)            using alias = type;

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#include <stdint.h>

// This only compiles on STM32 boards with SUBGHZ module, but also
// include when generating docs
#if (!defined(ARDUINO_ARCH_STM32) || !defined(SUBGHZSPI_BASE)) && !defined(DOXYGEN)
  #define RADIOLIB_EXCLUDE_STM32WLX (1)
#endif

// if verbose assert is enabled, enable basic debug too
#if RADIOLIB_VERBOSE_ASSERT
  #define RADIOLIB_DEBUG  (1)
#endif

// set the global debug mode flag
#if RADIOLIB_DEBUG_BASIC || RADIOLIB_DEBUG_PROTOCOL || RADIOLIB_DEBUG_SPI
  #define RADIOLIB_DEBUG  (1)
#else
  #define RADIOLIB_DEBUG  (0)
#endif

#if RADIOLIB_DEBUG
  #if !defined(RADIOLIB_DEBUG_PRINT)
    #define RADIOLIB_DEBUG_PRINT(M, ...) rlb_printf(false, M, ##__VA_ARGS__)
    #define RADIOLIB_DEBUG_PRINT_LVL(LEVEL, M, ...) rlb_printf(true, LEVEL "" M, ##__VA_ARGS__)
  #endif

  #if !defined(RADIOLIB_DEBUG_PRINTLN)
    #define RADIOLIB_DEBUG_PRINTLN(M, ...) rlb_printf(false, M RADIOLIB_LINE_FEED, ##__VA_ARGS__)
    #define RADIOLIB_DEBUG_PRINTLN_LVL(LEVEL, M, ...) rlb_printf(true, LEVEL "" M RADIOLIB_LINE_FEED, ##__VA_ARGS__)
  #endif

  #define RADIOLIB_DEBUG_PRINT_FLOAT(LEVEL, VAL, DECIMALS) RADIOLIB_DEBUG_PRINT(LEVEL "%.3f", VAL)

  #define RADIOLIB_DEBUG_HEXDUMP(LEVEL, ...) rlb_hexdump(LEVEL, __VA_ARGS__)
#else
  #define RADIOLIB_DEBUG_PRINT(...) {}
  #define RADIOLIB_DEBUG_PRINTLN(...) {}
  #define RADIOLIB_DEBUG_PRINT_FLOAT(LEVEL, VAL, DECIMALS) {}
  #define RADIOLIB_DEBUG_HEXDUMP(...) {}
#endif

#define RADIOLIB_DEBUG_TAG            ": "
#define RADIOLIB_DEBUG_TAG_BASIC      "RLB_DBG" RADIOLIB_DEBUG_TAG
#define RADIOLIB_DEBUG_TAG_PROTOCOL   "RLB_PRO" RADIOLIB_DEBUG_TAG
#define RADIOLIB_DEBUG_TAG_SPI        "RLB_SPI" RADIOLIB_DEBUG_TAG

#if RADIOLIB_DEBUG_BASIC
  #define RADIOLIB_DEBUG_BASIC_PRINT(...)         RADIOLIB_DEBUG_PRINT_LVL(RADIOLIB_DEBUG_TAG_BASIC, __VA_ARGS__)
  #define RADIOLIB_DEBUG_BASIC_PRINTLN(...)       RADIOLIB_DEBUG_PRINTLN_LVL(RADIOLIB_DEBUG_TAG_BASIC, __VA_ARGS__)
  #define RADIOLIB_DEBUG_BASIC_HEXDUMP(...)       RADIOLIB_DEBUG_HEXDUMP(RADIOLIB_DEBUG_TAG_BASIC, __VA_ARGS__)
  #define RADIOLIB_DEBUG_BASIC_PRINT_FLOAT(...)   RADIOLIB_DEBUG_PRINT_FLOAT(RADIOLIB_DEBUG_TAG_BASIC, __VA_ARGS__)
  #define RADIOLIB_DEBUG_BASIC_PRINT_NOTAG(...)   RADIOLIB_DEBUG_PRINT(__VA_ARGS__)
  #define RADIOLIB_DEBUG_BASIC_PRINTLN_NOTAG(...) RADIOLIB_DEBUG_PRINTLN(__VA_ARGS__)
#else
  #define RADIOLIB_DEBUG_BASIC_PRINT(...) {}
  #define RADIOLIB_DEBUG_BASIC_PRINTLN(...) {}
  #define RADIOLIB_DEBUG_BASIC_HEXDUMP(...) {}
  #define RADIOLIB_DEBUG_BASIC_PRINT_FLOAT(...) {}
  #define RADIOLIB_DEBUG_BASIC_PRINT_NOTAG(...) {}
  #define RADIOLIB_DEBUG_BASIC_PRINTLN_NOTAG(...) {}
#endif

#if RADIOLIB_DEBUG_PROTOCOL
  #define RADIOLIB_DEBUG_PROTOCOL_PRINT(...)          RADIOLIB_DEBUG_PRINT_LVL(RADIOLIB_DEBUG_TAG_PROTOCOL, __VA_ARGS__)
  #define RADIOLIB_DEBUG_PROTOCOL_PRINTLN(...)        RADIOLIB_DEBUG_PRINTLN_LVL(RADIOLIB_DEBUG_TAG_PROTOCOL, __VA_ARGS__)
  #define RADIOLIB_DEBUG_PROTOCOL_HEXDUMP(...)        RADIOLIB_DEBUG_HEXDUMP(RADIOLIB_DEBUG_TAG_PROTOCOL, __VA_ARGS__)
  #define RADIOLIB_DEBUG_PROTOCOL_PRINT_FLOAT(...)    RADIOLIB_DEBUG_PRINT_FLOAT(RADIOLIB_DEBUG_TAG_PROTOCOL, __VA_ARGS__)
  #define RADIOLIB_DEBUG_PROTOCOL_PRINT_NOTAG(...)    RADIOLIB_DEBUG_PRINT(__VA_ARGS__)
  #define RADIOLIB_DEBUG_PROTOCOL_PRINTLN_NOTAG(...)  RADIOLIB_DEBUG_PRINTLN(__VA_ARGS__)
#else
  #define RADIOLIB_DEBUG_PROTOCOL_PRINT(...) {}
  #define RADIOLIB_DEBUG_PROTOCOL_PRINTLN(...) {}
  #define RADIOLIB_DEBUG_PROTOCOL_HEXDUMP(...) {}
  #define RADIOLIB_DEBUG_PROTOCOL_PRINT_FLOAT(...) {}
  #define RADIOLIB_DEBUG_PROTOCOL_PRINT_NOTAG(...) {}
  #define RADIOLIB_DEBUG_PROTOCOL_PRINTLN_NOTAG(...) {}
#endif

#if RADIOLIB_DEBUG_SPI
  #define RADIOLIB_DEBUG_SPI_PRINT(...)           RADIOLIB_DEBUG_PRINT_LVL(RADIOLIB_DEBUG_TAG_SPI, __VA_ARGS__)
  #define RADIOLIB_DEBUG_SPI_PRINTLN(...)         RADIOLIB_DEBUG_PRINTLN_LVL(RADIOLIB_DEBUG_TAG_SPI, __VA_ARGS__)
  #define RADIOLIB_DEBUG_SPI_HEXDUMP(...)         RADIOLIB_DEBUG_HEXDUMP(RADIOLIB_DEBUG_TAG_SPI, __VA_ARGS__)
  #define RADIOLIB_DEBUG_SPI_PRINT_FLOAT(...)     RADIOLIB_DEBUG_PRINT_FLOAT(RADIOLIB_DEBUG_TAG_SPI, __VA_ARGS__)
  #define RADIOLIB_DEBUG_SPI_PRINT_NOTAG(...)     RADIOLIB_DEBUG_PRINT(__VA_ARGS__)
  #define RADIOLIB_DEBUG_SPI_PRINTLN_NOTAG(...)   RADIOLIB_DEBUG_PRINTLN(__VA_ARGS__)
#else
  #define RADIOLIB_DEBUG_SPI_PRINT(...) {}
  #define RADIOLIB_DEBUG_SPI_PRINTLN(...) {}
  #define RADIOLIB_DEBUG_SPI_HEXDUMP(...) {}
  #define RADIOLIB_DEBUG_SPI_PRINT_FLOAT(...) {}
  #define RADIOLIB_DEBUG_SPI_PRINT_NOTAG(...) {}
  #define RADIOLIB_DEBUG_SPI_PRINTLN_NOTAG(...) {}
#endif

// debug info strings
#define RADIOLIB_VALUE_TO_STRING(x) #x
#define RADIOLIB_VALUE(x) RADIOLIB_VALUE_TO_STRING(x)

#define RADIOLIB_INFO "\r\nRadioLib Info\nVersion:  \"" \
  RADIOLIB_VALUE(RADIOLIB_VERSION_MAJOR) "." \
  RADIOLIB_VALUE(RADIOLIB_VERSION_MINOR) "." \
  RADIOLIB_VALUE(RADIOLIB_VERSION_PATCH) "." \
  RADIOLIB_VALUE(RADIOLIB_VERSION_EXTRA) "\"\r\n" \
  "Platform: " RADIOLIB_VALUE(RADIOLIB_PLATFORM) "\r\n" \
  RADIOLIB_VALUE(__DATE__) " " RADIOLIB_VALUE(__TIME__)

/*!
  \brief A simple assert macro, will return on error.
  If RADIOLIB_VERBOSE_ASSERT is enabled, the macro will also print out file and line number trace,
  at a significant program storage cost.
*/
#if RADIOLIB_VERBOSE_ASSERT
#define RADIOLIB_ASSERT(STATEVAR) { if((STATEVAR) != RADIOLIB_ERR_NONE) { RADIOLIB_DEBUG_BASIC_PRINTLN("%d at %s:%d", STATEVAR, __FILE__, __LINE__); return(STATEVAR); } }
#define RADIOLIB_ASSERT_PTR(PTR) { if((PTR) == NULL) { RADIOLIB_DEBUG_BASIC_PRINTLN("NULL at %s:%d", __FILE__, __LINE__); return(RADIOLIB_ERR_MEMORY_ALLOCATION_FAILED); } }
#else
#define RADIOLIB_ASSERT(STATEVAR) { if((STATEVAR) != RADIOLIB_ERR_NONE) { return(STATEVAR); } }
#define RADIOLIB_ASSERT_PTR(PTR) { if((PTR) == NULL) { return(RADIOLIB_ERR_MEMORY_ALLOCATION_FAILED); } }
#endif

/*!
  \brief Macro to check variable is within constraints - this is commonly used to check parameter ranges. Requires RADIOLIB_CHECK_RANGE to be enabled
*/
#if RADIOLIB_CHECK_PARAMS
  #define RADIOLIB_CHECK_RANGE(VAR, MIN, MAX, ERR) { if(!(((VAR) >= (MIN)) && ((VAR) <= (MAX)))) { return(ERR); } }
#else
  #define RADIOLIB_CHECK_RANGE(VAR, MIN, MAX, ERR) {}
#endif

#if RADIOLIB_FIX_ERRATA_SX127X
  #define RADIOLIB_ERRATA_SX127X(...) { errataFix(__VA_ARGS__); }
#else
  #define RADIOLIB_ERRATA_SX127X(...) {}
#endif

// these macros are usually defined by Arduino, but some platforms undef them, so its safer to use our own
#define RADIOLIB_MIN(a,b)				((a)<(b)?(a):(b))
#define RADIOLIB_MAX(a,b)				((a)>(b)?(a):(b))
#define RADIOLIB_ABS(x)         ((x)>0?(x):-(x))

// version definitions
#define RADIOLIB_VERSION_MAJOR  7
#define RADIOLIB_VERSION_MINOR  3
#define RADIOLIB_VERSION_PATCH  0
#define RADIOLIB_VERSION_EXTRA  0

#define RADIOLIB_VERSION (((RADIOLIB_VERSION_MAJOR) << 24) | ((RADIOLIB_VERSION_MINOR) << 16) | ((RADIOLIB_VERSION_PATCH) << 8) | (RADIOLIB_VERSION_EXTRA))

#endif
