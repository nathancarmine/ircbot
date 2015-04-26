#ifndef IRCBOT_H_
#define IRCBOT_H_

class IrcBot
{
public:
    IrcBot(const char * _nick,const char * _usr);
    virtual ~IrcBot();

    bool setup;

    void start();
    bool charSearch(const char *toSearch,const char *searchFor);

private:
    const char *port;
    int s; //the socket descriptor

    const char *nick;
    const char *usr;

    bool isConnected(char *buf);
    char * timeNow();
    bool sendData(const char *msg);
    void sendPong(char *buf);
    void botFramework(char *buf);
};

#endif /* IRCBOT_H_ */
