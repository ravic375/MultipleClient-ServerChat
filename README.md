# MultipleClient-ServerChat

## Server:
Compile with : g++ Server.cpp -o server <br />
Execute with : ./server 1234 <br />
here 1234 is \<desired port number\> <br />
you can view Server IP Address with command: hostname -I <br />
Sender Thread actively looks for the messages to be sent. <br />
Send Message to Client         : \<clientID\> \<message\> <br />
View active clientIDs          : show <br />


## Client:
Compile with: g++ Client.cpp -o client <br />
Execute with: ./client 127.0.0.1 1234 <br />
here 127.0.0.1 is \<the Server IP Address\> <br />
here 1234 is \<desired port number\> <br />
Send Message to other Client   : \<clientID\> \<message\> <br />
Send Message to Server         : \<message\> <br />
View active clientIDs          : online <br />
