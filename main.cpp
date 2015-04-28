#include "IrcBot.cpp"
#include <cstdlib>
#include <iostream>

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
