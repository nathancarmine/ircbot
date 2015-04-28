# ircbot
### Project Summary
We aim to create the framework for a bot that could connect to an irc server (namely freenode) and perform various tasks based off user input. Some of these tasks may include:
* Doing calculations
* Store quotes from other users
* Other fun common social media functions (poking/slapping)
* A bot affection meter to a user

### How to run
With your IRC client connected to an IRC server create a new channel. Run the program with three command line arguments, first will be the bots nickname, second will be the server address and the channel will be entered last with quotes around the channel name. So [nickname] [server address] [channel] for example the local server would look something like this: Botty 127.0.0.1 "#Botting" (remember quotes for the channel name). The bot should join the channel with the nickname you gave it. After it joins there are some triggers the bot is looking for before it will reply.

### Dependencies
A connection to an IRC server is required. We recommend that you do not connect to a server on the internet as this bot might get you banned on one of those servers. Instead use a local server connection. We suggest that you install ircd-hybrid to use as your local server. Here is a quick installation guide for linux.

### System requirements
Tested only on Ubuntu 14.04, will probably work on other distros. May work on OS X with the proper IRC applications installed. 
### Group Members
Nathan Carmine  
Connor Carlson
#### Contributors
Tyler Allen from the-tech-tutorial.com  
Jacob Grunwald  
### Known bugs
None yet!
