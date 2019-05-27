#pragma once
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

class tcp_listener
{
public:

	tcp_listener(const char* ip_adress, int port) :
	m_ip_adress(ip_adress), m_port(port) { }

	int init();

	int run();

protected:

	virtual void on_client_connected(int client_socket);

	virtual void on_client_disconnected(int client_socket);

	virtual void on_message_received(int client_socket, const char* msg, int length);

	void send_to_client(int client_socket, const char* msg, int length);

	void broadcast_to_clients(int sending_client, const char* msg, int length);

private:

	const char* m_ip_adress;
	int m_port;
	int m_socket;
	fd_set m_master;
};