#pragma once
#include "tcp_listener.h"

class url_stuff
{
public:
	std::string path;
	int status;
};

class web_server : public tcp_listener
{
public:
	web_server(const char* ip_adress, int port) :
		tcp_listener(ip_adress, port) { }

protected:
	virtual void on_client_connected(int client_socket);

	virtual void on_client_disconnected(int client_socket);

	virtual void on_message_received(int client_socket, const char* msg, int length);

	virtual void send_html_to_client(std::string message, int client_socket);
};
