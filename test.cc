#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in remoteAddr;
    struct sockaddr_in localAddr;
    memset(&remoteAddr, 0, sizeof(remoteAddr));
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(8080);
    bind(sock, (sockaddr*)&localAddr, sizeof(localAddr));
    listen(sock, 10);
    char buf[100];

    socklen_t remoteAddrSize = sizeof(remoteAddr);
    int client = accept(sock, (sockaddr*)&remoteAddr, &remoteAddrSize);
    int len = recv(client, buf, 100, 0);
    printf("%s", buf);
    printf("%d", len);

    return 0;
}