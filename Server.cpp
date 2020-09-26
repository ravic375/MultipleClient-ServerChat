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

map<int, pthread_t*> ma;
queue<pthread_t*> delete_sequence;

void *cleaner(void *arg){
	while(1){
		if(delete_sequence.empty())
			sleep(5);
		else
			while(!delete_sequence.empty()){
				pthread_t *x = delete_sequence.front();
				delete_sequence.pop();
				delete x;
			}
    }
    return NULL;
}

void *sender(void *arg){
    char msg[1500];
	cout << "To Send a msg to a Client...\n<ClientID> message\n";
	while(1){
		string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, data.c_str());
        if(!strcmp(msg, "show"))
        {
        	map<int, pthread_t*>::iterator itr;
            cout << "ClientID\tThreadID\n"; 
    		for (itr = ma.begin(); itr != ma.end(); ++itr) { 
        		cout << itr->first << '\t' << '\t' << *(itr->second) << '\n'; 
    		} 
    		cout << endl;
            continue;
        }
        int newSd = stoi(data);
        if(ma.find(newSd) == ma.end())
        	continue;
        send(newSd, (char*)&msg, strlen(msg), 0);
    }
    return NULL;
}

void *receiver(void *arg){
	int newSd = *((int *)arg);
    char msg[1500];
	while(1){
        memset(&msg, 0, sizeof(msg));
        recv(newSd, (char*)&msg, sizeof(msg), 0);
        if(!strcmp(msg, "exit"))
        {
            cout << "Client with ClientID: " << newSd << " has quit the session" << endl;
            delete_sequence.push(ma[newSd]);
            ma.erase(newSd);
            close(newSd);
            return NULL;
        }
        if(!strcmp(msg, "online"))
        {
        	string data;
            map<int, pthread_t*>::iterator itr;
            ostringstream s1;
            s1 << "\tClientID\tThreadID\n"; 
            /*data = s1.str();
            s1.clear();
            memset(&msg, 0, sizeof(msg));
        	strcpy(msg, data.c_str());
            send(newSd, (char*)&msg, strlen(msg), 0);*/
    		for (itr = ma.begin(); itr != ma.end(); ++itr) { 
        		s1 << '\t' << '\t' << itr->first << "\t" << "\t" << *(itr->second) << "\n"; 
        		/*data = s1.str();
            	s1.clear();
        		memset(&msg, 0, sizeof(msg));
        		strcpy(msg, data.c_str());
        		send(newSd, (char*)&msg, strlen(msg), 0);*/
    		} 
    		s1 << "\n";
    		data = s1.str();
    		s1.clear();
    		memset(&msg, 0, sizeof(msg));
        	strcpy(msg, data.c_str());
        	send(newSd, (char*)&msg, strlen(msg), 0);
            continue;
        }
        if(msg[0] >= '0' && msg[0] <= '9'){
        	int deliver_to = stoi(msg);
        	if(ma.find(deliver_to) == ma.end())
        		continue;
        	ostringstream s1;
        	string data(msg);
        	s1 << "From " << newSd << " to " << data;
        	data = s1.str();
        	memset(&msg, 0, sizeof(msg));
        	strcpy(msg, data.c_str());
        	send(deliver_to, (char*)&msg, strlen(msg), 0);
        	continue;
        }
        cout << "Client" << newSd << ": " << msg << endl;
    }
    return NULL;
}

void newconnection(int newSd){
	pthread_t *x = new pthread_t;
    pthread_create(x, NULL, receiver, (void *)&newSd);
	ma[newSd] = x;
	cout << "Connected with Client. ClientID: " << newSd << endl;
	return;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    int port = atoi(argv[1]);
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, 
        sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    cout << "Actively waiting for a clients to connect..." << endl;
    pthread_t send, clean;
    pthread_create(&send, NULL, sender, NULL);
    pthread_create(&clean, NULL, cleaner, NULL);
    listen(serverSd, 5);
    sockaddr_in newSockAddr;
    while(1){
    	socklen_t newSockAddrSize = sizeof(newSockAddr);
    	int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    	if(newSd < 0)
    	{
        	cerr << "Error accepting request from client!" << endl;
        	continue;
    	}
    	newconnection(newSd);
    }
    pthread_join(send, NULL);
    pthread_join(clean, NULL);
    close(serverSd);
    cout << "Connection closed..." << endl;
    return 0;   
}
