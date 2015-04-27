#include <iostream>
//#include "IrcBot.cpp"
#include "IrcBot.h"
#include <stdlib.h>

using namespace std;


int main(int argc, const char* argv[]) {
		//quotes are required around channel input
	if(argv[3] == NULL) {
		cout<<"Quotes are required around the channel input"<<endl;
		exit(EXIT_FAILURE);
	}
	IrcBot bot = IrcBot(argv[1], argv[2], argv[3], "USER guest word1 word2: user\r\n");

	bot.start();


  return 0;
}
