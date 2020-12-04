#include <algorithm>
#include "UDPClient.h"

int main()
{
	unsigned char file[LEN_TEST_FILE];
	std::fill(&file[0], &file[LEN_TEST_FILE], 49);
	unsigned char file_id[8] = {'0', '0', '0', '0', '0', '0', '0', '1'};
	boost::asio::ip::udp::endpoint server_ep{boost::asio::ip::address::from_string("127.0.0.1"), 3397};
	UDPClient Client(server_ep, 9997);
	Client.send_file_to_server(file, sizeof(file), file_id);
	Client.result_from_server();
}