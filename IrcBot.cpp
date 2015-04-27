#include "IrcBot.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

using namespace std;

#define MAXDATASIZE 100


IrcBot::IrcBot(const char *_nick, const char *_server, const char *_channel, const char *_usr)
{
    root = new node;
    root->next = NULL;
	nick = _nick;
	server = _server;
	channel = _channel;
	usr = _usr;
}

IrcBot::~IrcBot()
{
	close (s);
}

void IrcBot::start()
{
	struct addrinfo hints, *servinfo;

	//Setup run with no errors
	setup = true;

	port = "6667";

	//Ensure that servinfo is clear
	memset(&hints, 0, sizeof hints); // make sure the struct is empty

	//setup hints
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	//Setup the structs if error print why
	int res;
	if ((res = getaddrinfo(server/*"127.0.0.1""irc.freenode.net"*/,port,&hints,&servinfo)) != 0)
	{
		setup = false;
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(res));
	}


	//setup the socket
	if ((s = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) == -1)
	{
		perror("client: socket");
	}

	//Connect
	if (connect(s,servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		close (s);
		perror("Client Connect");
	}


	//We dont need this anymore
	freeaddrinfo(servinfo);


	//Recv some data
	int numbytes;
	char buf[MAXDATASIZE];

	int count = 0;
	char sendnick[1000] = {"NICK "};
	strcat(sendnick, nick);
	strcat(sendnick, "\r\n");
	char joinchan[1000] = {"JOIN "};
	strcat(joinchan, channel);
	strcat(joinchan, "\r\n");
	puts(joinchan);
	while (1)
	{
		//declars
		count++;

		switch (count) {
			case 3:
					//after 3 recives send data to server (as per IRC protacol)
					sendData(sendnick);
					sendData(usr);
				break;
			case 4:
					//Join a channel after we connect, this time we choose beaker
				//sendData("JOIN #newchan\r\n");
				sendData(joinchan);
			default:
				break;
		}



		//Recv & print Data
		numbytes = recv(s,buf,MAXDATASIZE-1,0);
		buf[numbytes]='\0';
		cout << buf;
		//buf is the data that is recived

		//Pass buf to the message handeler
		botFramework(buf);


		//If Ping Recived
		/*
		 * must reply to ping overwise connection will be closed
		 * see http://www.irchelp.org/irchelp/rfc/chapter4.html
		 */
		if (charSearch(buf,"PING"))
		{
			sendPong(buf);
		}

		//break if connection closed
		if (numbytes==0)
		{
			cout << "----------------------CONNECTION CLOSED---------------------------"<< endl;
			cout << timeNow() << endl;

			break;
		}
	}
}


bool IrcBot::charSearch(const char *toSearch, const char *searchFor)
{
	int len = strlen(toSearch);
	int forLen = strlen(searchFor); // The length of the searchfor field

	//Search through each char in toSearch
	for (int i = 0; i < len;i++)
	{
		//If the active char is equil to the first search item then search toSearch
		if (searchFor[0] == toSearch[i])
		{
			bool found = true;
			//search the char array for search field
			for (int x = 1; x < forLen; x++)
			{
				if (toSearch[i+x]!=searchFor[x])
				{
					found = false;
				}
			}

			//if found return true;
			if (found == true)
				return true;
		}
	}

	return 0;
}

bool IrcBot::isConnected(const char *buf)
{//returns true if "/MOTD" is found in the input strin
	//If we find /MOTD then its ok join a channel
	if (charSearch(buf,"/MOTD") == true)
		return true;
	else
		return false;
}

const char * IrcBot::timeNow()
{//returns the current date and time
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	return asctime (timeinfo);
}


bool IrcBot::sendData(const char *msg)
{//Send some data
	//Send some data
	int len = strlen(msg);
	int bytes_sent = send(s,msg,len,0);

	if (bytes_sent == 0)
		return false;
	else
		return true;
}

void IrcBot::sendPong(char *buf)
{
	//Get the reply address
	//loop through bug and find the location of PING
	//Search through each char in toSearch

	const char * toSearch = "PING ";

	for (unsigned int i = 0; i < strlen(buf);i++)
		{
			//If the active char is equil to the first search item then search toSearch
			if (buf[i] == toSearch[0])
			{
				bool found = true;
				//search the char array for search field
				for (int x = 1; x < 4; x++)
				{
					if (buf[i+x]!=toSearch[x])
					{
						found = false;
					}
				}

				//if found return true;
				if (found == true)
				{
					int count = 0;
					//Count the chars
					for (unsigned int x = (i+strlen(toSearch)); x < strlen(buf);x++)
					{
						count++;
					}

					//Create the new char array
					char returnHost[count + 5];
					returnHost[0]='P';
					returnHost[1]='O';
					returnHost[2]='N';
					returnHost[3]='G';
					returnHost[4]=' ';


					count = 0;
					//set the hostname data
					for (unsigned int x = (i+strlen(toSearch)); x < strlen(buf);x++)
					{
						returnHost[count+5]=buf[x];
						count++;
					}

					//send the pong
					if (sendData(returnHost))
					{
						cout << timeNow() <<"  Ping Pong" << endl;
					}


					return;
				}
			}
		}

}

