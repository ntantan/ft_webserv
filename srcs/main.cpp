/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ntan <ntan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/03 14:57:05 by ntan              #+#    #+#             */
/*   Updated: 2022/12/12 15:35:58 by ntan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "config/Config.hpp"
# include "client/Request.hpp"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include "netinet/in.h"
#include <poll.h>
#include <sys/epoll.h>

#define PORT 8000

// int main()
// {
// 	char	path[] = "./config_files/default.conf";
// 	try
// 	{
// 		Config	conf(path);
// 	}
// 	catch (std::exception& e)
// 	{
// 		std::cerr << e.what() << std::endl;
// 		return (1);
// 	}
// 	return (0);
// }

// write an http server using poll
int main()
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		std::cerr << "Could not create socket" << std::endl;
		return (1);
	}

	struct sockaddr_in address;
	std::memset((char *) &address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
	{
		std::cerr << "Bind failed" << std::endl;
		return (1);
	}

	if (listen(server_fd, 3) < 0)
	{
		std::cerr << "In listen" << std::endl;
		return (1);
	}

	struct pollfd fds[1024];
	fds[0].fd = server_fd;
	fds[0].events = POLLIN;
	int nfds = 1;
	int addrlen = sizeof(address);
	while (1)
	{
		std::cout << "====== WAITING FOR NEW CONNECTION =====" << std::endl;
		int poll_ret = poll(fds, nfds, -1);
		if (poll_ret < 0)
		{
			std::cerr << "Poll error" << std::endl;
			return (1);
		}
		if (poll_ret == 0)
		{
			std::cerr << "Poll timeout" << std::endl;
			return (1);
		}
		for (int i = 0; i < nfds; i++)
		{
			if (fds[i].revents == 0)
				continue;
			if (fds[i].revents != POLLIN)
			{
				std::cerr << "Error in poll" << std::endl;
				return (1);
			}
			if (i == 0)
			{
				int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
				if (new_socket < 0)
				{
					std::cerr << "In accept" << std::endl;
					return (1);
				}
				fds[nfds].fd = new_socket;
				fds[nfds].events = POLLIN;
				nfds++;
			}
			else
			{
				char buffer[30000];
				int rd = read(fds[i].fd, buffer, 30000);
				if (rd < 0)
				{
					std::cerr << "Read error" << std::endl;
					return (1);
				}
				std::string request(buffer, rd);
				std::cout << request << std::endl;
				char response[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
				write(fds[i].fd, response, sizeof(response));
				std::cout << "===== RESPONSE SENT =====" << std::endl;
				close(fds[i].fd);
				fds[i].fd = -1;
			}
		}
	}
	close(server_fd);
	return (1);
}
