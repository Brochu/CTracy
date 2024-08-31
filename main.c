#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winerror.h>

#define DEFAULT_PORT 8086
#define DEFAULT_BACKLOG 4

// PLAN
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
//Seems like we need to create a separate UDP socket to send initial broadcast message
//Then accept connections on the listen socket created beforehand
// **************************

int makeAddrInfo(const char *ip, int port, int stype, struct addrinfo **out) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = stype;

    char portbuf[32];
    sprintf(portbuf, "%hu", port);

    int ret = getaddrinfo(ip, portbuf, &hints, out);
    if (ret != 0) {
        printf("[PADDRINFO] Could not call getaddrinfo (err = %i)\n", ret);
        return -1;
    }

    return 0;
}

SOCKET makeListenSock(struct addrinfo **outinfo) {
    struct addrinfo *addr = NULL;
    if (makeAddrInfo(NULL, DEFAULT_PORT, SOCK_STREAM, &addr) == -1) {
        printf("[LSOCK] Invalid addrinfo for port %i\n", DEFAULT_PORT);
        *outinfo = NULL;
        return -1;
    }

    SOCKET s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (s == -1) {
        printf("[LSOCK] Could not create socket\n");
        freeaddrinfo(addr);
        *outinfo = NULL;
        return -1;
    }

    *outinfo = addr;
    return s;
}

SOCKET makeBroadcastSock(struct addrinfo **outinfo) {
    struct addrinfo *addr = NULL;
    if (makeAddrInfo("255.255.255.255", DEFAULT_PORT, SOCK_DGRAM, &addr)) {
        printf("[BSOCK] Invalid addrinfo for port %i", DEFAULT_PORT);
        *outinfo = NULL;
        return -1;
    }

    SOCKET s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (s == -1) {
        printf("[BSOCK] Could not create socket\n");
        freeaddrinfo(addr);
        *outinfo = NULL;
        return -1;
    }

    //TODO: need to call setsockopt here to setup SO_BROADCAST

    *outinfo = addr;
    return s;
}

int main(int argc, char **argv) {
    int outcode = 0;
    printf("[CTracy] starting program\n");

    WSADATA wsaData;
    if(WSAStartup(MAKEWORD( 2, 2 ), &wsaData) != 0) {
        printf("Cannot init winsock.\n");
        return -1;
    }

    struct addrinfo *addr = NULL;
    SOCKET lsocket = makeListenSock(&addr);
    if (lsocket == -1) {
        printf("Cannot create listen socket.\n");
        return -1;
    }

    int ret = bind(lsocket, addr->ai_addr, addr->ai_addrlen);
    if (ret == -1) {
        printf("[MAIN] Could not bind socket\n");
        freeaddrinfo(addr);
        outcode = -1;
        goto cleanlisten;
    }
    ret = listen(lsocket, DEFAULT_BACKLOG);
    if (ret == -1) {
        printf("[MAIN] Could not listen on the socket\n");
        freeaddrinfo(addr);
        outcode = -1;
        goto cleanlisten;
    }
    freeaddrinfo(addr);
    addr = NULL;

    SOCKET bsocket = makeBroadcastSock(&addr);
    if (bsocket == -1) {
        printf("[MAIN] Could not broadcast on the socket\n");
        freeaddrinfo(addr);
        outcode = -1;
        goto cleanlisten;
    }

    const char *data = "Hello, World!";
    size_t len = strlen(data);
    if (send(bsocket, data, len, 0) == SOCKET_ERROR) {
        //TODO: Check if error persists after SO_BROADCAST is properly set
        int errcode = WSAGetLastError();
        printf("[MAIN] Could not send data to broadcast (err = %i)\n", errcode);
        freeaddrinfo(addr);
        outcode = -1;
        goto cleanbroadcast;
    }

    //TODO: Operations on connection
    printf("[MAIN] Operations on socket here ...\n");

cleanbroadcast:
    closesocket(bsocket);
cleanlisten:
    closesocket(lsocket);

    printf("[CTracy] Program end (outcode = %i)\n", outcode);
    return outcode;
}
