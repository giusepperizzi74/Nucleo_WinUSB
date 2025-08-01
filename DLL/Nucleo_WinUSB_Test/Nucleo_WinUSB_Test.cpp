
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <iomanip>
#include "..\Nucleo_WinUSB.h"

//#define MAX_DATA_LENGTH 2048
//uint8_t data[MAX_DATA_LENGTH];

void print_help()
{
	std::cout << "help                                                 -- Print this menu.\n";
	std::cout << "list                                                 -- List the Nucleo_WinUSB boards connected to this PC.\n";
	std::cout << "open                                                 -- Acquire the USB handle to the first available Nucleo_WinUSB.\n";
	std::cout << "close                                                -- Release the USB handle.\n";
	std::cout << "serial                                               -- Get the USB device serial number.\n";
	std::cout << "***** GPIO *****\n";
	std::cout << "gpio [-d 0|1] [-p 0|1|2] [-t 0|1] set|clear|get e?   -- Get/Set/Clear a gpio, e.g., g4.\n";
	std::cout << "                                                     Options:\n";
	std::cout << "                                                     -d 0|1   -> gpio direction: input(0), output(1)\n";
	std::cout << "                                                     -p 0|1|2 -> pull-ups: none(0), up(1), down(2)\n";
	std::cout << "                                                     -t 0|1   -> output type: pushpull(0), opendrain(1)\n";
}

int m_list()
{
	// List all available adapters
	char* device = get_next(NULL);
	while (device != NULL) {
		std::cout << device << std::endl;
		device = get_next(device);
	}
	return 0;
}

void* m_open()
{
	// Open the first one that is not already opened
	void* handle = NULL;
	char* device = get_next(NULL);
	while (device != NULL) {
		handle = open(device);
		if (handle != NULL) {
			std::cout << "\nSuccessfully opened device: " << device << std::endl;
			return handle;
		}
		device = get_next(device);
	}
	if (handle == NULL) {
		std::cout << "Unable to open Winusb device.\n";
	}
	return handle;
}

void m_close(void* handle)
{
	close(handle);
}

int str_to_int(const std::string& str)
{
	size_t pos;
	int n = std::stoi(str, &pos, 0);
	if (pos != str.length())
		throw std::invalid_argument("Invalid characters in input string.");
	return n;
}

int m_gpio(std::vector<std::string>& tokens, void* handle)
{
	char port = 0;
	uint8_t pin = 0xFF;
	uint8_t type = 0xFF;
	uint8_t pull = 0xFF;
	uint8_t direction = 0xFF;
	int op_type = 0;
	int res=-1;

	int i;
	for (i = 1; i < tokens.size(); i++) {

		if (tokens[i] == "-t") {
			i++;
			try {
				type = str_to_int(tokens[i]);
			}
			catch (...) { break; }
		}
		else if (tokens[i] == "-p") {
			i++;
			try {
				pull = str_to_int(tokens[i]);
			}
			catch (...) { break; }
		}
		else if (tokens[i] == "-d") {
			i++;
			try {
				direction = str_to_int(tokens[i]);
			}
			catch (...) { break; }
		}
		else if (tokens[i] == "set")
			op_type = 1;
		else if (tokens[i] == "clear")
			op_type = 2;
		else if (tokens[i] == "get")
			op_type = 3;
		else {
			try {
				port = tokens[i][0];
				pin = str_to_int(tokens[i].substr(1));
			}
			catch (...) {
				break;
			}
		}			
	}
	if (i < tokens.size()) {
		std::cout << "The command is ill-formatted.\n";
		return -1;
	}

	if (type != 0xFF || pull != 0xFF || direction != 0xFF) {
		res = gpio_config(handle, port, pin, direction, type, pull);
		if(res < 0)
			return res;
	}
 
	if (op_type==1)
		return gpio_set(handle, port, pin);
	else if (op_type==2)
		return gpio_clear(handle, port, pin);
	else if (op_type==3) {
		uint8_t value;
		res = gpio_get(handle, port, pin,&value);
		if (res >= 0)
			std::cout << (int)value << std::endl;
		return res;
	}
	else
		std::cout << "The command is ill-formatted.\n";
	return res;
}

int main(int argc, char argv[])
{
	std::string cmd_line;
	std::string token;
	std::vector<std::string> tokens;
	void* handle = NULL;
	int res=0;
	
	print_help();

	while (1) {
		std::cout << ">";
		std::getline(std::cin, cmd_line);
		std::istringstream iss{cmd_line};
		while (iss >> token) {
			tokens.push_back(token);
		}
		if (tokens.empty())
			continue;
		/* CLI commands */
		if (tokens[0] == "help")
			print_help();

		/* USB device commands */
		else if (tokens[0] == "list")
			m_list();
		else if (tokens[0] == "open")
			handle = m_open();
		else if (tokens[0] == "close") {
			m_close(handle);
			handle = NULL;
		}
		else if (tokens[0] == "serial") {
			char serial[256];
			if (get_serial_number(handle, serial) >= 0)
				std::cout << serial << std::endl;
		}

		/* GPIO */
		else if (tokens[0] == "gpio") {
			res = m_gpio(tokens, handle);
			if (res < 0)
				std::cerr << "Error\n";
		}
		else
			std::cout << "The command is ill-formatted\n";
		tokens.clear();
	}
}

