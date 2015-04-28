# ircbot
### Project Summary
We aim to create the framework for a bot that could connect to an irc server and perform various tasks based off user input. Some of these tasks may include:
* Doing calculations
* Store quotes from other users
* Other fun common social media functions (poking/slapping)
* A bot affection meter to a user

### How to run
We recommend using g++ to compile the code:  
`g++ main.cpp -o IrcBot`  
If you do not have g++ installed:  
`sudo apt-get install g++`

With your IRC client connected to an IRC server create a new channel. Run the program with three command line arguments, first will be the bots nickname, second will be the server address and the channel will be entered last with quotes around the channel name. So [nickname] [server address] [channel] for example the local server would look something like this:  
`./IrcBot Botty 127.0.0.1 "#Botting"`  
Be sure to use quotes around the channel name. The bot should then join the channel on the server with the nickname you gave it. After it joins there are some [message triggers](https://github.com/nathancarmine/ircbot/wiki/IRC-Bot-Response-Triggers) the bot is looking for before it will reply.

When rebooting the bot, first stop it by pressing ctrl-z. Be sure to **terminate** it before running it again (even if you recompiled). Otherwise, a name conflict error will occur. To terminate it, you must kill the process. You may have to kill the process more than once to terminate it, as the first time may only stop the process. To kill a process, type `kill %[job_number]`. For instance, if the bot is the first process, type `kill %1`. This is roughly how this should look.
![TERMINATE_ALL_ROBOTS](http://i.imgur.com/f8esBgL.png)

### Dependencies
A connection to an IRC server is required. We recommend that you do not connect to a server on the internet as this bot might get you banned on one of those servers. Instead use a local server connection. We suggest that you install ircd-hybrid to use as your local server. [Here](https://github.com/nathancarmine/ircbot/wiki/How-to-install-a-local-server) is a quick installation guide for linux.  
An IRC client version is required as well for communication with the bot. If you don't know what an IRC client is then [here](http://en.wikipedia.org/wiki/Internet_Relay_Chat) is the wikipedia page on it. Since you will need to install an IRC client to test our program [here](https://github.com/nathancarmine/ircbot/wiki/Installing-and-Using-Irssi) is an installation link for Irssi. If you want to use your own IRC client make sure it can create new channels, since the bot will be joining you in that channel.  
The rest of our dependencies are:  
* \#include "IrcBot.h"  
* \#include \<iostream>  
* \#include \<stdio.h>  
* \#include \<stdlib.h>  
* \#include \<sstream>  
* \#include \<math.h>  
* \#include \<algorithm>  
* \#include \<unistd.h>  
* \#include \<errno.h>  
* \#include \<string.h>  
* \#include \<sys/types.h>  
* \#include \<sys/socket.h>  
* \#include \<netinet/in.h>  
* \#include \<netdb.h>  
* \#include \<arpa/inet.h>  
* \#include \<sys/wait.h>  
* \#include \<signal.h>  
* \#include \<time.h>  

### System requirements
Tested only on Ubuntu 14.04, will probably work on other distros. May work on OS X with the proper IRC applications installed. 
### Group Members
Nathan Carmine  
Connor Carlson
#### Contributors
Tyler Allen from the-tech-tutorial.com  
Jacob Grunwald  
### Known Bugs/Open Issues
When the bot first joins the server, it may not reply to the first command, like so: ![like so](http://i.imgur.com/NXMdAqd.png)  
However, it will immediately reply to each subsequent command.

IrcBot::botTrig:
Integrating division into the trig functions would be nice, so cos 3pi/2 would work properly, for instance.
Also, typing "Bot: cos 2pa" will compute cos 2, instead of flagging an error.

Another known issue is that .q add will be apart of the quote when it gets turned to a string.  

One suggestion would be to figure out a way to retrieve usernames in the IRC channel and create the affection linked list off of the usernames retrieved creating an even better affection function all together. Other than that someone could impliment some other cool things for the bot like a quick little text game. 
