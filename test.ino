// only tested on teensy 4.1 so far

// from eeprom.c
extern "C" void eepromemu_flash_write(void *addr, const void *data, uint32_t len);
//extern "C" void eepromemu_flash_erase_sector(void *addr);
extern "C" void eepromemu_flash_erase_64K_block(void *addr);

#define FLASH_MEMORY_PAGE_SIZE 65536

typedef struct __attribute__((packed))
{
    uint8_t SomeData[10];
    // maybe pad here to make sure we can erase a full flash page without hitting anything else
    // brute force method: pad it with a full flash page worth of unused data
    unsigned char padding [FLASH_MEMORY_PAGE_SIZE];
} AStruct;

PROGMEM AStruct __attribute__((aligned (FLASH_MEMORY_PAGE_SIZE))) StructInstance;

void setup() {
  while (!Serial) {}
  delay(1000);
  
  testSingleBlockWrite();
  testMultipleBlockWrites();
}

void loop() {
}

void testSingleBlockWrite() {
  Serial.println("### single block write");
  eraseBlock();
  printTheData();
  eraseBlock();
  // write some test data to the middle
  // should result in FF FF 01 02 03 04 05 06 FF FF
  uint8_t data[] = {1,2,3,4,5,6};
  int offset = 2;
  programData(offset, &data, sizeof(data));
  printTheData();
}

void testMultipleBlockWrites() {
  Serial.println("### multiple block writes");
  eraseBlock();
  printTheData();
  eraseBlock();
  uint8_t data[] = {1,2};
  int offset = 0;
  programData(offset, &data, sizeof(data));

  uint8_t data2[] = {3,4};
  int offset2 = 8;
  programData(offset2, &data2, sizeof(data2));

  printTheData();
}

void printTheData() {
  Serial.println("Current data:");
  for (int i=0; i<10; i++) {
    Serial.print(StructInstance.SomeData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

int programData(int offset, const void *buffer, int size)
{
  //Serial.printf("   prog wr: block=%d, offset=%d, size=%d\n", block, offset, size);
  uint8_t *p = (uint8_t *)(&StructInstance) + offset;
  eepromemu_flash_write(p, buffer, size);
  return 0;
}

// uploading will have initialised it to all 0x00's, so wipe it ready for writing
// erasing turns it into all 0xFF's which can then be written
void eraseBlock()
{
  //Serial.printf("   prog er: block=%d\n", block);
  uint8_t *p = (uint8_t *)&StructInstance;
  eepromemu_flash_erase_64K_block(p);
}
