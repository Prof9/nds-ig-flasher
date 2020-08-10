#include <stdio.h>
#include "nds.h"
#include "fat.h"

#define BUFFER_SIZE		256
#define ROM_PATH		"fat:/ig/flash.gba"
#define CHIP_ID			(0x227E0089) // 89 00 7E 22
#define VERSION			"v0.1"

u8 _buffer[BUFFER_SIZE] __attribute__ ((aligned (16)));

inline vu16 readRomU16(u32 address) {
	return ((vu16*)GBAROM)[address / 2];
}
inline vu8 readRomU8(u32 address) {
	if (address & 1) {
		return readRomU16(address) >> 8;
	} else {
		return readRomU16(address);
	}
}
inline vu32 readRomU32(u32 address) {
	vu32 lo = readRomU16(address);
	vu32 hi = readRomU16(address+2);
	return (hi << 16) | lo;
}
void readRomArray(u32 address, u8* dst, u16 len) {
	vu16 x;
	while (len >= 2) {
		x = readRomU16(address);
		dst[0] = x;
		dst[1] = x >> 8;
		address += 2;
		len -= 2;
		dst += 2;
	}
	if (len >= 1) {
		x = readRomU16(address);
		dst[0] = x;
	}
}

void writeRomU16(u32 address, u16 val) {
	GBA_BUS[address / 2] = val;
	swiDelay(10);
}

void halt() {
	while (true) {
		swiWaitForVBlank();
	}
}

bool userConfirm() {
	iprintf("A: continue, B: cancel\n");

	while (true) {
		swiWaitForVBlank();

		scanKeys();
		switch (keysDown()) {
		case KEY_A:
			return true;
		case KEY_B:
			iprintf("Aborting...\n");
			return false;
		}
	}
}

bool getChipID(bool verify) {
	// Reset
	writeRomU16(0x000, 0xF0);

	// Read ID
	u32 id;
	writeRomU16(0xAAA, 0xAA);
	writeRomU16(0x555, 0x55);
	writeRomU16(0xAAA, 0x90);
	readRomArray(0x000, (u8*)&id, 4);
	iprintf("ID: %02X %02X %02X %02X\n",
		(u8)(id >>  0), (u8)(id >>  8),
		(u8)(id >> 16), (u8)(id >> 24));

	if (verify && id != CHIP_ID) {
		iprintf("Unknown chip ID, continue?\n");
		return userConfirm();
	}

	return true;
}

void waitForErase(u32 address) {
	while (readRomU16(address) != 0xFFFF) { };
}
void eraseRom() {
	// Reset
	writeRomU16(0x000, 0xF0);

	iprintf("Erasing... ");
	writeRomU16(0xAAA, 0xAA);
	writeRomU16(0x555, 0x55);
	writeRomU16(0xAAA, 0x80);
	writeRomU16(0xAAA, 0xAA);
	writeRomU16(0x555, 0x55);
	writeRomU16(0xAAA, 0x10);

	waitForErase(0x00);

	iprintf("Done!\n");
}

void readRom() {
	// Reset
	writeRomU16(0x000, 0xF0);

	u8 row[16];
	for (u32 i = 0; i < 16; i++) {
		readRomArray(i * 16, row, 16);
		iprintf("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			row[ 0], row[ 1], row[ 2], row[ 3], row[ 4], row[ 5], row[ 6], row[ 7],
			row[ 8], row[ 9], row[10], row[11], row[12], row[13], row[14], row[15]);
	}
}

void flashRom() {
	FILE* f = fopen(ROM_PATH, "rb");
	if (f == NULL) {
		iprintf("Error opening file\n");
		return;
	}

	// Get file size
	fseek(f, 0, SEEK_END);
	u32 fileSize = ftell(f);

	iprintf("Size: 0x%lX\n", fileSize);
	if (!userConfirm()) {
		return;
	}

	if (!getChipID(true)) {
		return;
	}

	// Erase entire ROM
	eraseRom();

	// Flash new ROM
	fseek(f, 0, SEEK_SET);
	for (u32 address = 0; address < fileSize; address += BUFFER_SIZE) {
		iprintf("Writing... 0x%lX\n", address);

		u32 read = fread(_buffer, 1, BUFFER_SIZE, f);
		if (read == 0) {
			iprintf("Error reading from file\n");
			return;
		}
		while (read < BUFFER_SIZE) {
			_buffer[read] = 0xFF;
		}

		for (u32 offset = 0; offset < BUFFER_SIZE; offset += 2) {
			u16 val = *((u16*)&_buffer[offset]);

			writeRomU16(0xAAA, 0xAA);
			writeRomU16(0x555, 0x55);
			writeRomU16(0xAAA, 0xA0);
			writeRomU16(address + offset, val);
			swiDelay(500);
		}
	}
	iprintf("Finished writing!\n");
}

int main() {
	// Initialize console
	if (consoleDemoInit() == NULL) {
		halt();
	}

	iprintf("NDS IG flasher"VERSION"\n\n");

	// Initialize FAT
	if (!fatInitDefault()) {
		iprintf("Failed to initialize FAT\n");
		halt();
	}

	// Enable SLOT-2 access
	sysSetCartOwner(true);

	iprintf("^ Read chip ID\n");
	iprintf("< Erase ROM\n");
	iprintf("> Flash "ROM_PATH"\n");
	iprintf("v Read start of ROM\n");

	while (true) {
		swiWaitForVBlank();

		scanKeys();
		switch (keysDown()) {
		case KEY_UP:
			getChipID(false);
			break;
		case KEY_LEFT:
			if (getChipID(true)) {
				eraseRom();
			}
			break;
		case KEY_RIGHT:
			flashRom();
			break;
		case KEY_DOWN:
			readRom();
			break;
		}
	}

	return 0;
}
