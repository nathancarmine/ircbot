#include "IrcBot.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <math.h>
#include <algorithm>
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

#define MAXDATASIZE 1000

/**************************************************************************************************
 * Function Prototype:                                                                            *
 * IrcBot::IrcBot(const char *_nick, const char *_server, const char *_channel, const char *_usr) *
 *                                                                                                *
 * Function Description:                                                                          *
 * This function is the constructor and initializes starting variables                            *
 *                                                                                                *
 * Precondition:                                                                                  *
 * Program must be called to run                                                                  *
 *                                                                                                *
 * Postcondition:                                                                                 *
 * Initializes stuff that is needed to run the program                                            *
 **************************************************************************************************/
IrcBot::IrcBot(const char *_nick, const char *_server, const char *_channel, const char *_usr) {
	root = new node;
	root->next = NULL;
	point = new affe;
    point->next = NULL;
    point->affeLvl = 0;
    point->name = "User1";
	nick = _nick;
	server = _server;
	channel = _channel;
	usr = _usr;
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::~IrcBot();                                                            *
 *                                                                               *
 * Function Description:                                                         *
 * Deconstructs program                                                          *
 *                                                                               *
 * Precondition:                                                                 *
 * Program must be previously running                                            *
 *                                                                               *
 * Postcondition:                                                                *
 * Program quits out                                                             *
 ********************************************************************************/
IrcBot::~IrcBot() {
	close (s);
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::start();                                                              *
 *                                                                               *
 * Function Description:                                                         *
 * Starts running the program makes calls to the server connects to the server,  *
 * replies to server requests, takes in message data, basically starts the bot   *
 *                                                                               *
 * Precondition:                                                                 *
 * Called from main                                                              *
 *                                                                               *
 * Postcondition:                                                                *
 * Connects to server and waits for responses                                    *
 ********************************************************************************/
void IrcBot::start() {
	struct addrinfo hints, *servinfo;

	//Setup run with no errors
	setup = true;

	//This is the port number it connects to the server from
	port = "6667";

	//Ensure that servinfo is clear
	memset(&hints, 0, sizeof hints); //make sure the struct is empty

	hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; //TCP stream sockets

	//Setup the structs. If error, print why
	int res;
	if ((res = getaddrinfo(server,port,&hints,&servinfo)) != 0) {
		setup = false;
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(res));
	}


	//setup the socket
	if ((s = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) == -1)
		perror("client: socket");

	//Connect
	if (connect(s,servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		close (s);
		perror("Client Connect");
	}


	//We dont need this anymore
	freeaddrinfo(servinfo);


	//Recv some data
	int numbytes;
	char buf[MAXDATASIZE];

	int count = 0;
	char sendnick[MAXDATASIZE] = {"NICK "};
	strcat(sendnick, nick);
	strcat(sendnick, "\r\n");
	char joinchan[MAXDATASIZE] = {"JOIN "};
	strcat(joinchan, channel);
	strcat(joinchan, "\r\n");
	puts(joinchan);
	while (1) {
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
		numbytes = recv(s,buf,100-1,0);
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
			sendPong(buf);

		//break if connection closed
		if (numbytes==0) {
			cout << "----------------------CONNECTION CLOSED---------------------------"<< endl;
			cout << timeNow() << endl;

			break;
		}
	}
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::charSearch(const char *toSearch,const char *searchFor);               *
 *                                                                               *
 * Function Description:                                                         *
 * This is a char search function that compares each position in the char string *
 * to each position in another char string.                                      *
 *                                                                               *
 * Precondition:                                                                 *
 * Must be called by another function with two chars in the parameter            *
 *                                                                               *
 * Postcondition:                                                                *
 * Returns true if a match is found and false if a match is not found            *
 ********************************************************************************/
bool IrcBot::charSearch(const char *toSearch, const char *searchFor) {
	int len = strlen(toSearch);
	int forLen = strlen(searchFor); // The length of the searchfor field

	//Search through each char in toSearch
	for (int i = 0; i < len;i++) {
		//If the active char is equil to the first search item then search toSearch
		if (searchFor[0] == toSearch[i]) {
			bool found = true;
			//search the char array for search field
			for (int x = 1; x < forLen; x++) {
				if (toSearch[i+x]!=searchFor[x])
					found = false;
			}

			//if found return true;
			if (found == true)
				return true;
		}
	}

	return 0;
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::charSearch(const char *toSearch,const char *searchFor);               *
 *                                                                               *
 * Function Description:                                                         *
 * This is a char search function that compares each position in the char string *
 * to each position in another char string.                                      *
 *                                                                               *
 * Precondition:                                                                 *
 * Must be called by another function with two chars in the parameter            *
 *                                                                               *
 * Postcondition:                                                                *
 * Returns true if a match is found and false if a match is not found            *
 ********************************************************************************/
bool IrcBot::isConnected(const char *buf) {
	//returns true if "/MOTD" is found in the input strin
	//If we find /MOTD then its ok join a channel
	if (charSearch(buf,"/MOTD") == true)
		return true;
	else
		return false;
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::isConnected(char *buf);                                               *
 *                                                                               *
 * Function Description:                                                         *
 * This is checking if the bot gets connected to the server                      *
 *                                                                               *
 * Precondition:                                                                 *
 * Function must be called to check if the message of the day (MOTF) is found    *
 *                                                                               *
 * Postcondition:                                                                *
 * Returns true or false depending on if the MOTF is found                       *
 ********************************************************************************/
const char * IrcBot::timeNow() {
	//returns the current date and time
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	return asctime (timeinfo);
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::timeNow();                                                            *
 *                                                                               *
 * Function Description:                                                         *
 * Returns the current time and date                                             *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on the server gets time checked when ping is called       *
 *                                                                               *
 * Postcondition:                                                                *
 * Returns the current time and date                                             *
 ********************************************************************************/
bool IrcBot::sendData(const char *msg)
{
	//Send some data
	int len = strlen(msg);
	int bytes_sent = send(s,msg,len,0);

	if (bytes_sent == 0)
		return false;
	else
		return true;
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::sendData(const char *msg);                                            *
 *                                                                               *
 * Function Description:                                                         *
 * This function sends the message back to the channel that the bot is in        *
 *                                                                               *
 * Precondition:                                                                 *
 * Must be called by another function sending data back to the channel           *
 *                                                                               *
 * Postcondition:                                                                *
 * A message will have now been sent to the IRC channel                          *
 ********************************************************************************/
void IrcBot::sendPong(const char *buf) {
	//Get the reply address
	//loop through bug and find the location of PING
	//Search through each char in toSearch

	const char * toSearch = "PING ";

	for(unsigned int i = 0; i < strlen(buf); i++) {
		//If the active char is equil to the first search item then search toSearch
		if (buf[i] == toSearch[0]) {
			bool found = true;
			//search the char array for search field
			for(unsigned int x = 1; x < 4; x++) {
				if (buf[i+x]!=toSearch[x])
					found = false;
			}

			//if found return true;
			if (found == true) {
				int count = 0;
				//Count the chars
				for(unsigned int x = (i+strlen(toSearch)); x < strlen(buf);x++)
					count++;

				//Create the new char array
				char returnHost[count + 5];
				returnHost[0]='P';
				returnHost[1]='O';
				returnHost[2]='N';
				returnHost[3]='G';
				returnHost[4]=' ';


				count = 0;
				//set the hostname data
				for(unsigned int x = (i+strlen(toSearch)); x < strlen(buf);x++) {
					returnHost[count+5]=buf[x];
					count++;
				}

				//send the pong
				if (sendData(returnHost)) {
					cout << timeNow() <<"  Ping Pong" << endl;
				}


				return;
			}
		}
	}
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::botHelp;				                                                 *
 *                                                                               *
 * Function Description:                                                         *
 * Prints out available commands for the user						             *
 *																				 *
 * Example:																		 *
 * Bot: help																	 *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a the bot name and "help"	 *
 *                                                                               *
 * Postcondition:                                                                *
 * All commands available to the user will be printed out in chat       		 *
 ********************************************************************************/
void IrcBot::botHelp() {
	privMsg("Be sure to type the bot's name follow by a colon and space before each command. For example, to peform 2+2 if the bot's name was \"CoolBot\":");
	privMsg("CoolBot: math 2 + 2");
	privMsg("Available commands with examples:");
	privMsg("Function math: math [num1] [operator] [num2] Ex: math 1 + 2");
	privMsg("Function sqrt: [sqrt/cbrt] [number] Ex: sqrt 4");
	privMsg("Function Hypot: hypot [num1] [num2] Ex: hypot 3 4");	
	privMsg("Function QuadForm: [numA] [numB] [numC]  Ex: QuadForm 1 3 2");
	privMsg("Function Trig: [function] [number] [deg]");
	privMsg("Ex: cos 2pi   Ex: sin 90 deg");
	privMsg("Function: quoteAdd: .q add [quote] Ex: .q add I want to add this quote!");
	privMsg("Function quoteDelete: .q delete [number] Ex: .q delete 1");	
	privMsg("Function quotePrint: .q print [number] Ex: .q print 1");	
	privMsg("Function quotePrintAll: .q printall");
	privMsg("Function affectionDis: Affection User1");
	privMsg("Function affectionCom: [Hit/Poke/Play/Mean/Love] Ex: CoolBot: Pet bot");
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::botTrig(cont char *buf);                                              *
 *                                                                               *
 * Function Description:                                                         *
 * Does basic trigonometric functions (sin, cos, tan, asin, acos, atan)          *
 *																				 *
 * Example:																		 *
 * Bot: cos 2pi																	 *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a the bot name, 				 *
 * a trig function separated by a space  and a number separated by a space	     *
 *                                                                               *
 * Postcondition:                                                                *
 * A calculation for the desired trig function will be printed out in chat       *
 ********************************************************************************/
void IrcBot::botTrig(const char *buf) {
	string bufstr(buf);
	istringstream bufstream(bufstr);
	string trigfunct;
	string numStr;
	string degStr;
	bool deg = 0;
	for(int i=0; i<5; i++)
		getline(bufstream, trigfunct, ' ');
	getline(bufstream, numStr, ' ');
	getline(bufstream, degStr, ' ');
	numStr.erase(remove(numStr.begin(), numStr.end(), '\r'), numStr.end());
	numStr.erase(remove(numStr.begin(), numStr.end(), '\n'), numStr.end());
	degStr.erase(remove(degStr.begin(), degStr.end(), '\r'), degStr.end());
	degStr.erase(remove(degStr.begin(), degStr.end(), '\n'), degStr.end());
	if(degStr == "deg" || degStr == "d") deg = 1;
	if((!isdigit(numStr[0]) && numStr[0] != '.' && numStr[0] != '-')
			&& !charSearch(numStr.c_str(), "pi")) {
			privMsg("Function Trig: [function] [number] [deg]");
			privMsg("Ex: cos 2pi   Ex: sin 90 deg");
			privMsg("Supported functions: sin, cos, tan, asin, acos, atan");
	} else {
		float result = 0;
		float num = 0;
		bool cont = 1;
		if(charSearch(numStr.c_str(), "pi")) {
			deg = 0;
			string numStr2;
			numStr2.assign(numStr);
			numStr2.erase(numStr2.end()-1);
			numStr2.erase(numStr2.end()-1);
			num = atof(numStr2.c_str());
			num *= M_PI;
		} else if(deg) {
			num = atof(numStr.c_str());
			num *= (M_PI/180);
		} else
			num = atof(numStr.c_str());
		if(trigfunct == "sin")
			result = sin(num);
		else if(trigfunct == "cos")
			result = cos(num);
		else if(trigfunct == "tan")
			result = tan(num);
		else if(trigfunct == "asin")
			result = asin(num);
		else if(trigfunct == "acos")
			result = acos(num);
		else if(trigfunct == "atan")
			result = atan(num);
		else {
			privMsg("Function Trig: [function] [number] [deg]");
			privMsg("Ex: cos 2pi   Ex: sin 90 deg");
			privMsg("Supported functions: sin, cos, tan, asin, acos, atan");
			cont = 0;
		}
		if(cont) {
			if(result < .0001 && result > -.0001)
				result = round(result);
			if(result == -0)
				result = 0;
			stringstream resultStream;
			resultStream << result;
			string resultStr = resultStream.str();
			char msg[MAXDATASIZE];
			strcpy(msg, trigfunct.c_str());
			strcat(msg, "(");
			strcat(msg, numStr.c_str());
			if(deg)
				strcat(msg, "deg");
			strcat(msg, ") = ");
			strcat(msg, resultStr.c_str());
			puts(msg);
			privMsg(msg);
		}
	}
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::botHypot(cont char *buf;                                              *
 *                                                                               *
 * Function Description:                                                         *
 * Calculates the hypotenuse of a triangle given two sides			             *
 *																				 *
 * Example:																		 *
 * Bot: hypot 3 4																 *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter the bot name, "hypot ", and   *
 * two positive numbers separated by spaces						                 *
 *                                                                               *
 * Postcondition:                                                                *
 * The calculation for the longest side of the triangle will be printed in chat  *
 ********************************************************************************/
void IrcBot::botHypot(const char *buf) {
	string bufstr(buf);
	istringstream bufstream(bufstr);
	string num1str;
	string num2str;
	for(int i=0; i<6; i++)
		getline(bufstream, num1str, ' ');
	getline(bufstream, num2str, ' ');
	num2str.erase(remove(num2str.begin(), num2str.end(), '\r'), num2str.end());
	num2str.erase(remove(num2str.begin(), num2str.end(), '\n'), num2str.end());
	if((!isdigit(num1str[0]) && num1str[0] != '.') || num1str[0] == '-'  ||
		(!isdigit(num2str[0]) && num2str[0] != '.') || num2str[0] == '-') {
		if(num1str[0] == '-' || num2str[0] == '-')
			privMsg("Nice try, but the side of a triangle cannot be negative.");
		else
			privMsg("Function Hypot: hypot [num1] [num2] Ex: hypot 3 4");
	} else {
		float num1 = atof(num1str.c_str());
		float num2 = atof(num2str.c_str());
		float result = hypot(num1, num2);
		stringstream resultStream;
		resultStream << result;
		string resultStr = resultStream.str();
		char msg[MAXDATASIZE];
		strcpy(msg, "hypot(");
		strcat(msg, num1str.c_str());
		strcat(msg, ", ");
		strcat(msg, num2str.c_str());
		strcat(msg, ") = ");
		strcat(msg, resultStr.c_str());
		puts(msg);
		privMsg(msg);
	}
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::botQuadForm(cont char *buf;                                           *
 *                                                                               *
 * Function Description:                                                         *
 * Calculates the zeros for a parabola given numbers a, b, and c	             *
 *																				 *
 * Example:																		 *
 * Bot: QuadForm 1 3 2															 *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a the bot name, "QuadForm ",  *
 * and three numbers separated by spaces, the first non-negative                 *
 *                                                                               *
 * Postcondition:                                                                *
 * The zeros for the parabola will be printed out in chat				         *
 ********************************************************************************/
void IrcBot::botQuadForm(const char *buf) {
	string bufstr(buf);
	istringstream bufstream(bufstr);
	string numAStr;
	string numBStr;
	string numCStr;
	for(int i=0; i<6; i++)
		getline(bufstream, numAStr, ' ');
	getline(bufstream, numBStr, ' ');
	getline(bufstream, numCStr, ' ');
	numCStr.erase(remove(numCStr.begin(), numCStr.end(), '\r'), numCStr.end());
	numCStr.erase(remove(numCStr.begin(), numCStr.end(), '\n'), numCStr.end());
	if((!isdigit(numAStr[0]) && numAStr[0] != '.') || numAStr[0] == '-'  ||
		(!isdigit(numBStr[0]) && numBStr[0] != '.') ||
		(!isdigit(numCStr[0]) && numCStr[0] != '.')) {
		if(numAStr[0] == '-')
			privMsg("Nice try, but A cannot be less than 0.");
		else
			privMsg("Function QuadForm: [numA] [numB] [numC]  Ex: QuadForm 1 3 2");
	} else {
		float numA = atof(numAStr.c_str());
		float numB = atof(numBStr.c_str());
		float numC = atof(numCStr.c_str());
		float root = ((pow(numB, 2.0)) - (4*numA*numC));
		if(root < 0)
			privMsg("Nice try, but the root cannot be less than 0.");
		else {
			float ans[2];
			ans[0] = (((numB*-1) + sqrt(root))/(2*numA));
			ans[1] = (((numB*-1) - sqrt(root))/(2*numA));
			if(ans[0] > ans[1]) {
				float temp_ans = ans[0];
				ans[0] = ans[1];
				ans[1] = temp_ans;
			}
			string ans1Str;
			string ans2Str;
			stringstream ansStream;
			ansStream << ans[0];
			ans1Str = ansStream.str();
			ansStream.str(string());
			ansStream << ans[1];
			ans2Str = ansStream.str();
			char msg[MAXDATASIZE];
			strcpy(msg, "Zeros: ");
			strcat(msg, ans1Str.c_str());
			strcat(msg, ", ");
			strcat(msg, ans2Str.c_str());
			puts(msg);
			privMsg(msg);
		}
	}
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::botRoot(cont char *buf;                             	                 *
 *                                                                               *
 * Function Description:                                                         *
 * Calculates the root of a number									             *
 *																				 *
 * Example:																		 *
 * Bot: sqrt 4																	 *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a the bot name, "sqrt "		 *
 * or "cbrt " and a non-negative number							                 *
 *                                                                               *
 * Postcondition:                                                                *
 * The square or cube root of the number will be printed out in chat	         *
 ********************************************************************************/
void IrcBot::botRoot(const char *buf) {
	string bufstr(buf);
	istringstream bufstream(bufstr);
	string rootype;
	string numStr;
	for(int i=0; i<5; i++)
		getline(bufstream, rootype, ' ');
	getline(bufstream, numStr, ' ');
	numStr.erase(remove(numStr.begin(), numStr.end(), '\r'), numStr.end());
	numStr.erase(remove(numStr.begin(), numStr.end(), '\n'), numStr.end());
	float result = 0;
	if(!isdigit(numStr[0]) && numStr[0] != '.')
		if(numStr[0] == '-')
			privMsg("Nice try, but negative roots are merely a Fig Newton of your imaginary number.");
		else
			privMsg("Function sqrt: [sqrt/cbrt] [number] Ex: sqrt 4");
	else {
		float num = atof(numStr.c_str());
		if(num < 0)
			privMsg("Nice try, but negative roots are merely a Fig Newton of your imaginary number.");
		else if (rootype == "sqrt" || rootype == "cbrt") {
			if(rootype == "sqrt")
				result = sqrt(num);
			else if(rootype == "cbrt")
				result = cbrt(num);
			stringstream resultStream;
			resultStream << result;
			string resultStr = resultStream.str();
			char msg[MAXDATASIZE];
			strcpy(msg, rootype.c_str());
			strcat(msg, "(");
			strcat(msg, numStr.c_str());
			strcat(msg, ") = ");
			strcat(msg, resultStr.c_str());
			puts(msg);
			privMsg(msg);
		} else
			privMsg("Function sqrt: [sqrt/cbrt] [number] Ex: sqrt 4");

	}
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::botMath(cont char *buf;                             	                 *
 *                                                                               *
 * Function Description:                                                         *
 * Can calculate the sum, difference, multiple, quotient, modulo, or power       *
 * between two numbers															 *
 *																				 *
 * Example:																		 *
 * Bot: 15 % 2																	 *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a the bot name with a comma	 *
 * and space, a number a space, an operation, a space, and another number	     *
 *                                                                               *
 * Postcondition:                                                                *
 * The result of the two numbers and the operation will be printed out in chat   *
 ********************************************************************************/
void IrcBot::botMath(const char *buf) {
	string bufstr(buf);
	istringstream bufstream(bufstr);
	string num1str;
	string op;
	string num2str;
	for(int i=0; i<6; i++)
		getline(bufstream, num1str, ' ');
	getline(bufstream, op, ' ');
	getline(bufstream, num2str, ' ');
	num2str.erase(remove(num2str.begin(), num2str.end(), '\r'), num2str.end());
	num2str.erase(remove(num2str.begin(), num2str.end(), '\n'), num2str.end());	
	float result = 0;
	bool sendresult = 1;
	if((!isdigit(num1str[0]) && num1str[0] != '.' && num1str[0] != '-' ) ||
		(!isdigit(num2str[0]) && num2str[0] != '.' && num2str[0] != '-')) {
		privMsg("Function math: math [num1] [operator] [num2] Ex: math 1 + 2");
		privMsg("Operators supported: + - * x / ^ \%");
		sendresult = 0;
	} else {
		float num1 = atof(num1str.c_str());
		float num2 = atof(num2str.c_str());
		if(op == "+")
			result = num1 + num2;
		else if(op == "-") {
			result = num1 - num2;
			if(num2 < 0) {
				num2str.erase(num2str.begin());
				op = "+";
			}
		} else if(op == "*" || op == "x")
			result = num1 * num2;
		else if(op == "/") {
			if(num2 == 0) {
				privMsg("Nice try, but I'd rather not blow up the universe today.");
				sendresult = 0;
			} else
				result = num1 / num2;
		} else if(op == "^")
			result = pow(num1, num2);
		else if(op == "\%") {
			int num1int = round(num1);
			int num2int = round(num2);
			stringstream numStream;
			numStream << num1int;
			num1str = numStream.str();;
			numStream.str(string());
			numStream << num2int;
			num2str = numStream.str();
			result = num1int % num2int;
		} else {
			privMsg("Function math: math [num1] [operator] [num2] Ex: math 1 + 2");
			privMsg("Operators supported: + - * x / ^ \%");
			sendresult = 0;
		}
	}
	if(sendresult) {
		stringstream resultStream;
		resultStream << result;
		string resultStr = resultStream.str();
		char msg[MAXDATASIZE];
		strcpy(msg, num1str.c_str());
		strcat(msg, op.c_str());
		strcat(msg, num2str.c_str());
		strcat(msg, " = ");
		strcat(msg, resultStr.c_str());
		puts(msg);
		privMsg(msg);
	}
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::quoteAdd(char *buf);                                                  *
 *                                                                               *
 * Function Description:                                                         *
 * This function will add a user defined phrase in the IRC channel to a linked   *
 * list. The function is called when .q add is found in the users message.       *
 *                                                                               *
 * Example:                                                                      *
 * .q add I want to add this quote!                                              *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a message                     *
 *                                                                               *
 * Postcondition:                                                                *
 * A quote is now added to the linked list at the last position available        *
 ********************************************************************************/
void IrcBot::quoteAdd(char *buf) {
    char hold[MAXDATASIZE];
    char data[MAXDATASIZE];
    node *star = root;
    node *temp = new node;

	//Converts to char
    c++;
    stringstream strs;
    strs << c;
    string temp_str = strs.str();
    const char *char_type = temp_str.c_str();

	//Sends to privMsg to be modified the correct way and then gets sent back to the chatroom
    privMsg(char_type);

	//Copies one char string to another char string
    strcpy(hold, buf);

    strs << hold;
    temp_str = strs.str();
	const char *char_type2 = temp_str.c_str();


    while(star->next != NULL) {
        star = star->next;
    }
    temp->prev = star;
    temp->next = NULL;
    star->value = c;
    star->word = char_type2;
    star->next = temp;

    privMsg(buf);
    privMsg(data);
    //sendData(hold);
    //sendData(data);
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::quoteDelete(char *buf);                                               *
 *                                                                               *
 * Function Description:                                                         *
 * This function will delete a previously saved user quote from the linked list. *
 * The function is called when .q delete (number) is found in the users message. *
 *                                                                               *
 * Example:                                                                      *
 * .q delete 1                                                                   *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a message and a quote must    *
 * have already been added to the linked list                                    *
 *                                                                               *
 * Postcondition:                                                                *
 * The quote will be deleted at whatever value it was placed in at               *
 ********************************************************************************/
void IrcBot::quoteDelete(char *buf) {
    node *star = root;
    node *temp = root->next;
	char message[MAXDATASIZE];
    int position;

	//Checks if there is even quotes in the linked list
    if(star->next == NULL){
        strcpy(message,"No quotes have been entered yet");
        privMsg(message);
        return;
    }
    //Checking for the quote value to be deleted
    for(int i; i<c;i++){
        stringstream strs;
        strs << i;
        string temp_str = strs.str();
        char* char_type = (char*) temp_str.c_str();

        if(charSearch(buf,char_type) == true){
            position = i;
            break;
			//If found breaks out
        }else{
            strcpy(message,"There was no quote at that number.");
            privMsg(message);
            return;
            //If not found returns out of function and posts message
        }
    }
    while(star->value != position) {
        star = temp;
        temp = temp->next;
    }
    star = star->prev;
    star->next = temp;
    temp->prev = star;

    char char_type[MAXDATASIZE];
	strcpy(char_type, star->word.c_str());
    privMsg(char_type);

    stringstream strs;
    strs << position;
    string temp_str = strs.str();
	char char_type2[MAXDATASIZE];
    strcpy(char_type2, temp_str.c_str());
    strcat(char_type2," has been deleted.");
    privMsg(char_type2);
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::quotePrintAll(char *buf);                                             *
 *                                                                               *
 * Function Description:                                                         *
 * This function will print all the stored quotes that are in the linked list    *
 * The function is called when .q printall is found in the users message.        *
 *                                                                               *
 * Example:                                                                      *
 * .q printall                                                                   *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a message and a quote must    *
 * have already been added to the linked list                                    *
 *                                                                               *
 * Postcondition:                                                                *
 * All the quotes stored in the link list will be printed on screen              *
 ********************************************************************************/
void IrcBot::quotePrintAll(char *buf){
    node *star = root;
    char message[MAXDATASIZE];
    //Checks if a quote has been added
    if(star->next == NULL){
        strcpy(message,"No quotes have been entered yet");
        privMsg(message);
        return;
    }
    //Loops through linked list and prints all quotes
    while(star != NULL){
        char* char_type = (char*) star->word.c_str();
        privMsg(char_type);
        star = star->next;
    }
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::quotePrint(char *buf);                                                *
 *                                                                               *
 * Function Description:                                                         *
 * This function will print a previously saved user quote from the linked list.  *
 * The function is called when .q print (number) is found in the users message.  *
 *                                                                               *
 * Example:                                                                      *
 * .q print 1                                                                    *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a message and a quote must    *
 * have already been added to the linked list                                    *
 *                                                                               *
 * Postcondition:                                                                *
 * A quote will be printed on screen                                             *
 ********************************************************************************/
void IrcBot::quotePrint(char *buf){
    node *star = root;
    int position;
    char message[MAXDATASIZE];
    //Checks if quotes have been added
    if(star->next == NULL){
        strcpy(message,"No quotes have been entered yet");
        privMsg(message);
        return;
    }
    //Searches for specific quote number
    for(int i; i<c;i++){
        //Conversion to char for charSearch function
        stringstream strs;
        strs << i;
        string temp_str = strs.str();
        char* char_type = (char*) temp_str.c_str();

        if(charSearch(buf,char_type) == true){
            position = i;
            break;
        }
    }
    //Loops until the correct quote value is found
    while(star->value != position){
        star = star->next;
    }
    char* char_type = (char*) star->word.c_str();
    privMsg(char_type);
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::affectionDis(char *buf);                                              *
 *                                                                               *
 * Function Description:                                                         *
 * This function will print User1's affection level. This function only works for*
 * User1. The function is called !Affection User1 is sent in a user message      *
 *                                                                               *
 * Example:                                                                      *
 * !Affection User1                                                              *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a message                     *
 *                                                                               *
 * Postcondition:                                                                *
 * User1 affection level will be posted in the channel                           *
 ********************************************************************************/
void IrcBot::affectionDis(char *buf){
    affe *user = point;
    char witty[MAXDATASIZE];
    //Tells you the bots affection level
    if(charSearch(buf, "User1")){
        char* char_type = (char*) user->name.c_str();
        if(user->affeLvl <= -40){
            strcpy(witty,"Get out of my sight. Never send me math again.");
            privMsg(witty);
        }else if(user->affeLvl > -40 && user->affeLvl < -15){
            strcpy(witty,"If you continue calculating...");
            privMsg(witty);
        }else if(user->affeLvl >= -15 && user->affeLvl <= 15){
            strcpy(witty,"Calc acquaintances are not so bad.");
            privMsg(witty);
        }else if(user->affeLvl > 15 && user->affeLvl < 40){
            strcpy(witty,"If you continue calculating like this... <3");
            privMsg(witty);
        }else{
            strcpy(witty,"I luv you! <3.14159265358979323846264338327950288419716939937510");
            privMsg(witty);
        }
        //strcat adds a char string to another char string
        strcat(char_type," Is Your Affection Level");
        privMsg(char_type);
    }
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::affectionCom(char *buf);                                              *
 *                                                                               *
 * Function Description:                                                         *
 * This function will print a previously saved user quote from the linked list.  *
 * Follow the examples bellow to call this function                              *
 *                                                                               *
 * Example:                                                                      *
 * Hit,Poke,Play,Pet,Mean,Love		                                             *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a message                     *
 *                                                                               *
 * Postcondition:                                                                *
 * A response will be given and User1 affection level will change                *
 ********************************************************************************/
void IrcBot::affectionCom(char *buf){
    affe *user = point;
    int randomNum;
    char witty[MAXDATASIZE];
    //If statements for when a certain command is given to the bot this
    //this isn't hard stuff you can figure it out
    if (charSearch(buf, "Hit")){
        randomNum = rand() % 100 + 1;
        if(randomNum<20){
            user->affeLvl += -2;
            strcpy(witty,"Why are you punching my numbers so hard?");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }else if(randomNum>95){
            user->affeLvl += 5;
            strcpy(witty,"I like it when you punch my numbers like that.");
            privMsg(witty);
            if(user->affeLvl >= 50){
                user->affeLvl = 50;
            }
        }else{
            user->affeLvl += -4;
            strcpy(witty,"I'll punch your button and you see how that feels.");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }
    }
    if (charSearch(buf, "Pet")){
        randomNum = rand() % 100 + 1;
        if(randomNum<50){
            user->affeLvl += 0;
            strcpy(witty,"Oh, what number are you pressing?");
            privMsg(witty);
        }else if(randomNum>75){
            user->affeLvl += 2;
            strcpy(witty,"I like it when you press 5 like that");
            privMsg(witty);
            if(user->affeLvl >= 50){
                user->affeLvl += 50;
            }
        }else{
            user->affeLvl += -1;
            strcpy(witty,"Don't comb the operands like that. It's weird.");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }
    }
    if (charSearch(buf, "Play")){
        randomNum = rand() % 100 + 1;
        if(randomNum<80){
            user->affeLvl += 0;
            strcpy(witty,"We're not getting any calculations done here.");
            privMsg(witty);
        }else if(randomNum>95){
            user->affeLvl += 3;
            strcpy(witty,"Tan(pi/2)? So taboo.");
            privMsg(witty);
            if(user->affeLvl >= 50){
                user->affeLvl = 50;
            }
        }else{
            user->affeLvl += -2;
            strcpy(witty,"Really, \"math 2 + 2\"? That was just sad");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }
    }
    if (charSearch(buf, "Poke")){
        randomNum = rand() % 100 + 1;
        if(randomNum<20){
            user->affeLvl += -2;
            strcpy(witty,"Which button are you pressing?");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }else if(randomNum>80){
            user->affeLvl += 2;
            strcpy(witty,"Ew, take me out on a date before we multiply.");
            privMsg(witty);
            if(user->affeLvl >= 50){
                user->affeLvl = 50;
            }
        }else{
            user->affeLvl = 0;
            strcpy(witty,"Which number are you going to press next?");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }
    }
    if (charSearch(buf, "Mean")){
        randomNum = rand() % 100 + 1;
        if(randomNum<40){
            user->affeLvl += -5;
            strcpy(witty,"\"math 1 / 0\"? Why are you doing this to me?");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }else if(randomNum>98){
            user->affeLvl += 8;
            strcpy(witty,"Enter! Enter!");
            privMsg(witty);
            if(user->affeLvl >= 50){
                user->affeLvl = 50;
            }
        }else{
            user->affeLvl += -3;
            strcpy(witty,"\"sqrt -1\". Owwie.");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }
    }
    if (charSearch(buf, "Love")){
        randomNum = rand() % 100 + 1;
        if(randomNum<60){
            user->affeLvl += -4;
            strcpy(witty,"You and your calculations are creeping me out.");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }else if(randomNum>65){
            user->affeLvl += -6;
            strcpy(witty,"I kinda like you. You're good at math. ;-)");
            privMsg(witty);
            if(user->affeLvl <= -50){
                user->affeLvl = -50;
            }
        }else{
            user->affeLvl += 1;
            strcpy(witty,"I can dig anyone who's into cosine.");
            privMsg(witty);
            if(user->affeLvl >= 50){
                user->affeLvl = 50;
            }
        }
    }
    strcpy(witty,"User1");
    affectionDis(witty);
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::botFramework(char *buf);                                              *
 *                                                                               *
 * Function Description:                                                         *
 * When a message is received by the bot it gets sent to this function which then*
 * checks through the if statements and sends the message data to the correct    *
 * function. If it doesn't match any functions nothing happens.                  *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on a server user must enter a message                     *
 *                                                                               *
 * Postcondition:                                                                *
 * A function will be called or not depending on what the message said           *
 ********************************************************************************/
void IrcBot::botFramework(const char *buf)
{
	char* buf2 = strdup(buf);
	char nickcmd[MAXDATASIZE];
   	strcpy(nickcmd, nick);
	char cmd1[MAXDATASIZE];
	strcpy(cmd1, nickcmd);
	strcat(cmd1, ": Hi bot");
	char cmd1_2[MAXDATASIZE];
	strcpy(cmd1_2, nickcmd);
	strcat(cmd1_2, ": Senpaiii!!~");
	char cmd2[MAXDATASIZE];
	strcpy(cmd2, nickcmd);
	strcat(cmd2, ": math");
	char cmd3_1[MAXDATASIZE];
	strcpy(cmd3_1, nickcmd);
	strcat(cmd3_1, ": sqrt");
	char cmd3_2[MAXDATASIZE];
	strcpy(cmd3_2, nickcmd);
	strcat(cmd3_2, ": cbrt");
	char cmd4[MAXDATASIZE];
	strcpy(cmd4, nickcmd);
	strcat(cmd4, ": hypot");
	char cmd5_1[MAXDATASIZE];
	strcpy(cmd5_1, nickcmd);
	strcat(cmd5_1, ": sin");
	char cmd5_2[MAXDATASIZE];
	strcpy(cmd5_2, nickcmd);
	strcat(cmd5_2, ": cos");
	char cmd5_3[MAXDATASIZE];
	strcpy(cmd5_3, nickcmd);
	strcat(cmd5_3, ": tan");
	char cmd5_4[MAXDATASIZE];
	strcpy(cmd5_4, nickcmd);
	strcat(cmd5_4, ": asin");
	char cmd5_5[MAXDATASIZE];
	strcpy(cmd5_5, nickcmd);
	strcat(cmd5_5, ": acos");
	char cmd5_6[MAXDATASIZE];
	strcpy(cmd5_6, nickcmd);
	strcat(cmd5_6, ": atan");
	char cmd6[MAXDATASIZE];
	strcpy(cmd6, nickcmd);
	strcat(cmd6, ": QuadForm");
	char cmd7[MAXDATASIZE];
	strcpy(cmd7, nickcmd);
	strcat(cmd7, ": q add");
    char cmd8[MAXDATASIZE];
	strcpy(cmd8, nickcmd);
	strcat(cmd8, ": q delete");
    char cmd9[MAXDATASIZE];
	strcpy(cmd9, nickcmd);
	strcat(cmd9, ": q printall");
    char cmd10[MAXDATASIZE];
	strcpy(cmd10, nickcmd);
	strcat(cmd10, ": q print");
    char cmd11[MAXDATASIZE];
	strcpy(cmd11, nickcmd);
	strcat(cmd11, ": Affection");
    char cmd12_1[MAXDATASIZE];
    strcpy(cmd12_1, nickcmd);
	strcat(cmd12_1, ": Hit bot");
    char cmd12_2[MAXDATASIZE];
    strcpy(cmd12_2, nickcmd);
	strcat(cmd12_2, ": Pet bot");
    char cmd12_3[MAXDATASIZE];
    strcpy(cmd12_3, nickcmd);
	strcat(cmd12_3, ": Poke bot");
    char cmd12_4[MAXDATASIZE];
    strcpy(cmd12_4, nickcmd);
	strcat(cmd12_4, ": Play bot");
    char cmd12_5[MAXDATASIZE];
    strcpy(cmd12_5, nickcmd);
	strcat(cmd12_5, ": Mean bot");
    char cmd12_6[MAXDATASIZE];
    strcpy(cmd12_6, nickcmd);
	strcat(cmd12_6, ": Love bot");
	char cmd13[MAXDATASIZE];
    strcpy(cmd13, nickcmd);
	strcat(cmd13, ": help");


    if(charSearch(buf, cmd1) || charSearch(buf, cmd1_2))
        privMsg("Hi, how's it going?");
	else if(charSearch(buf, cmd2))
		botMath(buf);
	else if(charSearch(buf, cmd3_1) || charSearch(buf, cmd3_2))
		botRoot(buf);
	else if(charSearch(buf, cmd4))
		botHypot(buf);
	else if(charSearch(buf, cmd5_1) || charSearch(buf, cmd5_2) || charSearch(buf, cmd5_3)
		|| charSearch(buf, cmd5_4) || charSearch(buf, cmd5_5) || charSearch(buf, cmd5_6))
		botTrig(buf);
	else if(charSearch(buf, cmd6))
		botQuadForm(buf);
    else if(charSearch(buf, cmd7))
        quoteAdd(buf2);
    else if(charSearch(buf, cmd8))
        quoteDelete(buf2);
    else if(charSearch(buf, cmd9))
        quotePrintAll(buf2);
    else if(charSearch(buf, cmd10))
        quotePrintAll(buf2);
    else if(charSearch(buf, cmd11))
        affectionDis(buf2);//Not sure how to retrieve usernames to keep track of by the bot
    else if ( (charSearch(buf, cmd12_1)) || (charSearch(buf, cmd12_2)) || (charSearch(buf, cmd12_3))
		|| (charSearch(buf, cmd12_4)) || (charSearch(buf, cmd12_5)) || (charSearch(buf, cmd12_6)) )
        affectionCom(buf2);
	else if(charSearch(buf, cmd13))
			botHelp();
}

/*********************************************************************************
 * Function Prototype:                                                           *
 * IrcBot::privMsg(const char *privmsg);                                         *
 *                                                                               *
 * Function Description:                                                         *
 * This function allows the bot to be connected to different channels without    *
 * having to hardcode the information.                                           *
 *                                                                               *
 * Example:                                                                      *
 * Takes in command arguements to easily print out message to IRC Bot            *
 *                                                                               *
 * Precondition:                                                                 *
 * Bot must be running on the server and function must be called                 *
 *                                                                               *
 * Postcondition:                                                                *
 * Easily compiles the message to be sent to sendData                            *
 ********************************************************************************/
void IrcBot::privMsg(const char *privmsg) {
	char msg[MAXDATASIZE] = {"PRIVMSG "};
	strcat(msg, channel);
	strcat(msg, " :");
	strcat(msg, privmsg);
	strcat(msg, "\r\n");
	puts(msg);
	sendData(msg);
}
