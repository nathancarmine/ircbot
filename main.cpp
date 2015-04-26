#include <iostream>
#include "IrcBot.h"
//#include "IrcBot.cpp"
//Needed for VIM^


using namespace std;


int main()
{
	IrcBot bot = IrcBot("NICK NotexactlyReal\r\n","USER guest tolmoon tolsun :Ronnie Reagan\r\n");
	bot.start();

  return 0;

}
