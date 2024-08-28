#include <stdio.h>

int main(int argc, char **argv) {
    printf("[CTracy] starting program\n");
    //TODO: First thing is to setup UDP broadcast socket
    //Look into WorkerThread to see how they do it
    //
    //First Goal, Find out if we can connect to Tracy server socket
    //-> Do we receieve any data?
    //Second Goal, Broadcast new app + Go through handshake process
    //Third Goal, Figure out how to handle keep-alive requests?
    //Fourth Goal, organize data to send a message
    //TODO: Do we keep the app running with Raylib?
    return 0;
}
