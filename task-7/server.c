#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() 
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};
    char *hello = "Hello from server";

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
    {
        printf("WSAStartup failed\n");
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
    {
        printf("socket creation failed\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) 
    {
        printf("bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 3) == SOCKET_ERROR) 
    {
        printf("listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == INVALID_SOCKET) 
    {
        printf("accept failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) 
    {
        printf("Received message from client: %s\n", buffer);
    }

    if (send(client_socket, hello, strlen(hello), 0) == SOCKET_ERROR) 
    {
        printf("send failed\n");
        closesocket(server_socket);
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    printf("Hello message sent to client\n");

    closesocket(server_socket);
    closesocket(client_socket);
    WSACleanup();
    return 0;
}
