/* This program tries to extract the controller firmware from the nvstusb.sys
 * driver file. The program tries to read the driver file from an optional 
 * command line argument first, then from nvstusb.sys in the current directory
 * and finally on Windows it is tried to get it from an installed driver from
 * your Windows directory.
 *
 * The firmware will be written to nvstusb.fw which contains chunks of data
 * that can be sent directly to the device as vendor requests (0xA0). Also a
 * nvstusb.bin will be created containing a binary dump of the program memory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define DRIVER 0
#define FIRMWARE 1
#define BINARY 2

static const char *fileNames[3] = { "nvstusb.sys", "nvstusb.fw", "nvstusb.bin" };
static FILE *      files[3]     = {  0, 0, 0 };

/* look for these bytes in the driver file to find the firmware */
static unsigned char firmwareSignature[8] = {
  0xC2, 0x55, 0x09, 0x07, 0x00, 0x00, 0x00, 0x00
};

/* print an error, clean up and exit */
void 
error(
  int fileIndex,
  const char *format,
  ...
) {
  va_list va;
  va_start(va, format);
  fprintf(stderr, "%s: ", fileNames[fileIndex]);
  vfprintf(stderr, format, va);
  fprintf(stderr, "\n\n");
  va_end(va);

  if (0 != files[DRIVER])   fclose(files[DRIVER]);
  if (0 != files[FIRMWARE]) fclose(files[FIRMWARE]);
  if (0 != files[BINARY])   fclose(files[BINARY]);

  exit(EXIT_FAILURE);
}

/* try to open driver file */
static FILE *
openDriver(
  const char *fileName
) {
  FILE *file = fopen(fileName, "rb");
  if (0 != file) {
    fileNames[DRIVER] = fileName;
    fprintf(stderr, "%s: opened driver file\n", fileName);
  }
  return file;
}

/* try to open driver file */
static void
openOutput(
  int fileIndex
) {
  FILE *file = fopen(fileNames[fileIndex], "wb");
  if (0 == file) {
    error(fileIndex, "could not open output!");
  }
  fprintf(stderr, "%s: opened output file\n", fileNames[fileIndex]);
  files[fileIndex] = file;
}

/* seek to an absolute position or abort */
void
seekFile(
  int fileIndex,
  unsigned long pos
) {
  if (fseek(files[fileIndex], pos, SEEK_SET) != 0) {
    error(fileIndex, "could not seek to position %lu!", pos);
  }
}

/* read from a file or abort */
void 
readFile(
  int fileIndex,
  void *dest,
  size_t size
) {
  if (fread(dest, size, 1, files[fileIndex]) != 1) {
    error(fileIndex, "could not read %lu bytes!", size);
  }
} 

/* read from a file at an absolute position (or abort) */
void 
readFileAt(
  int fileIndex,
  size_t offset,
  void *dest,
  size_t size
) {
  seekFile(fileIndex, offset);
  readFile(fileIndex, dest, size);
} 

/* read a little endian word from a file at an absolute position (or abort) */
unsigned short
readFileWORD(
  int fileIndex,
  size_t offset
) {
  unsigned char buf[2];
  readFileAt(fileIndex, offset, buf, 2);
  return buf[0] | (buf[1]<<8);
}

/* read a little endian double word from a file at an absolute position (or abort) */
unsigned long
readFileDWORD(
  int fileIndex,
  size_t offset
) {
  unsigned char buf[4];
  readFileAt(fileIndex, offset, buf, 4);
  return buf[0] | (buf[1]<<8) | (buf[2]<<16) | (buf[3]<<24);
}

/* write to a file or abort */
void 
writeFile(
  int fileIndex,
  void *data,
  size_t size
) {
  if (fwrite(data, size, 1, files[fileIndex]) != 1) {
    error(fileIndex, "could not write %lu bytes!", size);
  }
} 

