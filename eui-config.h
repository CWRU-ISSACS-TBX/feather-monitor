#ifndef EUI_CONFIG_H
#define EUI_CONFIG_H

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from Helium output, this means to reverse
// the bytes.
static const u1_t PROGMEM APPEUI[8] = { 0xA2, 0xED, 0xFB, 0xB9, 0x16, 0x39, 0xD2, 0xD1 };

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = { 0x2C, 0xB4, 0xC7, 0xBF, 0xE9, 0x0C, 0xBD, 0x1E };

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the Helium console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { 0x99, 0xB1, 0x81, 0x18, 0xF1, 0xD1, 0x97, 0x16, 0x14, 0x19, 0x9A, 0x4D, 0xB4, 0xD7, 0xF1, 0x43 };

#endif
