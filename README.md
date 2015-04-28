# ircbot
### Project Summary
We aim to create the framework for a bot that could connect to an irc server (namely freenode) and perform various tasks based off user input. Some of these tasks may include:
* Doing calculations
* Store quotes from other users
* Other fun common social media functions (poking/slapping)
* Possibly short simple text game
* A bot affection meter to a user

### How to run
With an IRC server connected to in your IRC client create a new channel. Run the program with three command line arguements first will be the bots nickname, second will be the server address and the channel will be entered last with quotes around the channel name. So [nickname] [server address] [channel] (remember quotes for channel) for example the local server would look something like this: Botty 127.0.0.1 "#Botting". The bot should join the channel with the nickname you gave it. After it joins there are some triggers the bot is looking for before it will reply.

### Dependencies
A connection to an IRC server is required. We recommend that you do not connect to a server on the internet as this bot might get you banned on one of those servers. Instead use a local server connection. We suggest that you install ircd-hybrid to use as your local server. Here's a quick installation guide for linux.

apt-get install ircd-hybrid  
This will install an IRC server on your linux device.  
Now you can get out your IRC client and join the server.  
To connect to the server use the IP address 127.0.0.1  
The port for the connection should already be set as 6667  
If you would like to set the Message of The Day use:  
sudo nano /etc/ircd-hybrid/ircd.motd (If you prefer vim use vim instead of nano)  
Reset the server with:  
sudo /etc/init.d/ircd-hybrid restart  
Now you're set to go with your IRC server  

### System requirements
Tested only on Ubuntu 14.04, will probably work on other distros. May work on OS X with the proper IRC applications installed. 
### Group Members
Nathan Carmine  
Connor Carlson
#### Contributors
Tyler Allen from the-tech-tutorial.com
### Known bugs
None yet!
