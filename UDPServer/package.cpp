#include "package.h"

Package::Package() { }

Package::Package(uint32_t seq_num,
				 uint32_t seq_tot,
				 uint8_t type,
				 unsigned char *id,
				 unsigned char *data,
				 size_t size_data
				 )
				:seq_number_(seq_num),
				seq_total_(seq_tot),
				type_(type)
{
	std::memcpy(id_, id, sizeof(id_));
	std::memcpy(data_, data, size_data);
}