#include "../Header/ServerH/serverheader.h"
#include <cstring>

int transFiles(int fd)
{
	// Noraml clientfd
	FileData fileData = { 0 };
	strcpy(fileData.buf, "file1");
	fileData.dataLength = strlen(fileData.buf);
	// Send a file name.
	send(fd, &fileData, 4 + fileData.dataLength, 0);
	int newFileFd = open(fileData.buf, O_RDONLY);
	ERRORCHECK(newFileFd, -1, "Read File Error.", 0, 0);
	while ((fileData.dataLength = read(newFileFd, fileData.buf, sizeof(fileData.buf)))) {
		send(fd, &fileData, 4 + fileData.dataLength, 0);
	}
	send(fd, &fileData, 4, 0);
	std::cout << "send successfully. " << std::endl;

	return 0;
}
