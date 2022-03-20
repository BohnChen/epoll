#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_ADDRESS 192.168.17.130
#define SERVER_PORT 3000

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

struct FileData {
	int dataLength;
	char buf[1000];
};

int main(int argc, char* argv[])
{
	int clientfd = socket(AF_INET, SOCK_STREAM, 0);
	ERRORCHECK(clientfd, -1, "Create client Socket Error. ", 0, 0);

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);

	int ret = connect(clientfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	ERRORCHECK(ret, -1, "Connect Error. ", 0, 0);
	// FileData fileData;
	int dataLength = 0;
	char buf[1000] { 0 };
	// recv dataLength
	recv(clientfd, &dataLength, 4, 0);
	// recieve file name.
	recv(clientfd, buf, dataLength, 0);
	// std::cout << "buf : " << buf << std::endl;
	int fd = open(buf, O_WRONLY | O_CREAT, 0666);
	ERRORCHECK(fd, -1, "open Error.", 0, 0);
	// std::cout << "Begin Download." << std::endl;
	while (1) {
		recv(clientfd, &dataLength, 4, 0);
		if (dataLength > 0){
			recv(clientfd, buf, dataLength, 0);
			write(fd, buf, dataLength);
		}
		else {
			break;
		}
	}

	close(fd);
	close(clientfd);

	return 0;
}
