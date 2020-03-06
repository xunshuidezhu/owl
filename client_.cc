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
    memset(&remoteAddr, 0, sizeof(remoteAddr));
    inet_aton("127.0.0.1", &remoteAddr.sin_addr);
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(8080);
    char buf[100] = "asdasd";

    connect(sock, (sockaddr*)&remoteAddr, sizeof(remoteAddr));
    int len = send(sock, buf, strlen(buf), 0);
    printf("%d", len);
    return 0;
}