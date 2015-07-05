/*
 * spieeprom.h - library for SPI EEPROM IC's
 * https://bitbucket.org/trunet/spieeprom/
 * 
 * This library is based on code by Heather Dewey-Hagborg
 * available on http://www.arduino.cc/en/Tutorial/SPIEEPROM
 * 
 * by Wagner Sartori Junior <wsartori@gmail.com>
 */

#ifndef SPIEEPROM_h
#define SPIEEPROM_h

#include <Arduino.h>
#include <SPI.h>

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
#define SPIEEPROM_DPD   0xB9 //Deep Power-Down mode

class SPIEEPROM
{
  public:
    SPIEEPROM(); // defaults to type 0
    SPIEEPROM(byte type); // type=0: 16-bits address
                          // type=1: 24-bits address
                          // type>1: defaults to type 0
              
    void setup(int pin_num);

    void write(long addr, byte data);
    void write(long addr, byte data[], int arrLength);
    //void write(long addr, char data);
    void write(long addr, char data[], int arrLength);
    //void write(long addr, int data);
    //void write(long addr, long data);
    //void write(long addr, float data);
    
    byte read_byte(long addr);
    void read_byte_array(long addr, byte data[], int arrLength);
    char read_char(long addr);
    
    //void erase_page(int sector_num);
    //void erase_sector(int sector_num);
    void erase_chip();
    void protect();
    void unprotect();
    
  private:
    long address;
    byte eeprom_type;
    int  slave_select;
    
    void send_address(long addr);
    void start_write();
    bool isWIP(); // is write in progress?
    bool write_status(byte status); // write to status register
    
};

#endif // SPIEEPROM_h
