#ifndef UDPSERVER_PACKAGE_H
#define UDPSERVER_PACKAGE_H

#include <iostream>

enum TypePackage {
	ACK = 0,
	PUT = 1,
};

struct Package
{
	Package();
	Package(uint32_t seq_num, uint32_t seq_tot, uint8_t type, unsigned char *id, unsigned char *data, size_t size_data);

	uint32_t seq_number_; // номер пакета
	uint32_t seq_total_; // количество пакетов с данными
	uint8_t type_; // тип пакета: 0 == ACK, 1 == PUT
	unsigned char id_[8]; // 8 байт - идентификатор, отличающий один файл от другого
	unsigned char data_[1'455]; // после заголовка и до конца UDP пакета идут данные
};
#endif
