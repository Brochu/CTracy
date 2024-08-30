#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winerror.h>

#define DEFAULT_PORT 8086
#define DEFAULT_BACKLOG 4

int makePortAddrInfo(int port, int stype, struct addrinfo **out) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = stype;

    char portbuf[32];
    sprintf(portbuf, "%hu", port);

    int ret = getaddrinfo(NULL, portbuf, &hints, out);
    if (ret != 0) {
        printf("[PADDRINFO] Could not call getaddrinfo (err = %i)\n", ret);
        return -1;
    }

    return 0;
}

int makeIPAddrInfo(const char *ip, int stype, struct addrinfo **out) {
    //TODO: Implement this, needed to create broadcast socket
    return 0;
}

SOCKET makeListenSock(struct addrinfo **ainfo) {
    struct addrinfo *addr = NULL;
    if (makePortAddrInfo(DEFAULT_PORT, SOCK_STREAM, &addr) == -1) {
        printf("[LSOCK] Invalid addrinfo for port %i\n", DEFAULT_PORT);
        return -1;
    }

    SOCKET s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (s == -1) {
        printf("[LSOCK] Could not create socket\n");
        freeaddrinfo(addr);
        return -1;
    }

    *ainfo = addr;
    return s;
}

SOCKET handleBroadcastSetup() {
    return (SOCKET)0;
}

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
    //Seems like we need to create a separate UDP socket to send initial broadcast message
    //Then accept connections on the listen socket created beforehand
    // **************************

    struct addrinfo *addr = NULL;
    SOCKET lsocket = makeListenSock(&addr);

    int ret = bind(lsocket, addr->ai_addr, addr->ai_addrlen);
    if (ret == -1) {
        printf("[MAIN] Could not bind socket\n");
        freeaddrinfo(addr);
        outcode = -1;
        goto cleansock;
    }
    ret = listen(lsocket, DEFAULT_BACKLOG);
    if (ret == -1) {
        printf("[MAIN] Could not listen on the socket\n");
        freeaddrinfo(addr);
        outcode = -1;
        goto cleansock;
    }
    freeaddrinfo(addr);

    //TODO: Operations on connection
    printf("[MAIN] Operations on socket here ...\n");

cleansock:
    closesocket(lsocket);

    printf("[CTracy] Program end\n");
    return outcode;
}