void IrcBot::botMath(const char *buf) {
	string bufstr(buf);
	istringstream bufstream(bufstr);
	string num1;
	string op;
	string num2;
	for(int i=0; i<3; i++)
		getline(bufstream, num1, ' ');
	getline(bufstream, op, ' ');
	getline(bufstream, num2, ' ');
	char *num1c = new char[num1.size()+1];
	//char *opc = new char[1];
	//char *num2c;
	num1c[num1.size()]=0;
	memcpy(num1c, num1.c_str(), num1.size());
	//sendData(
}

void IrcBot::botFramework(char * buf)
{
    /*
     * TODO: add you code to respod to commands here
     * the example below replys to the command hi scooby
     */
    char nickcmd[1000];
   	strcpy(nickcmd, nick);
	char nickcmd1[1000];
	strcpy(nickcmd1, nickcmd);
	strcat(nickcmd1, ": Hello bot");
	char nickcmd2[1000];
	strcpy(nickcmd2, nickcmd);
    if(charSearch(buf, nickcmd1))
        privMsg("Hi, how's it going?");
	else if(charSearch(buf, "Bot: math"))
		botMath(buf);

    if (charSearch(buf,"Hello bot"))
    {
        sendData("PRIVMSG #Botting :Hi, How is it going\r\n");
    }
    if (charSearch(buf,".q add")){
        quoteAdd(buf);
    }
    if (charSearch(buf, ".q delete")){
        quoteDelete(buf);
    }
    if (charSearch(buf, ".q print all")){
        quotePrintAll(buf);
    }
}

void IrcBot::quoteAdd(char *buf){
    char hold[MAXDATASIZE];
    char data[MAXDATASIZE];
    node *star = root;
    node *temp = new node;

    c++;
    stringstream strs;
    strs << c;
    string temp_str = strs.str();
    char* char_type = (char*) temp_str.c_str();

    strcpy(data,"PRIVMSG #Botting :Quote Stored Position ");
    strcat(data,char_type);
    strcat(data,"\r\n");
    //strcpy(data,"PRIVMSG #Botting :");
    strcpy(hold,buf);
    strcat(hold,"\r\n");

    strs << hold;
    temp_str = strs.str();

    while(star->next != NULL){
        star = star->next;
    }
    temp->prev = star;
    temp->next = NULL;
    star->value = c;
    star->word = temp_str.c_str();
    star->next = temp;

    sendData(hold);
    sendData(data);
}

void IrcBot::quoteDelete(char *buf){
    node *star = root;
    node *temp = root->next;
    int position;

    for(int i; i<c;i++){
        stringstream strs;
        strs << i;
        string temp_str = strs.str();
        char* char_type = (char*) temp_str.c_str();

        if(charSearch(buf,char_type) == true){
            position = i;
            break;
        }
    }
    while(star->value != position){
        star = temp;
        temp = temp->next;
    }
    star = star->prev;
    star->next = temp;
    temp->prev = star;

    char* char_type = (char*) star->word.c_str();
    privMsg(char_type);

    stringstream strs;
    strs << position;
    string temp_str = strs.str();
    char_type = (char*) temp_str.c_str();
    strcat(char_type," Has been deleted.");
    privMsg(char_type);
    sendData("PRIVMSG #Botting :Deleting function works\r\n");
}

void IrcBot::quotePrintAll(char *buf){
    node *star = root;
    //char hold[MAXDATASIZE];
    while(star != NULL){
        char* char_type = (char*) star->word.c_str();
        //sendData(star->word);
        privMsg(char_type);
        sendData("PRIVMSG #Botting :Looping Print Function\r\n");
        star = star->next;
    }
}

void IrcBot::quotePrint(char *buf){
    node *star = root;
    int position;

    for(int i; i<c;i++){
        stringstream strs;
        strs << i;
        string temp_str = strs.str();
        char* char_type = (char*) temp_str.c_str();

        if(charSearch(buf,char_type) == true){
            position = i;
            break;
        }
    }
    while(star->value != position){
        star = star->next;
    }
    char* char_type = (char*) star->word.c_str();
    privMsg(char_type);
    sendData("PRIVMSG #Botting :Printing function works\r\n");
}

void IrcBot::privMsg(const char *privmsg) {
	char msg[1000] = {"PRIVMSG "};
	strcat(msg, channel);
	strcat(msg, " :");
	strcat(msg, privmsg);
	strcat(msg, "\r\n");
	puts(msg);
	sendData(msg);
}
