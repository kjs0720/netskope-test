#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int BUFFER_SIZE = 1024;

void error(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
    exit(1);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    char buffer[BUFFER_SIZE];

    // 서버 소켓 생성
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        error("Failed to create server socket");
    }

    // 서버 주소 설정
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);

    // 서버 소켓에 주소 바인딩
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        error("Failed to bind server socket");
    }

    // 클라이언트의 연결 요청 대기
    listen(serverSocket, 1);
    std::cout << "Server is listening on port 12345..." << std::endl;

    // 클라이언트 연결 수락
    socklen_t clientAddressLength = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket < 0) {
        error("Failed to accept client connection");
    }
    std::cout << "Client connected: " << inet_ntoa(clientAddress.sin_addr) << std::endl;

    // 클라이언트와 서버 간의 데이터 터널링
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
        if (bytesRead < 0) {
            error("Failed to read from client socket");
        }
        if (bytesRead == 0) {
            break;
        }
        std::cout << "Received from client: " << buffer << std::endl;

        int bytesWritten = write(clientSocket, buffer, strlen(buffer));
        if (bytesWritten < 0) {
            error("Failed to write to client socket");
        }
    }

    // 연결 종료
    close(clientSocket);
    close(serverSocket);

    return 0;
}