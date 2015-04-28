# ircbot
### Project Summary
We aim to create the framework for a bot that could connect to an irc server (namely freenode) and perform various tasks based off user input. Some of these tasks may include:
* Doing calculations
* Store quotes from other users
* Other fun common social media functions (poking/slapping)
* Possibly short simple text game
* A bot affection meter to a user

### How to run
[to be included]
### Dependencies
A connection to an IRC server is required. We recommend that you do not connect to a server on the internet as this bot might get you banned on one of those servers. Instead use a local server connection. We suggest that you install ircd-hybrid to use as your local server. Here's a quick installation guide for linux.

apt-get install ircd-hybrid\n
This will install an IRC server on your linux device.\n
Now you can get out your IRC client and join the server.\n
To connect to the server use the IP address 127.0.0.1\n
The port for the connection should already be set as 6667\n
If you would like to set the Message of The Day use:\n
sudo nano /etc/ircd-hybrid/ircd.motd (If you prefer vim use vim instead of nano)\n
Reset the server with:\n
sudo /etc/init.d/ircd-hybrid restart\n
Now you're set to go with your IRC server\n

### System requirements
Should run on all systems with an internet connection
### Group Members
Nathan Carmine  
Connor Carlson
#### Contributors
Tyler Allen from the-tech-tutorial.com
### Known bugs
None yet!
