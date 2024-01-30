#include "Server.hpp"

Server::~Server() {
}

Server::Server(int argc, char **argv) {
	if (argc != 3) {
		std::string error_message;
		std::string program_name = argv[0];
		error_message = "Usage: " + program_name + " <port> <password>\n";
		error_handling(error_message);
	}

	ServerSocketInit(argv);
	KqueueInit();
	p_server_info();
}

bool	Server::Run(void) {
	// main loop of ircserv with kqueue
	int nev;
	while (true) {
		nev = kevent(kq_, &(chlist_[0]), chlist_.size(), evlist_, FT_KQ_EVENT_SIZE, &timeout_);
		chlist_.clear();
		if (nev == -1)
			error_handling("kevent() error\n");
		else if (nev == 0)
			HandleTimeout();
		else if (nev > 0)
			HandleEvents(nev);

		DeleteInvalidClient();
		print_clients();
		print_channels();
	}
	return true;
}

void	Server::ServerSocketInit(char **argv) {
	name_ = FT_SERVER_NAME;
	port_ = atoi(argv[1]);
	version_ = IRC_VERSION;
	password_ = argv[2];
	createtime_ = set_create_time();

	if ((sock_ = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		error_handling("socket() error\n");
	
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_.sin_port = htons(port_);
	int reuse = 1;
	setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	if (bind(sock_, (struct sockaddr*)&addr_, sizeof(addr_)) == -1)
		error_handling("socket bind() error\n");
	if (listen(sock_, FT_SOCK_QUEUE_SIZE) == -1)
		error_handling("socket listen() error\n");
	fcntl(sock_, F_SETFL, O_NONBLOCK);
}

void	Server::KqueueInit(void) {
	if ((kq_ = kqueue()) == -1)
		error_handling("kqueue() error\n");
	struct kevent	server_event;
	EV_SET(&server_event, sock_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	chlist_.push_back(server_event);
	timeout_.tv_sec = FT_TIMEOUT_SEC;
	timeout_.tv_nsec = FT_TIMEOUT_NSEC;
}

std::string	Server::set_create_time(void) {
	std::time_t now = std::time(0);
	std::tm* localTime = std::localtime(&now);

	if (localTime->tm_hour >= 24) {
		localTime->tm_hour -= 24;
	}
	
	std::ostringstream timeStream;
	timeStream << localTime->tm_year + 1900 << "-" 
		<< localTime->tm_mon + 1 << "-" << localTime->tm_mday << " "
		<< localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec;

	return timeStream.str();
}

const std::string&	Server::get_name(void) {
	return this->name_;
}

const std::string&	Server::get_version(void) {
	return this->version_;
}

const std::string&	Server::get_create_time(void) {
	return this->createtime_;
}

const int& Server::get_port(void) {
	return this->port_;
}

const struct sockaddr_in&	Server::get_addr(void) {
	return this->addr_;
}

std::map<std::string, Channel>& Server::get_channels(void) {
	return this->channels_;
}

Channel*	Server::get_channel_ptr(const std::string& name) {
	Channel *ret = NULL;

	std::map<std::string, Channel>::iterator	itr = this->channels_.find(name);
	if (itr != this->channels_.end())
		ret = &(itr->second);
	return ret;
}

//need Fix
std::string	Server::SearchClientBySock(const int& sock) {
	std::string	nick;

	std::map<int, Client>::iterator	itr = this->clients_.find(sock);
	if (itr != this->clients_.end())
		nick = (itr->second).get_nick();
	return nick;
}

int	Server::SearchClientByNick(const std::string& nick) {
	int	ret = FT_INIT_CLIENT_FD;
	std::string	tmp_nick;

	std::map<int, Client>::iterator	iter = this->clients_.begin();
	while (iter != this->clients_.end()) {
		tmp_nick = (iter->second).get_nick();
		if (nick.compare(tmp_nick) == 0) {
			ret = iter->first;
			break ;
		}
		iter++;
	}
	return ret;
}

bool	Server::SearchChannelByName(const std::string& name) {
	bool	search_ret = false;
	std::map<std::string, Channel>::iterator	itr = this->channels_.find(name);
	if (itr != this->channels_.end())
		search_ret = true;
	return search_ret;
}

void	Server::HandleEvents(int nev) {
	struct kevent	event;
	for (int i = 0; i < nev; ++i) {
		event = evlist_[i];
		if (event.flags & EV_ERROR)
			HandleEventError(event);
		else if (event.flags & EV_EOF)
			DisconnectClient(event.ident);//need to implement
		else if (event.filter == EVFILT_READ && event.ident == (uintptr_t)sock_)
			ConnectClient();
		else if (event.filter == EVFILT_READ)
			HandleClientEvent(event);
		else if (event.filter == EVFILT_WRITE)
			DisconnectClient(event.ident);
	}
}

void	Server::HandleClientEvent(struct kevent event) {
	Client	*client = &(clients_[event.ident]);
	char	buff[FT_BUFF_SIZE];
	std::string& buffer = buffers_[client->get_sock()];
	int read_byte = read(event.ident, buff, sizeof(buff));
	if (read_byte == -1) 
		log::cout << "client read error\n";
	else if (read_byte == 0)
		DisconnectClient(event.ident);
	else {
		buff[read_byte] = '\0';
		buffer += buff;
		std::vector<Command *> cmds;
		int	offset;
		cmds = Request::ParseRequest(this, client, buffer, &offset);
		for (size_t i = 0; i < cmds.size(); ++i)
			cmds[i]->Run();
		buffer.erase(0, offset);
	}
}

void	Server::ConnectClient(void) {
	Client	client;
	if (client.set_sock(accept(sock_, (struct sockaddr*)&client.addr_, &client.addr_size_)) == -1)
		error_handling("accept() error\n");

	/* handle new client */
	AddEvent(client.get_sock(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	clients_[client.get_sock()] = client;
	buffers_[client.get_sock()] = "";
	log::cout << CYAN << "accent new client: " << client.get_sock() << RESET << "\n";
}

bool	Server::AuthPassword(const std::string& password) {
	if (this->password_.compare(password) == 0)
		return true;
	return false;
}

void	Server::AddEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent	event;
	EV_SET(&event, ident, filter, flags, fflags, data, udata);
	chlist_.push_back(event);
}

void	Server::HandleEventError(struct kevent event) {
	if (event.ident == (uintptr_t)this->sock_)
		error_handling("server socket event error\n");
	else
		DisconnectClient(event.ident);//need to implement fucntion
}

void	Server::AddDeleteClient(const int& sock) {
	del_clients_.insert(sock);
}

void	Server::DeleteInvalidClient(void) {
	std::set<int>::iterator	itr = this->del_clients_.begin();
	while (itr != this->del_clients_.end()) {
		DisconnectClient(*itr);
		itr++;
	}
	this->del_clients_.clear();
}

void	Server::CeaseChannel(const std::string& channel_name) {
	DeleteChannel(channel_name);
}

void	Server::DeleteChannel(const std::string& channel_name) {
	/*Channel should be deleted when the last participant PART from that channel */
	std::map<std::string, Channel>::iterator	ch_itr;
	ch_itr = this->channels_.find(channel_name);
	if (ch_itr != this->channels_.end())
		this->channels_.erase(ch_itr);
}

void	Server::DisconnectClient(const int& sock) {
	std::vector<std::string>	channels_of_client;
	std::map<int, Client>::iterator	client_it;
	
	client_it = clients_.find(sock);
	if (client_it == clients_.end()) {
		return;
	}

	struct kevent del_evlist[2];
	EV_SET(&del_evlist[0], sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	EV_SET(&del_evlist[1], sock, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(this->kq_, del_evlist, 2, NULL, 0, NULL);
	close(sock);

	channels_of_client = client_it->second.get_channels();
	
	/* Channel 에서 Client 삭제 */
	std::vector<std::string>::iterator channel_itr = channels_of_client.begin();
	int	left_client_num = 0;
	while (channel_itr != channels_of_client.end()) {
		
		left_client_num = this->channels_[*channel_itr].Kick(sock);

		if (left_client_num < 1)
			DeleteChannel(*channel_itr);
		channel_itr++;
	}
	
	this->clients_.erase(client_it);
}

void	Server::HandleTimeout(void) {
	/* handle timeout */
	log::cout << "time out!\n";
}

void	Server::p_server_info(void) {
	log::cout << "IRC server:" << GREEN << this->name_ << RESET << " started at ";
	log::cout << GREEN << inet_ntoa(this->addr_.sin_addr) << RESET;
	log::cout << " :: " << GREEN << this->port_ << RESET << "\n";
}

/* wooseoki functions */
void	Server::print_event(struct kevent *event, int i) {
	
	log::cout << "=============================\n";
	log::cout << "index : " << i << "\n";
	log::cout << "ident : " << event->ident << "\n";
	p_event_filter(event);
	p_event_flags(event);
	log::cout << "=============================\n";
}

void	Server::p_event_filter(struct kevent *event) {
	log::cout << "filter : " << GREEN;
	if (event->filter == EVFILT_READ) log::cout << "EVFILT_READ";
	else if (event->filter == EVFILT_WRITE) log::cout << "EVFILT_WRITE";
	else if (event->filter == EVFILT_AIO) log::cout << "EVFILT_AIO";
	else if (event->filter == EVFILT_VNODE) log::cout << "EVFILT_VNODE";
	else if (event->filter == EVFILT_PROC) log::cout << "EVFILT_PROC";
	else if (event->filter == EVFILT_SIGNAL) log::cout << "EVFILT_SIGNAL";
	else if (event->filter == EVFILT_EXCEPT) log::cout << "EVFILT_EXCEPT";
	else log::cout << "unknown EVFILT";
	log::cout << "\n" << RESET;
}

void	Server::p_event_flags(struct kevent *event) {
	log::cout << "flags : " << GREEN;
	if (event->flags & EV_ADD) log::cout << "EV_ADD | ";
	if (event->flags & EV_DELETE) log::cout << "EV_DELETE | ";
	if (event->flags & EV_ENABLE) log::cout << "EV_ENABLE | ";
	if (event->flags & EV_DISABLE) log::cout << "EV_DISABLE | ";
	if (event->flags & EV_ONESHOT) log::cout << "EV_ONESHOT | ";
	if (event->flags & EV_CLEAR) log::cout << "EV_CLEAR | ";
	if (event->flags & EV_RECEIPT) log::cout << "EV_RECEIPT | ";
	if (event->flags & EV_OOBAND) log::cout << "EV_OOBAND | ";
	if (event->flags & EV_ERROR) log::cout << "EV_ERROR | ";
	if (event->flags & EV_EOF) log::cout << "EV_EOF | ";
	log::cout << RESET << "\n";
}

// queries for command process
bool	Server::get_channel_members(std::map<int, std::string>* ret, \
									const std::string& channel_name, \
									const int& flag) {
	if (SearchChannelByName(channel_name) == false)
		return false;

	std::set<int>	client_list;
	if (flag & FT_CH_MEMBER)
		client_list = this->channels_[channel_name].get_members();
	else if (flag & FT_CH_OPERATOR)
		client_list = this->channels_[channel_name].get_operators();
	else if (flag & FT_CH_BAN_LIST)
		client_list = this->channels_[channel_name].get_ban_list();

	std::set<int>::iterator	itr = client_list.begin();
	while (itr != client_list.end()) {
		std::pair<int, std::string>	tmp;
		tmp.first = *itr;
		tmp.second = this->clients_[tmp.first].get_nick();
		ret->insert(tmp);
		itr++;
	}
	return true;
}

bool	Server::AddChannelMember(const std::string& channel_name, \
								const int& flag, \
								const int& sock) {
	if ((flag & FT_CH_MEMBER || flag & FT_CH_OPERATOR))
		this->channels_[channel_name].Join(sock);
	if (flag & FT_CH_OPERATOR)
		this->channels_[channel_name].PromoteMember(sock);
	return true;
}

void	Server::print_clients(void) {
	log::cout << BOLDWHITE << "_________CLIENT INFO_________\n" << RESET;
	std::map<int, Client>::iterator itr = clients_.begin();
	while (itr != clients_.end()) {
		Response	print;
		print << BOLDWHITE << "(" << itr->first << ") nick: " << itr->second.get_nick() << RESET;
		if (itr->second.IsAuth() == true) print << GREEN << " is Authenticated\n";
		else print << RED << " is not Authenticated\n";
		print << RESET;
		log::cout << print.get_str();
		itr++;
	}
}

void	Server::print_channels(void) {
	Response	logging;

	logging << BOLDWHITE << "_________CHANNEL INFO________\n";
	std::map<std::string, Channel>::iterator	iter = channels_.begin();
	while (iter != channels_.end()) {
		logging << "NAME: " << MAGENTA << iter->first << RESET << "\n";
		logging << BOLDWHITE << "\tMODE: " << RESET;
		char mode = iter->second.get_mode();
		{
			if (mode & MODE_LIMIT) logging << "+l ";
			else logging << "-l ";

			if (mode & MODE_INVITE) logging << "+i ";
			else logging << "-i ";

			if (mode & MODE_TOPIC) logging << "+t ";
			else logging << "-t ";

			if (mode & MODE_KEY) logging << "+k ";
			else logging << "-k ";
		}
		logging << "\n";
		if (mode & MODE_TOPIC) logging << BOLDWHITE << "\tTOPIC: " << RESET << iter->second.get_topic() << "\n";
		if (mode & MODE_KEY) logging << BOLDWHITE << "\tKEY: " << RESET <<iter->second.get_password() << "\n";
		logging << BOLDWHITE << "\tSIZE: " << RESET;
		std::set<int>	mem = iter->second.get_members();
		size_t	s = mem.size();
		logging << RESET << (int)s << "\n" << RESET;
		std::set<int>::iterator itr = mem.begin();
		for (size_t i = 0; i < s; ++i) {
			logging << BOLDMAGENTA << "\tMEM." << i << ": " << RESET <<  *itr << "\n";
			itr++;
		}
		iter++;
	}
	logging << RED << "_____________________________\n" << RESET;
	log::cout << logging.get_str();
}

