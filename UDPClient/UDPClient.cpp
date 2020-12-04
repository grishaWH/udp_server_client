#include "UDPClient.h"

UDPClient::UDPClient(boost::asio::ip::udp::endpoint server_ep, unsigned short port_client)
:socket_(service_,boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v4(),
		 													 port_client
															}),
server_ep_(server_ep)
{ }

void UDPClient::send_file_to_server(unsigned char *file, size_t size, unsigned char *file_id)
{
	check_sum_file_ = crc32c(0, file, size);
//	std::cout << check_sum_file_ << std::endl;
	formation_packages(file, size, file_id);
	send_packages();
	set_answer_from_server();
}

void UDPClient::result_from_server() const
{
	std::cout << "Size file: [" << LEN_TEST_FILE << "]" << std::endl;
	std::cout << "Number of packages [" << recv_packages_.back().seq_total_ << "] was sended." << std::endl;
	std::cout << "Number of packages [" << recv_packages_.back().seq_total_ << "] was received." << std::endl;
	std::cout << "Check sum from server ["
			  << check_sum_file_from_server_
			  << "] and check sum from client ["
			  << check_sum_file_
			  << "]";
	if (check_sum_file_from_server_ == check_sum_file_)
		std::cout << " are equal" << std::endl;
	else
		std::cout << " not equal" << std::endl;
}

uint32_t UDPClient::crc32c(uint32_t crc, const unsigned char *buf, size_t len)
{
	int k;
	crc = ~crc;
	while (len--) {
		crc ^= *buf++;
		for (k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ 0x82f63b78 : crc >> 1;
	}
	return ~crc;
}

void UDPClient::formation_packages(unsigned char *file, size_t size, unsigned char *file_id)
{
	uint32_t seq_total = size / size_data_package_;
	seq_total = (size % size_data_package_ == 0) ? seq_total : seq_total + 1; //num of packages
	packages_.reserve(seq_total); //reserve heap memory for data
	unsigned char *ptr = file;
	uint32_t offset = 0;
	for (int i = 0; i < seq_total; i++)
	{
		offset = (ptr - file + size_data_package_ <= size) ? size_data_package_ : size - (ptr - file);
		packages_.push_back(Package(i, seq_total, uint8_t(TypePackage::PUT), file_id, ptr, offset));
		ptr = ptr + offset;
//		std::cout << offset << std::endl;
	}
}

void UDPClient::send_packages()
{
	socket_.send_to(boost::asio::buffer("local address: "
										+ socket_.local_endpoint().address().to_string()
										+ ":"
										+ std::to_string(socket_.local_endpoint().port())
					),
					server_ep_);
	int size_recv = 0;
	Package recv_pack;
	recv_packages_.reserve(packages_.back().seq_total_);
	for (int i = 0; i < packages_.back().seq_total_; i++)
	{
		socket_.send_to(boost::asio::buffer(&packages_[i], sizeof(packages_[i])), server_ep_);
//		std::cout << strlen((const char*)packages_[i].data_) << std::endl;
//		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		size_recv = socket_.receive_from(boost::asio::buffer(&recv_pack, sizeof(recv_pack)), server_ep_);
		if (size_recv != sizeof(recv_pack))
		{
			std::cout << "Package crashed. Retrying send package..."
					  << packages_[i].seq_number_
					  << " id"
					  << std::endl;
			i--;
			continue ;
		}
		recv_packages_.push_back(recv_pack);
	}
}

void UDPClient::set_answer_from_server()
{
	check_sum_file_from_server_ = atoi((const char *)recv_packages_.back().data_);
}

UDPClient::~UDPClient() {
	socket_.close();
}