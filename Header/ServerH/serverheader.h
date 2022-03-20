/**
 * The header of server
 * BohnChen 2022-03-19
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define ERRORCHECK(ret, errorno, errorInfo, closeistrue, closefd) \
	{                                                               \
		if (ret == errorno) {                                         \
			std::cout << errorInfo << std::endl;                        \
			if (closeistrue == 1) {                                     \
				close(closefd);                                           \
			}                                                           \
			return errorno;                                             \
		}                                                             \
	}


// Define a struct to send a file.
struct FileData {
	int dataLength;
	char buf[1000];
};


// Client Using it to download file.
int transFiles(int fd);

