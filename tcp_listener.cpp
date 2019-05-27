#include "tcp_listener.h"

int tcp_listener::init()
{
	WSADATA ws_data;
	WORD ver = MAKEWORD(2, 2);

	int ws_ok = WSAStartup(ver, &ws_data);
	if (ws_ok != 0)
	{
		return ws_ok;
	}

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_port);
	inet_pton(AF_INET, m_ip_adress, &hint.sin_addr);

	if(bind(m_socket, (sockaddr*)&hint, sizeof(hint)))
	{
		return WSAGetLastError();
	}

	if(listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	FD_ZERO(&m_master);

	FD_SET(m_socket, &m_master);
	return 0;
}

int tcp_listener::run()
{
	bool running = true;

	while (running)
	{
		fd_set copy = m_master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == m_socket)
			{
				SOCKET client = accept(m_socket, nullptr, nullptr);

				FD_SET(client, &m_master);
				on_client_connected(client);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					on_client_disconnected(sock);
				}
				else
				{
					on_message_received(sock, buf, bytesIn);
				}
			}
		}
	}

	FD_CLR(m_socket, &m_master);
	closesocket(m_socket);

	while (m_master.fd_count > 0)
	{
		SOCKET sock = m_master.fd_array[0];
		FD_CLR(sock, &m_master);
		closesocket(sock);
	}

	WSACleanup();
	return 0;
}

void tcp_listener::send_to_client(int client_socket, const char* msg, int length) { send(client_socket, msg, length, 0); }

void tcp_listener::broadcast_to_clients(int sending_client, const char* msg, int length)
{
	for (int i = 0; i < m_master.fd_count; i++)
	{
		SOCKET outSock = m_master.fd_array[i];
		if (outSock != m_socket && outSock != sending_client) { send_to_client(outSock, msg, length); }
	}
}

void tcp_listener::on_client_connected(int client_socket) { }
void tcp_listener::on_client_disconnected(int client_socket) { }
void tcp_listener::on_message_received(int client_socket, const char* msg, int length) { }