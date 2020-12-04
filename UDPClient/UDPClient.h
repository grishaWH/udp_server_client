#ifndef UDPCLIENT_UDPCLIENT_H
#define UDPCLIENT_UDPCLIENT_H
#define LEN_TEST_FILE 1'000'000

#include <array>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <thread>
#include <list>
#include "package.h"

class UDPClient
{
public:
	UDPClient(boost::asio::ip::udp::endpoint server_ep, unsigned short port_client);

	void send_file_to_server(unsigned char *file, size_t size, unsigned char *file_id);

	void result_from_server() const;

	~UDPClient();

private:
	boost::asio::io_service service_;
	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint server_ep_;
	std::vector<Package> packages_;
	std::vector<Package> recv_packages_;

	uint32_t check_sum_file_ = 0;
	const uint32_t size_data_package_ = 1454;
	uint32_t check_sum_file_from_server_ = 0;

	uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len);

	void formation_packages(unsigned char *file, size_t size, unsigned char *file_id);

	void send_packages();

	void set_answer_from_server();

};
#endif
