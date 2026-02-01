# liquidbot
stuff so you can make selfbot stuff




std::string liquidLogin(std::string token)
logs in with a token and returns userid


UserInfo liquidGetInfo(std::string accountID)
gets all of the information on a logged in account
returns UserInfo struct that has display name, username, pfp, etc


std::vector<std::string> liquidGetActiveAccounts()
returns a vector of all accounts that are logged in


std::string liquidSend(std::string accountID, std::string channelID, std::string content)
sends a message to a channel
returns the message id of the sent message


void liquidReact(std::string accountID, std::string channelID, std::string messageID, std::string emoji)
adds a reaction to a message


void liquidDelete(std::string accountID, std::string channelID, std::string messageID)
deletes a message


std::vector<Friend> liquidFriends(std::string accountID)
fetches everyone in an accounts friends lsit and if the dm is open/closed
returns a vectord for 'friend' structs


void liquidCreateChannel(std::string accountID, std::string guildID, std::string name, int type)
creates a new channel


void liquidDeleteChannel(std::string accountID, std::string channelID)
deletes a channel or closes a dm


void liquidKick(std::string accountID, std::string guildID, std::string userID)
kicks someone from a server


void liquidBan(std::string accountID, std::string guildID, std::string userID)
bans someone from a server


void liquidTimeout(std::string accountID, std::string guildID, std::string userID, std::string duration)
times out a someone from a server
you can add a duration in formats: 1h, 1d, 1m


void liquidListen(std::string accountID, std::string channelID)
starts a thread that listens for messages on a channel


void liquidPrefix(std::string newPrefix)
sets the prefix for commands. default prefix is '!'


void liquidOnCommand(std::string name, callback)
registers a function to run when a command is triggered
callback signature: void(string accountID, string channelID, vector<string> args)


void liquidWhitelistUser(std::string userID)
adds a user user id to the command whitelist. you cant run commands on an account unless you are whitelisted
the account youre running the bot on is always whitelisted


void liquidRPC(std::string status)
useless because its not done
