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
#ifdef EEPROM_SPI  

#include "spieeprom.h"

SPIEEPROM::SPIEEPROM() {
	eeprom_type = 0;
	address = 0;
}

SPIEEPROM::SPIEEPROM(byte type) {
	if (type>1) {
		eeprom_type = 0;
	} else {
		eeprom_type = type;
	}
	address = 0;
}

void SPIEEPROM::setup(int pin_num) {
  this->slave_select = pin_num;
	pinMode(this->slave_select, OUTPUT);
	SPI.begin();
}

void SPIEEPROM::send_address(long addr) {
	if (eeprom_type == 1) {
		SPI.transfer((byte)(addr>>16));
	}
	SPI.transfer((byte)(addr>>8));
	SPI.transfer((byte)(addr));
}

void SPIEEPROM::start_write() {
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_WREN); //send WREN command
	digitalWrite(this->slave_select, HIGH);
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_WRITE); //send WRITE command
}

bool SPIEEPROM::isWIP() {
	byte data;
	
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_RDSR); // send RDSR command
	
	data = SPI.transfer(0xFF); //get data byte
	
	digitalWrite(this->slave_select, HIGH);
	
	return (data & (1 << 0));
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

void SPIEEPROM::write(long addr, char data[], int arrLength) {
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
	
	return;
}

char SPIEEPROM::read_char(long addr) {
	char data;
	
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_READ); // send READ command
	
	send_address(addr); // send address
	data = SPI.transfer(0xFF); // get data byte
	
	digitalWrite(this->slave_select, HIGH); // release chip, signal end transfer
	return data;
}

void SPIEEPROM::erase_chip() {
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_CE); // send Chip erase command
	digitalWrite(this->slave_select, HIGH);  
}

void protect() {
  // BP0 = 1 & BP1 = 1: All sectors (0 to 3) are write-protected 
  // All other bits default
  // 0000 1100
  write_status(0x8C);
}

void unprotect() {
  // BP0 = 0 & BP1 = 0: None protected
  write_status(0x00);
}


bool SPIEEPROM::write_status(byte status) {
  // Status register bits:
  // 0: Write-in-process (read-only)
  // 1: Write enable latch (read-only)
  // 2: Block protection 0
  // 3: Block protection 1 
  // 4-6: Unused
  // 7: WPEN 
	digitalWrite(this->slave_select, LOW);
	SPI.transfer(SPIEEPROM_RDSR); // send RDSR command
	
	SPI.transfer(status); // set status
	
	digitalWrite(this->slave_select, HIGH);
 
}

#endif  // EEPROM_SPI
