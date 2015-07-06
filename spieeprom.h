/**
 * spieeprom.h - library for SPI EEPROM IC's
 * https://bitbucket.org/trunet/spieeprom/
 * 
 * This library is based on code by Heather Dewey-Hagborg
 * available on http://www.arduino.cc/en/Tutorial/SPIEEPROM
 * 
 * by Wagner Sartori Junior <wsartori@gmail.com>
 *
 * Modified by David Beaudette
 *
 **/

#ifndef SPIEEPROM_h
#define SPIEEPROM_h

#include <Arduino.h>
#include <SPI.h>

// Status register bits:
// 0: Write-in-process (read-only)
// 1: Write enable latch (read-only)
// 2: Block protection 0
// 3: Block protection 1 
// 4-6: Unused
// 7: WPEN 
  
// Instruction set
#define SPIEEPROM_READ  0x03 // Read
#define SPIEEPROM_WRITE 0x02 // Write
#define SPIEEPROM_WREN  0x06 // Write enable
#define SPIEEPROM_WRDI  0x04 // Write disable
#define SPIEEPROM_RDSR  0x05 // Read status register
#define SPIEEPROM_WRSR  0x01 // Write status register
#define SPIEEPROM_PE    0x42 // Page erase
#define SPIEEPROM_SE    0xD8 // Sector erase
#define SPIEEPROM_CE    0xC7 // Chip erase
#define SPIEEPROM_RDID  0xAB // Release from Deep power-down and read electronic signature
#define SPIEEPROM_DPD   0xB9 // Deep Power-Down mode

// Timing values
#define SPIEEPROM_TREL   100 // Max /CS high to standby mode time (us)

class SPIEEPROM
{
  public:
    SPIEEPROM();

    // Hardware setup
    void setup(int pin_num);
    
    // Memory write
    void write(long addr, byte data);
    void write(long addr, byte data[], int arrLength);
    
    // Memory read
    byte read_byte(long addr);
    void read_byte_array(long addr, byte data[], int arrLength);
    
    // Manufacturer ID 
    byte get_manufacturer_id(); 
    
    // Deep power-down mode
    void enter_dpd();
    void release_dpd();
    
    void erase_page(unsigned int page_num);
    void erase_sector(unsigned int sector_num);
    void erase_chip();
    
    // Array protection
    void protect_none();
    void protect_upper_quarter();
    void protect_upper_half();
    void protect_all();
    
    // Hardware protection
    void enable_hw_protect();
    void disable_hw_protect();
    
    bool isWIP();  // is write in progress?
    bool isWPEN(); // is write protection enabled?
    bool isBP0();  // is block protection bit 0 enabled?
    bool isBP1();  // is block protection bit 1 enabled?
    
  private:
    byte eeprom_type;
    int  slave_select;
    
    // Status register
    byte read_status();
    void write_status(byte status);

    // Recurrent actions
    void send_address(long addr);
    void enable_write();
    void start_write();
};

#endif // SPIEEPROM_h
