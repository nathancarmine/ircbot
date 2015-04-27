#include <iostream>
#include "IrcBot.cpp"


using namespace std;


int main() {
	IrcBot bot = IrcBot("NICK BeedaBoop\r\n","127.0.0.1","#newchan","USER guest word1 word2: user\r\n");
	bot.start();

  return 0;
}