/* find the .data section in a portable executable file */
size_t 
findDataSection(
  int fileIndex,
  size_t *size
) {
  char buf[1024];

  /* check for MZ header */
  const char headerMZ[2] = { 'M', 'Z' };
  readFileAt(fileIndex, 0, buf, 2);
  if (memcmp(buf, headerMZ, 2) != 0) {
    error(fileIndex, "MZ header not found, not a driver file!");
  }

  /* find PE header */
  size_t peHeaderOffset = readFileDWORD(fileIndex, 0x0000003C);

  /* check for PE header */
  const char headerPE[4] = { 'P', 'E', 0, 0 };
  readFileAt(fileIndex, peHeaderOffset, buf, 4);
  if (memcmp(buf, headerPE, 4) != 0) {
    error(fileIndex, "PE header not found, not a driver file!\n");
  }

  /* read section information */
  peHeaderOffset += 4;

  unsigned short sectionCount = readFileWORD(fileIndex, peHeaderOffset + 2);
  fprintf(stderr, "found %hu sections\n", sectionCount);

  unsigned short optionalHeaderSize = readFileWORD(fileIndex, peHeaderOffset + 16);
  fprintf(stderr, "%hu bytes of optional PE header\n", optionalHeaderSize);

  size_t sectionOffset = peHeaderOffset + 20 + optionalHeaderSize;
  fprintf(stderr, "first section header at %08x\n\n", (int)sectionOffset);

  unsigned short i;
  size_t dataSectionAddress = 0;
  for (i=0; i<sectionCount; i++) {
    size_t curSectionOffset = sectionOffset + 40 * i;
    char sectionName[9];
    memset(sectionName, 0, 9);

    readFileAt(fileIndex, curSectionOffset, sectionName, 8);

    size_t curSectionSize    = readFileDWORD(fileIndex, curSectionOffset + 16);
    size_t curSectionAddress = readFileDWORD(fileIndex, curSectionOffset + 20);

    fprintf(stderr, "section %8s: %8d bytes at %08x\n", 
      sectionName, (int)curSectionSize, (int)curSectionAddress
    );

    if (0 == strcmp(sectionName, ".data")) {
      *size = curSectionSize;
      dataSectionAddress = curSectionAddress;
    }
  }
  fprintf(stderr, "\n");
  if (0 != dataSectionAddress) return dataSectionAddress;

  error(fileIndex, "could not find .data section!");
  return 0;
}

/* find the beginning of the firmware in driver file, or abort */
size_t 
findFirmware(
  int fileIndex
) {
  size_t dataSectionSize   = 0;
  size_t dataSectionOffset = findDataSection(fileIndex, &dataSectionSize);
  size_t offset;
  
  for (offset = 0; offset < dataSectionSize; offset++) {
    unsigned char buf[8];
    readFileAt(fileIndex, dataSectionOffset + offset, buf, 8);

    if (0 == memcmp(buf, firmwareSignature, 8)) {
      size_t fwOffset = dataSectionOffset + offset + 8;
      fprintf(stderr, "probably found firmware %d bytes into .data section at %08x\n\n", (int)offset, (int)fwOffset);
      return fwOffset;
    }
  }
  error(fileIndex, "could not find firmware in .data section");
  return 0;
}

int 
main(
  int argc, 
  char **argv
) {
  /* open driver file */
  if (argc > 1) {
    files[DRIVER] = openDriver(argv[1]);
  } else {
    files[DRIVER] = openDriver("nvstusb.sys");
  }
#ifdef _WIN32
  if (0 == files[DRIVER]) {
    files[DRIVER] = openDriver("c:\\Windows\\System32\\drivers\\nvstusb.sys");
  }
#endif  
  if (0 == files[DRIVER]) {
    error(DRIVER, "could not open driver");
  }

  /* open output files */
  openOutput(FIRMWARE);
  openOutput(BINARY);

  /* seek to beginning of firmware */
  seekFile(DRIVER, findFirmware(DRIVER));
  
  /* add vendor request to be automatically send before sending the firmware */
  unsigned char cfg1[5] = { 
    0x00, 0x01, /* one byte of data */
    0xE6, 0x00, /* write to 0xE600 = CPUCS */
    0x01        /* bit 0 (8051RES) set == put controller into reset. */
  };
  writeFile(FIRMWARE, cfg1, 5);

  /* initialize firmware memory dump */
  char mem[0x2000];
  memset(mem, 0, sizeof(mem));

  int block = 0;
  do {
    /* read block header */
    unsigned char lenPos[4];
    readFile(DRIVER, lenPos, 4);

    /* stop after last block */
    if (lenPos[0] & 0x80) break;

    unsigned short length  = (lenPos[0] << 8) | lenPos[1];
    unsigned short address = (lenPos[2] << 8) | lenPos[3];

    fprintf(stderr, "block %10u: %8u bytes at     %04x\n", block, length, address);

    /* read block */
    char buf[length];
    readFile(DRIVER, buf, length);
    writeFile(FIRMWARE, lenPos, 4);
    writeFile(FIRMWARE, buf, length);

    /* copy block to memory dump */
    if (address > sizeof(mem)) {
      fprintf(stderr, "  block start address seems out of range\n");
      length = 0;
    } else if (address+length > sizeof(mem)) {
      fprintf(stderr, "  block length seems out of range\n");
      length -= address+length - sizeof(mem);
    }
    memcpy(mem+address, buf, length);

    block++;
  } while(1);

  /* vendor request to clear reset flag */
  unsigned char cfg2[5] = { 
    0x00, 0x01, /* one byte of data */
    0xE6, 0x00, /* write to 0xE600 = CPUCS */
    0x00        /* bit 0 (8051RES) clear == let controller run. */
  };
  writeFile(FIRMWARE, cfg2, 5);

  /* write memory dump */
  writeFile(BINARY, mem, sizeof(mem));
  
  /* clean up */
  fclose(files[DRIVER]);    files[DRIVER] = 0;
  fclose(files[FIRMWARE]);  files[FIRMWARE] = 0;
  fclose(files[BINARY]);    files[BINARY] = 0;

  fprintf(stderr, "\ndone extracting firmware...\n");
  return EXIT_SUCCESS;
}
