#include "UDPServer.h"

UDPServer::UDPServer(unsigned short num_port)
		:socket_(service_, {boost::asio::ip::udp::v4(), num_port})
{
	std::cout << "Start receive packages ..." << std::endl;
}

void UDPServer::start()
{
	recv_put_and_send_ack_packages();
	calculate_check_sum();
	send_answer_to_client();
}

void UDPServer::calculate_check_sum()
{
	check_sum_ = 0;
	unsigned char file[LEN_TEST_FILE];
	unsigned char *ptr = &file[0];
	uint32_t len;
	for (int i = 0; i < recv_packages_.size(); i++)
	{
		len = strlen((char *)recv_packages_[i].data_);
		memcpy(ptr, recv_packages_[i].data_, len);
		ptr += len;
	}
	check_sum_ = crc32c(0, file, sizeof(file));
}

uint32_t UDPServer::crc32c(uint32_t crc, const unsigned char *buf, size_t len)
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

void UDPServer::recv_put_and_send_ack_packages() {
	len_buf_ = socket_.receive_from(boost::asio::buffer(recv_buf_), client_ep);
	std::cout << "Recieved message from "
			  << client_ep.address().to_string()
				 + ":" + std::to_string(client_ep.port())
			  << std::endl;
	std::cout.write(recv_buf_.data(), len_buf_);
	std::cout << std::endl;
	size_t seq_total_ = 0;
	Package package;
	for( ; ; )
	{
		len_buf_ = socket_.receive_from(boost::asio::buffer(&package, sizeof(package)), client_ep);
		if (len_buf_ != sizeof(package))
		{
			socket_.send_to(boost::asio::buffer("package crashed"), client_ep);
			continue ;
		}
		recv_packages_.push_back(package);
		++seq_total_;
		if (seq_total_ == package.seq_total_)
			break ;
		send_ack_package(TypePackage::ACK, seq_total_ - 1);
	}
}

void UDPServer::send_ack_package(TypePackage type, int seq_total) {
	Package end_recv_package = recv_packages_.back();
	Package ack_package(end_recv_package.seq_number_,
						seq_total,
						uint8_t(type),
						end_recv_package.id_,
						end_recv_package.data_,
						sizeof(end_recv_package.data_));
	socket_.send_to(boost::asio::buffer(&ack_package, sizeof(ack_package)), client_ep);
}

void UDPServer::send_answer_to_client()
{
	auto& recv_package = recv_packages_.back();
	unsigned char recv_check_sum[sizeof(recv_package)];
	memcpy(recv_check_sum, (unsigned char *)(&std::to_string(check_sum_)[0]), sizeof(recv_check_sum));
	memcpy(&recv_packages_.back().data_, recv_check_sum, sizeof(recv_check_sum));
	send_ack_package(TypePackage::ACK, recv_packages_.size());
	std::cout << "Answer "
			  << "[file check sum = "
			  << check_sum_
			  << "] "
			  << "was sended on client ["
			  << client_ep.address().to_string()
			  << ":"
			  << std::to_string(client_ep.port())
			  << "]\n";
}


UDPServer::~UDPServer() {
	socket_.close();
}