/** La Fabrique

   spieeprom library test 
   by David Beaudette
   
   Tests the spieeprom library methods.
   
**/
#include <SPI.h>
#include "spieeprom.h"

#define PAGE_SIZE        256 // Number of bytes per page  
#define NUM_PAGES        512 // Number of pages in EEPROM  
#define EEPROM_SIZE   131072 // Total memory size in bytes
                             
// Declare memory instance as type 1 (24 bits address)
SPIEEPROM memory;

// Test configuration options
// Reset memory, clearing previous data 
const int erase_chip = 1;
// Fill memory with data
const int fill_memory = 1;
// Print memory content on serial
const int print_memory = 1;
// Verify data content
const int check_write = 1;


void setup() {
  // Setup serial communication  
  Serial.begin(115200);
  
  Serial.println(F("SPIEEPROM class test"));
  Serial.println(F("---------")); 
  
  // Setup chip on pin 19 
  memory.setup(19);
}

void loop() {
  int test_ok = 1;
  unsigned long t_start, t_stop;
  int t_diff;
  byte page_data_in[PAGE_SIZE];
  byte page_data_out[PAGE_SIZE];
  long addr;
  
  // Wait for user input
  Serial.println(F("Send character to perform memory checks."));
  while(Serial.available() <= 0) {
    delay(500);
  }  
  // Self-test the device
  byte ID = memory.get_manufacturer_id();
  Serial.print(F("Manufacturer ID is "));
  Serial.print(ID);
  if(ID != 0x29) {
    Serial.println(F(" which is not expected for 24LC1024."));
    test_ok = 0;
  } 
  else {
    Serial.println(F(" as expected."));
  }
  
  // Overwrite BPn values in status register
  memory.protect_none();
  t_start = micros();
  while(memory.isWIP());
  t_stop = micros();
  Serial.println();
  Serial.print(F("Writing 0 to BP0 and BP1 took roughly "));
  t_diff = t_stop - t_start;
  Serial.print(t_diff);
  Serial.println(F(" us."));
  
  if(memory.isBP0()) {
    Serial.println(F("  Error: Write protection bit 0 is enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection bit 0 is disabled."));
  }
  if(memory.isBP1()) {
    Serial.println(F("  Error: Write protection bit 1 is enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection bit 1 is disabled."));
  }
  
  // Overwrite hardware data protection bit in status register
  memory.disable_hw_protect();
  t_start = micros();
  while(memory.isWIP());
  t_stop = micros();
  Serial.println();
  Serial.print(F("Disabling HW protection took roughly "));
  t_diff = t_stop - t_start;
  Serial.print(t_diff);
  Serial.println(F(" us."));
  
  if(memory.isWPEN()) {
    Serial.println(F("  Error: Write protection is globally enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection is globally disabled."));
  }
  
  // Check protection bits
  Serial.println();
  Serial.println(F("Protecting upper memory quarter."));
  memory.protect_upper_quarter();
  while(memory.isWIP());
  if(!memory.isBP0()) {
    Serial.println(F("  Error: Write protection bit 0 was not enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection bit 0 was enabled."));
  }
  if(memory.isBP1()) {
    Serial.println(F("  Error: Write protection bit 1 is enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection bit 1 is disabled."));
  }
  if(memory.isWPEN()) {
    Serial.println(F("  Error: HW write protection bit is enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: HW write protection bit is still disabled."));
  }
  
  // Attempt to erase in Sector 3
  memory.erase_sector(3);
  if(memory.isWIP()) {
    Serial.println(F("  Error: Sector 3 erased with protection enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Sector 3 not written with protection enabled."));
  }  

  // Check protection bits
  Serial.println();
  Serial.println(F("Protecting upper memory half."));
  memory.protect_upper_half();
  while(memory.isWIP());
  if(memory.isBP0()) {
    Serial.println(F("  Error: Write protection bit 0 was enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection bit 0 was not enabled."));
  }
  if(!memory.isBP1()) {
    Serial.println(F("  Error: Write protection bit 1 was not enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection bit 1 was enabled."));
  }
  if(memory.isWPEN()) {
    Serial.println(F("  Error: HW write protection bit is enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: HW write protection bit is still disabled."));
  }
  
  // Attempt to erase in Sector 2
  memory.erase_sector(2);
  if(memory.isWIP()) {
    Serial.println(F("  Error: Sector 2 erased with protection enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Sector 2 not written with protection enabled."));
  }  

  // Check protection bits
  Serial.println();
  Serial.println(F("Protecting all memory."));
  memory.protect_all();
  while(memory.isWIP());
  if(!memory.isBP0()) {
    Serial.println(F("  Error: Write protection bit 0 was not enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection bit 0 was enabled."));
  }
  if(!memory.isBP1()) {
    Serial.println(F("  Error: Write protection bit 1 was not enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Write protection bit 1 was enabled."));
  }
  if(memory.isWPEN()) {
    Serial.println(F("  Error: HW write protection bit is enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: HW write protection bit is still disabled."));
  }
  
  // Attempt to erase in Sector 0 and 1
  memory.erase_sector(0);
  if(memory.isWIP()) {
    Serial.println(F("  Error: Sector 0 erased with protection enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Sector 0 not written with protection enabled."));
  }  

  memory.erase_sector(1);
  if(memory.isWIP()) {
    Serial.println(F("  Error: Sector 1 erased with protection enabled."));
    test_ok = 0;
  } 
  else {
    Serial.println(F("  Ok: Sector 1 not written with protection enabled."));
  }  

  // Remove protection 
  Serial.println();
  Serial.println(F("Removing memory protection."));
  memory.protect_none();
  
  // Write data on page 15-16
  Serial.println();
  Serial.println(F("Writing on page 15-16."));
  for(int i = 0; i < PAGE_SIZE; i++) {
    page_data_out[i] = i;
  }
  memory.write(0xF00, page_data_out, PAGE_SIZE);
  t_start = micros();
  if(!memory.isWIP()) {
    Serial.println(F("  Error: Writing page should set WIP bit."));
    test_ok = 0;
  } 
  while(memory.isWIP());
  t_stop = micros();
  Serial.print(F("  Writing page 15 took roughly "));
  t_diff = t_stop - t_start;
  Serial.print(t_diff);
  Serial.println(F(" us."));
  
  memory.write(0x1000, page_data_out, PAGE_SIZE);
  t_start = micros();
  if(!memory.isWIP()) {
    Serial.println(F("  Error: Writing page should set WIP bit."));
    test_ok = 0;
  } 
  while(memory.isWIP());
  t_stop = micros();
  Serial.print(F("  Writing page 16 took roughly "));
  t_diff = t_stop - t_start;
  Serial.print(t_diff);
  Serial.println(F(" us."));
  
  // Read data on page 15-16
  Serial.println();
  Serial.println(F("Reading on page 15 and 16."));
  memory.read_byte_array(0xF00, page_data_in, PAGE_SIZE);
  for(int i = 0; i < PAGE_SIZE; i++) {
    if(page_data_in[i] != page_data_out[i]) {
      test_ok = 0;
    }
  }
  memory.read_byte_array(0x1000, page_data_in, PAGE_SIZE);
  for(int i = 0; i < PAGE_SIZE; i++) {
    if(page_data_in[i] != page_data_out[i]) {
      test_ok = 0;
    }
  }
  if(!test_ok) {
    Serial.println(F("  Error: Page 15-16 data was not written."));
  }
  else {
    Serial.println(F("  Ok: Page 15-16 data was written and read back successfully."));
  }
  // Erase page 15
  Serial.println();
  Serial.println(F("Erasing page 15."));
  memory.erase_page(15);
  t_start = micros();
  if(!memory.isWIP()) {
    Serial.println(F("  Error: Sector erasing should set WIP bit."));
    test_ok = 0;
  } 
  while(memory.isWIP());
  t_stop = micros();
  Serial.print(F("  Erasing page 15 took roughly "));
  t_diff = t_stop - t_start;
  Serial.print(t_diff);
  Serial.println(F(" us."));

  // Read data on page 15-16
  Serial.println();
  Serial.println(F("Reading on page 15 and 16."));
  memory.read_byte_array(0xF00, page_data_in, PAGE_SIZE);
  for(int i = 0; i < PAGE_SIZE; i++) {
    if(page_data_in[i] != 0xFF) {
      Serial.println(page_data_in[i]);
      test_ok = 0;
    }
  }
  if(!test_ok) {
    Serial.println(F("  Error: Page 15 data was not erased."));
    while(1);
  }
  else {
    Serial.println(F("  Ok: Page 15 data was erased."));
  }
  memory.read_byte_array(0x1000, page_data_in, PAGE_SIZE);
  for(int i = 0; i < PAGE_SIZE; i++) {
    if(page_data_in[i] != page_data_out[i]) {
      test_ok = 0;
    }
  }
  if(!test_ok) {
    Serial.println(F("  Error: Page 16 data was also erased."));
    while(1);
  }
  else {
    Serial.println(F("  Ok: Page 16 data was not erased."));
  }
  
  // Erase sector 0
  Serial.println();
  Serial.println(F("Erasing sector 0."));
  memory.erase_sector(0);
  t_start = micros();
  if(!memory.isWIP()) {
    Serial.println(F("  Error: Sector erasing should set WIP bit."));
    test_ok = 0;
  } 
  while(memory.isWIP());
  t_stop = micros();
  Serial.print(F("  Erasing sector 0 took roughly "));
  t_diff = t_stop - t_start;
  Serial.print(t_diff);
  Serial.println(F(" us."));
  
  Serial.println();
  Serial.println(F("Reading on page 16."));
  memory.read_byte_array(0x1000, page_data_in, PAGE_SIZE);
  for(int i = 0; i < PAGE_SIZE; i++) {
    if(page_data_in[i] != 0xFF) {
      test_ok = 0;
    }
  }
  if(!test_ok) {
    Serial.println(F("  Error: Page 16 data was not erased."));
    while(1);
  }
  else {
    Serial.println(F("  Ok: Page 16 data was erased."));
  }
  
  // Display test conclusion
  Serial.println();
  if(test_ok) {
    Serial.println(F("Test succeeded."));
  }
  else {
    Serial.println(F("Test failed"));
  }

  // Test complete
  while(1);  
}

