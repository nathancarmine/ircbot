#ifndef IRCBOT_H_
#define IRCBOT_H_

#include <string>

struct node {
    int value;
	std::string word;
    node *next;
    node *prev;
};

class IrcBot
{
public:

    IrcBot(const char *_nick, const char *_server, const char *_channel, const char *_usr);
    virtual ~IrcBot();

    bool setup;

    void start();
    bool charSearch(const char *toSearch,const char *searchFor);

private:
    const char *port;
    int s; //the socket descriptor
    int c;
    node *root;

    const char *nick;
    const char *server;
    const char *channel;
    const char *usr;

    bool isConnected(const char *buf);
    const char * timeNow();
    bool sendData(const char *msg);
    void sendPong(char *buf);
    void botFramework(char *buf);

    void quoteDelete(char *buf);
    void quoteAdd(char *buf);
    void quotePrint(char *buf);
    void quotePrintAll(char *buf);

    void privMsg(const char *privmsg);
    void botMath(const char *buf);
};

#endif /* IRCBOT_H_ */
