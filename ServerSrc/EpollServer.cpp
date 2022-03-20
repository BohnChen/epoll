/**
 * EpollServer.cpp : main
 * ChenBohn 2022-03-19
 */
#include "../Header/ServerH/serverheader.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
int main(int argc, char* argv[])
{
	// create a socket
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	ERRORCHECK(listenfd, -1, "create listen socket error.", 0, 0);

	// Set ip and port reuseful
	int on = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));

	// Set socket none-block
	int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
	int newSocketFlag = oldSocketFlag | O_NONBLOCK;
	int ret = fcntl(listenfd, F_SETFL, newSocketFlag);
	ERRORCHECK(ret, -1, "Set listenfd to nonblock", 1, listenfd);

	// Init server address
	struct sockaddr_in bindaddr;
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bindaddr.sin_port = htons(3000);

	ret = bind(listenfd, (struct sockaddr*)&bindaddr, sizeof(bindaddr));
	ERRORCHECK(ret, -1, "Bind listen socket error.", 1, listenfd);

	// Start listening
	ret = listen(listenfd, SOMAXCONN);
	ERRORCHECK(ret, -1, "Listen error.", 1, listenfd);

	// Create epollfd
	int epollfd = epoll_create(1);
	ERRORCHECK(epollfd, -1, "create epollfd error. ", 1, listenfd);

	epoll_event listen_fd_event;
	listen_fd_event.data.fd = listenfd;
	listen_fd_event.events = EPOLLIN;

	// Comment the line to  use Level trigger.
	// listen_fd_event.events |= EPOLLIN;

	// Bind listening socket to epollfd.
	ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &listen_fd_event);
	ERRORCHECK(ret, -1, "epoll_ctl error.", 1, listenfd);

	int n;
	while (true) {
		epoll_event epoll_events[1024];
		n = epoll_wait(epollfd, epoll_events, 1024, 1000);
		if (n < 0) {
			// Interrupted by a singnal.
			if (errno == EINTR)
				continue;

			// error
			break;
		} else if (n == 0) {
			// timeout
			continue;
		}

		for (int i = 0; i < n; ++i) {
			// Events readable
			if (epoll_events[i].events & EPOLLIN) {
				if (epoll_events[i].data.fd == listenfd) {

					// Listening socket, Accept new connection
					struct sockaddr_in clientaddr;
					socklen_t clientaddrlen = sizeof(clientaddr);
					int clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientaddrlen);
					if (clientfd != -1) {
						int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
						int newSocketFlag = oldSocketFlag | O_NONBLOCK;
						if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1) {
							close(clientfd);
							std::cout << "Set clientfd to nonblocking error." << std::endl;
						} else {
							epoll_event client_fd_event;
							client_fd_event.data.fd = clientfd;
							client_fd_event.events = EPOLLIN;
							// Comment the line to  use Level trigger.
							// client_fd_event.events |= EPOLLIN;

							if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &client_fd_event) != -1) {
								std::cout << "New client accepted, clientfd: " << clientfd << std::endl;
								std::cout << "client fd: " << clientfd << std::endl;
								transFiles(clientfd);
								// transFiles(epoll_events[i].data.fd);
							} else {
								std::cout << "add client fd to epollfd error." << std::endl;
								close(clientfd);
							}
						}
					}
				} else {
					std::cout << "client fd: " << epoll_events[i].data.fd << std::endl;
					// download
					// std::cout << "epoll_events[i].data.fd: " << epoll_events[i].data.fd << std::endl;
					// transFiles(epoll_events[i].data.fd);

					if (ret == 0) {
						// Client quit connection, so delete clientfd from epollfd.
						if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1) {
							std::cout << "Client disconnected, clientfd : " << epoll_events[i].data.fd << std::endl;
						}
						close(epoll_events[i].data.fd);
					} else if (ret < 0) {
						// error
						if (errno != EWOULDBLOCK && errno != EINTR) {
							if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1) {
								std::cout << "client disconnected, clientfd : " << epoll_events[i].data.fd << std::endl;
							}
							close(epoll_events[i].data.fd);
						}
					} else {
						// Receive data normally
						// std::cout << "recv from client:" << epoll_events[i].data.fd << ", " << ch << std::endl;
						std::cout << "Send successfully." << std::endl;
					}
				}
			} else if (epoll_events[i].events & EPOLLERR) {
				// TODO:
			}
		}
	}

	close(listenfd);
	return 0;
}
