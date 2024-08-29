#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winerror.h>

int main(int argc, char **argv) {
    int outcode = 0;
    printf("[CTracy] starting program\n");

    WSADATA wsaData;
    if(WSAStartup(MAKEWORD( 2, 2 ), &wsaData) != 0)
    {
        printf("Cannot init winsock.\n");
        return -1;
    }

    // **************************
    //TODO: First thing is to setup UDP broadcast socket
    //Look into WorkerThread to see how they do it
    //
    //First Goal, Find out if we can connect to Tracy server socket
    //-> Do we receieve any data?
    //Second Goal, Broadcast new app + Go through handshake process
    //Third Goal, Figure out how to handle keep-alive requests?
    //Fourth Goal, organize data to send a message
    //TODO: Do we keep the app running with Raylib?
    // **************************

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int port = 8086;
    char portbuf[32];
    sprintf(portbuf, "%hu", port);

    struct addrinfo *addr = NULL;
    int ret = getaddrinfo(NULL, portbuf, &hints, &addr);
    if (ret != 0) {
        printf("[MAIN] Could not call getaddrinfo (err = %i)\n", ret);
        return -1;
    }

    SOCKET s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (s == -1) {
        printf("[MAIN] Could not create socket\n");
        outcode = -1;
        goto cleanaddr;
    }

    ret = bind(s, addr->ai_addr, addr->ai_addrlen);
    if (ret == -1) {
        printf("[MAIN] Could not bind socket\n");
        outcode = -1;
        goto cleansock;
    }

    //TODO: Operations on connection
    printf("[MAIN] Operations on socket here ...\n");

cleansock:
    closesocket(s);
cleanaddr:
    freeaddrinfo(addr);

    printf("[CTracy] Program end\n");
    return outcode;
}
