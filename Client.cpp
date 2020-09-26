#include <bits/stdc++.h>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <pthread.h>
using namespace std;

void *sender(void *arg){
	int clientSd = *((int *)arg);
    char msg[1500];
	while(1){
        string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);
            close(clientSd);
    		cout << "Connection closed" << endl;
            return NULL;
        }
        send(clientSd, (char*)&msg, strlen(msg), 0);
    }
    return NULL;
}

void *receiver(void *arg){
	int clientSd = *((int *)arg);
    char msg[1500];
	while(1){
        memset(&msg, 0, sizeof(msg));
        recv(clientSd, (char*)&msg, sizeof(msg), 0);
        if(!strcmp(msg, "exit"))
        {
            cout << "Server has quit the session" << endl;
            close(clientSd);
    		cout << "Connection closed" << endl;
            exit(0);
        }
        cout << "Server: " << msg << endl;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cerr << "Usage: ip_address port" << endl; exit(0); 
    }
    char *serverIp = argv[1]; int port = atoi(argv[2]); 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    int status = connect(clientSd,(sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;
		return 0;
    }
    cout << "Connected to the server!" << endl;
    pthread_t tsend, treceive;
    pthread_create(&tsend, NULL, sender, (void *)&clientSd);
    pthread_create(&treceive, NULL, receiver, (void *)&clientSd);
    pthread_join(tsend, NULL);
    pthread_cancel(treceive);
    return 0;    
}
