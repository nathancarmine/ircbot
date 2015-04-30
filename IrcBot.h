#ifndef IRCBOT_H_
#define IRCBOT_H_

#include <string>

struct node {
    int value;
    std::string word;
    node *next;
    node *prev;
};

struct affe {
    int affeLvl;
    std::string name;
    node *next;
};

class IrcBot {
public:
	IrcBot(const char *_nick, const char *_server, const char *_channel, const char *_usr);
	virtual ~IrcBot();

	bool setup;
	void start();
	bool charSearch(const char *toSearch, const char *searchFor);
	void botHelp();

	void botMath(const char *buf);
	void botRoot(const char *buf);
	void botHypot(const char *buf);
	void botQuadForm(const char *buf);
	void botTrig(const char *buf);

	void quoteDelete(char *buf);
	void quoteAdd(char *buf);
	void quotePrint(char *buf);
	void quotePrintAll(char *buf);
	void affectionDis(char *buf);
	void affectionCom(char *buf);

private:
	const char *port;
	int s; //the socket descriptor
	int c;
	node *root;
	affe *point;

	const char *nick;
	const char *server;
	const char *channel;
	const char *usr;

	bool isConnected(const char *buf);
	const char * timeNow();
	bool sendData(const char *msg);
	void sendPong(const char *buf);
	
	void botFramework(const char *buf);
	void privMsg(const char *privmsg);
};

#endif /* IRCBOT_H_ */
