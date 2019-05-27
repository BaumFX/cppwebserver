#include "web_server.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <fstream>

/**
	sends html response to client

	@param response body
	@return void
*/
void web_server::send_html_to_client(std::string content, int client_socket = 0)
{
	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n" << "Cache-Control: no-cache, private\r\n" << "Content-Type: text/html\r\n" << "Content-Length: " << content.length() << "\r\n" << "\r\n" << content;
	send_to_client(client_socket, oss.str().c_str(), oss.str().size());
}

/**
	checks whether a file exists
	@param string of the path
	@return boolean (existence)
*/
inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

/**
	gets and returns contents of a file on disk

	@param string of the path
	@return file contents or empty string if file doesnt exist
*/
std::string get_file_contents(std::string path)
{
	std::string hahayes;
	std::cout << path << std::endl;
	std::ifstream file("wwwroot/" + path);
	std::string yes;
	if (!file.good()) { return ""; }

	while (!file.eof()) { std::getline(file, yes); hahayes += yes; }
	file.close();
	return hahayes;
}

/**
	removes last slash in string

	@param string to remove slash from
	@return url_stuff object (has status integer and url string)
*/
url_stuff remove_slash_from_path(url_stuff path)
{
	if (path.status != 0) { return path; }

	std::cout << std::endl << path.path << std::endl;
	if (path.path.back() != '/') { return path; }
	path.path.pop_back();
	return remove_slash_from_path(path);
}

/**
	cleans up path to work

	@param string to remove slashes from
	@return cleaned string
*/
url_stuff clean_up_path(std::string path)
{
	url_stuff urlstuff;
	urlstuff.status = 0;
	std::string http_string = " HTTP/1.1";
	size_t pos = path.find(http_string);
	//if the path contains the http part, erase everything to the end (including the " HTTP/1.1")
	//after that remove the first 4 characters which is "GET " (path left)
	if (pos != std::string::npos) { path.erase(pos, path.length() - pos); path.erase(0, 4); }
	else 
	{ 
		std::cout << path;
		//TODO: handle POST requests etc.
		urlstuff.status = 1;
		urlstuff.path = "";
		return urlstuff;
	}

	//path now holds the directory/file requested.
	//path traversal etc. is prevented by removing slashes + defaulting the folder to "wwwroot/"
	//TODO: add more security

	std::transform(path.begin(), path.end(), path.begin(), ::tolower);

	if (path == "/") {
		urlstuff.path = "index";
		return urlstuff;
	}

	urlstuff.path = path;

	return remove_slash_from_path(urlstuff);
}

/**
	does the actual handling of the request, called when client sends a message/request

	@param int for clientsocket, char* for the request message, int for length
	@return void
*/
void web_server::on_message_received(int client_socket, const char* msg, int length)
{
	std::string output = "<h1>internal server error</h1>";
	url_stuff url = clean_up_path(msg);

	if (url.status != 0) { send_html_to_client(output, client_socket); return; }
		
	output = get_file_contents(url.path + ".html");
	
	if (output == "") { send_html_to_client("<h1>internal server error</h1>", client_socket); return; }

	send_html_to_client(output, client_socket);
}

/**
	functions get called on client connected (not really necessary for webserver)
	@param int with socket id for the client
	@return void
*/
void web_server::on_client_connected(int client_socket) { }
/**
	functions get called on client disconnected (not really necessary for webserver)
	@param int with socket id for the client
	@return void
*/
void web_server::on_client_disconnected(int client_socket) { }