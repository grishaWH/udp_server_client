#ifndef UDPSERVER_UDPSERVER_H
#define UDPSERVER_UDPSERVER_H
#define LEN_TEST_FILE 1'000'000

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <boost/asio.hpp>
#include "package.h"

class UDPServer
{
public:
	UDPServer(unsigned short num_port);

	void start();

	~UDPServer();

private:
	boost::asio::io_service service_;
	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint client_ep;
	std::array<char, 100> recv_buf_;
	size_t len_buf_;
	uint32_t check_sum_ = 0;
	std::vector<Package> recv_packages_;

	void calculate_check_sum();

	uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len);

	void recv_put_and_send_ack_packages();

	void send_ack_package(TypePackage type, int seq_total);

	void send_answer_to_client();

};

#endif
