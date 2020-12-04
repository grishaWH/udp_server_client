#include "package.h"

Package::Package() { }

Package::Package(uint32_t seq_num,
				 uint32_t seq_tot,
				 uint8_t type,
				 unsigned char *id,
				 unsigned char *data,
				 size_t size_data)
				 :seq_number_(seq_num),
				 seq_total_(seq_tot),
				 type_(type)
{
	for  (int i = 0; i < sizeof(id); i++)
		id_[i] = id[i];
	int i = 0;
	for  ( ; i < size_data; i++)
		data_[i] = data[i];
	data_[i] = '\0';
}