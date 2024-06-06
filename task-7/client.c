#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() 
{
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char *hello = "Hello from client";
    char buffer[BUFFER_SIZE] = {0};

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
    {
        printf("WSAStartup failed\n");
        return 1;
    }

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
    {
        printf("socket creation failed\n");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        printf("connection failed\n");
        WSACleanup();
        return 1;
    }

    if (send(client_socket, hello, strlen(hello), 0) == SOCKET_ERROR) 
    {
        printf("send failed\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    printf("Hello message sent\n");

    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) 
    {
        printf("Received message from server: %s\n", buffer);
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
