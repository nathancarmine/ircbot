#ifndef IRCBOT_H_
#define IRCBOT_H_

class IrcBot
{
public:
	IrcBot(const char *_nick, const char *_server, const char *_channel, const char *_usr);
	virtual ~IrcBot();

	bool setup;

	void start();
	bool charSearch(const char *toSearch, const char *searchFor);

private:
	const char *port;
	int s; //the socket descriptor

	const char *nick;
	const char *server;
	const char *channel;
	const char *usr;

	bool isConnected(const char *buf);

	const char * timeNow();

	bool sendData(const char *msg);

	void sendPong(const char *buf);

	void msgHandel(const char *buf);

	void botMath(const char *buf);
};

#endif /* IRCBOT_H_ */
