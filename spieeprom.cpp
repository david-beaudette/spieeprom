/**
 * spieeprom.cpp - library for SPI EEPROM IC's
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

#include "spieeprom.h"

SPIEEPROM::SPIEEPROM() {
  
}

void SPIEEPROM::setup(int pin_num) {
  this->slave_select = pin_num;
	pinMode(this->slave_select, OUTPUT);
	digitalWrite(this->slave_select, HIGH);
	SPI.begin();
}

void SPIEEPROM::send_address(long addr) {
  // Transfer 24 bits address
  SPI.transfer((byte)(addr>>16));
	SPI.transfer((byte)(addr>>8));
	SPI.transfer((byte)(addr));
}

void SPIEEPROM::enable_write() {
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_WREN); //send WREN command
	digitalWrite(this->slave_select, HIGH);
}

void SPIEEPROM::start_write() {
  // Wait until any previous write is finished
  while(isWIP());
	enable_write();
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_WRITE); //send WRITE command
}

byte SPIEEPROM::read_status() {
	byte data;
	
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_RDSR); // send RDSR command
	
	data = SPI.transfer(0xFF); //get data byte
	
	digitalWrite(this->slave_select, HIGH);
	
	return data;
}

bool SPIEEPROM::isWIP() {
	byte data = read_status();
  
	return (data & (1 << 0));
}

bool SPIEEPROM::isWPEN() {
	byte data = read_status();
  
	return (data & (1 << 7));
}

bool SPIEEPROM::isBP0() {
	byte data = read_status();
  
	return (data & (1 << 2));
}

bool SPIEEPROM::isBP1() {
	byte data = read_status();
  
	return (data & (1 << 3));
}

void SPIEEPROM::write(long addr, byte data) {
	start_write();
	
	send_address(addr); // send address
	SPI.transfer(data); // transfer data
	
	digitalWrite(this->slave_select, HIGH);
	
	while (isWIP()) {
		delay(1);
	}
}

void SPIEEPROM::write(long addr, byte data[], int arrLength) {
	start_write();
	
	send_address(addr); // send address

	for (int i=0;i<arrLength;i++) {
		SPI.transfer(data[i]); // transfer data
	}
	
	digitalWrite(this->slave_select, HIGH);
	while (isWIP()) {
		delay(1);
	}
}

byte SPIEEPROM::read_byte(long addr) {
	byte data;
	
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_READ); // send READ command
	
	send_address(addr); // send address
	data = SPI.transfer(0xFF); // get data byte
	
	digitalWrite(this->slave_select, HIGH); // release chip, signal end transfer
	
	return data;
}

void SPIEEPROM::read_byte_array(long addr, byte data[], int arrLength) {
	
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_READ); // send READ command
	
	send_address(addr); // send address
  for(int i = 0; i < arrLength; i++) {
    data[i] = SPI.transfer(0xFF); // get data byte
  }
	digitalWrite(this->slave_select, HIGH); // release chip, signal end transfer
}

void SPIEEPROM::erase_chip() {
  // Wait until any previous write is finished
  while(isWIP());
  enable_write();
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_CE); // send Chip erase command
	digitalWrite(this->slave_select, HIGH);  
}

void SPIEEPROM::erase_sector(unsigned int sector_num) {
  // Build an address in sector
  long addr = sector_num;
  addr = addr << 15;  
  
  // Wait until any previous write is finished
  while(isWIP());
  enable_write();
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_SE); // send sector erase command
	send_address(addr); // send Chip erase command
	digitalWrite(this->slave_select, HIGH);  
}

void SPIEEPROM::protect_none() {
  // BP0  = 0
  // BP1  = 0
  byte data = read_status();
  // Overwrite BPn bits
  data = data & 0xF3;
  write_status(data);
}
void SPIEEPROM::protect_upper_quarter() {
  // BP0  = 1
  // BP1  = 0
  byte data = read_status();
  // Overwrite BPn bits
  data = data & 0xF3;
  data = data | 0x4; 
  write_status(data);
}
void SPIEEPROM::protect_upper_half() {
  // BP0  = 0
  // BP1  = 1
  byte data = read_status();
  // Overwrite BPn bits
  data = data & 0xF3;
  data = data | 0x8; 
  write_status(data);
}
void SPIEEPROM::protect_all() {
  // BP0  = 1
  // BP1  = 1
  byte data = read_status();
  // Overwrite BPn bits
  data = data | 0xC; 
  write_status(data);
}

void SPIEEPROM::enable_hw_protect() {
  byte data = read_status();
  // Overwrite WPEN bit
  data = data | 0x80;
  write_status(data);
}

void SPIEEPROM::disable_hw_protect() {
  byte data = read_status();
  // Overwrite WPEN bit
  data = data & 0x7F;
  write_status(data);
}

void SPIEEPROM::write_status(byte status) {  
  // Wait until any previous write is finished
  while(isWIP());
  // Send WREN command
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_WREN); 
	digitalWrite(this->slave_select, HIGH);
  
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_WRSR); // send RDSR command
	
	SPI.transfer(status); // set status
	
	digitalWrite(this->slave_select, HIGH);
}

byte SPIEEPROM::get_manufacturer_id() {
	byte data;
	
	digitalWrite(this->slave_select, LOW);
  
  // Send release from deep power-down and 
  // read electronic signature command
	SPI.transfer(SPIEEPROM_RDID); 
  
  // Send 3 dummy address bytes
  for(int i = 0; i < 3; i++) {
		SPI.transfer(0xFF);
  }
  // Get manufacturer ID (1 byte)
	data = SPI.transfer(0xFF); 
	
  // Release chip, signal end transfer
	digitalWrite(this->slave_select, HIGH); 
	
	return data;
}

void SPIEEPROM::enter_dpd() {
	digitalWrite(this->slave_select, LOW);
  
  // Send deep power-down command
	SPI.transfer(SPIEEPROM_DPD); 
  
  // Release chip, signal end transfer
	digitalWrite(this->slave_select, HIGH); 
}

void SPIEEPROM::release_dpd() {
	digitalWrite(this->slave_select, LOW);
  
  // Send release from deep power-down and 
  // read electronic signature command
	SPI.transfer(SPIEEPROM_RDID); 
  
  // Release chip, signal end transfer
	digitalWrite(this->slave_select, HIGH); 
	
  // Device enters standby mode after a certain time
  delayMicroseconds(SPIEEPROM_TREL);
}