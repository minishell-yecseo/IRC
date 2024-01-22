#include "Server.hpp"

Server::~Server() {
	delete pool_;
}

Server::Server(int argc, char **argv) {
	if (argc != 3) {
		std::string error_message;
		std::string program_name = argv[0];
		error_message = "Usage: " + program_name + " <port> <password>\n";
		error_handling(error_message);
	}
	name_ = FT_SERVER_NAME;
	pool_ = new ThreadPool(FT_THREAD_POOL_SIZE);
	port_ = atoi(argv[1]);
	password_ = argv[2];
	pthread_mutex_init(&(this->del_clients_mutex_), NULL);
	ServerSocketInit();
	KqueueInit();

	/* Psuedo Client for NickCommand test */
	int fd = open("test.txt", O_RDWR | O_CREAT, 0666);
	log::cout << BOLDGREEN << fd << "\n";
	Client p_client;
	p_client.set_sock(fd);
	p_client.set_nick("saseo");
	p_client.SetAuthFlag(FT_AUTH_PASS | FT_AUTH_NICK | FT_AUTH_USER);
	clients_.insert(std::make_pair(fd, p_client));
}

const std::string&	Server::get_name(void) {
	return this->name_;
}

const int& Server::get_port(void) {
	return this->port_;
}

const struct sockaddr_in&	Server::get_addr(void) {
	return this->addr_;
}

int	Server::SearchClientByNick(const std::string& nick) {
	int	ret = FT_INIT_CLIENT_FD;

	pthread_mutex_lock(&(this->pool_->s_clients_mutex_));

	std::map<int, Client>::iterator	iter = this->clients_.begin();
	int i = 0;
	while (iter != this->clients_.end()) {
		log::cout << MAGENTA << "\tSEARCH_" << i << " " << (iter->second).get_nick() << "\n" << RESET;
		std::string temp_nick = (iter->second).get_nick();
		if (nick.compare(temp_nick) == 0) {
			ret = iter->first;
			log::cout << RED << "NICK:" << nick << " foudn in Server\n" << RESET;
			break ;
		}
		iter++;
		i++;
	}

	pthread_mutex_unlock(&(this->pool_->s_clients_mutex_));
	log::cout << BOLDGREEN << "SearchClientByNick " << nick << " : " << ret << "\n" << RESET;
	return ret;
}

bool	Server::SearchChannelByName(const std::string& name) {
	bool	search_ret = false;
	pthread_mutex_lock(&this->pool_->s_channels_mutex_);
	std::map<std::string, Channel>::iterator	itr = this->channels_.find(name);
	if (itr != this->channels_.end())
		search_ret = true;
	pthread_mutex_unlock(&this->pool_->s_channels_mutex_);
	return search_ret;
}

bool	Server::LockClientMutex(const int& sock) {
	return (this->pool_->LockClientMutex(sock));
}

bool	Server::LockChannelMutex(const std::string& name) {
	return (this->pool_->LockChannelMutex(name));
}

void	Server::UnlockClientMutex(const int& sock) {
	this->pool_->UnlockClientMutex(sock);
}

void	Server::UnlockChannelMutex(const std::string& name) {
	this->pool_->UnlockChannelMutex(name);
}

void	Server::ServerSocketInit(void) {
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

bool	Server::Run(void) {
	// main loop of ircserv with kqueue
	int nev;
	while (true) {
		nev = kevent(kq_, &(chlist_[0]), chlist_.size(), evlist_, FT_KQ_EVENT_SIZE, &timeout_);
		chlist_.clear();//why should I write this line?
		if (nev == -1)
			error_handling("kevent() error\n");
		else if (nev == 0)
			HandleTimeout();
		else if (nev > 0)
			HandleEvents(nev);
		DeleteInvalidClient();
		/* clients list print */
		pthread_mutex_lock(&pool_->s_clients_mutex_);
		std::map<int, Client>::iterator itr = clients_.begin();
		while (itr != clients_.end()) {
			Response	print;
			pool_->LockClientMutex(itr->first);
			print << BOLDWHITE << itr->first << ") nick: " << itr->second.get_nick() << RESET;
			if (itr->second.IsAuth() == true)
				print << GREEN << " is Authenticated\n";
			else
				print << RED << " is not Authenticated\n";
			print << RESET;
			log::cout << print.get_str();
			pool_->UnlockClientMutex(itr->first);
			itr++;
		}
		pthread_mutex_unlock(&pool_->s_clients_mutex_);
	}
	return true;
}

void	Server::HandleEvents(int nev) {
	struct kevent	event;
	for (int i = 0; i < nev; ++i) {
		event = evlist_[i];
		print_event(&event, i);
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
	if (pool_->LockClientMutex(event.ident) == false) {//lock
		log::cout << "HandleClientEvent() error\n";
		return ;
	}

	//Client client = clients_[event.ident];
	Client	*client = &(clients_[event.ident]);
	char	buff[FT_BUFF_SIZE];
	std::string& buffer = buffers_[client->get_sock()];
	int read_byte = read(event.ident, buff, sizeof(buff));
	if (read_byte == -1) {
		pool_->UnlockClientMutex(event.ident);//unlock
		
		log::cout << "client read error\n";
	}
	else if (read_byte == 0) {
		pool_->UnlockClientMutex(event.ident);//unlock
		DisconnectClient(event.ident);
	}
	else {
		buff[read_byte] = '\0';
		buffer += buff;
		std::vector<Command *> cmds;
		int	offset;
		cmds = Request::ParseRequest(this, client, buffer, &offset);
		for (size_t i = 0; i < cmds.size(); ++i) {
			
			log::cout << "index : " << i << "\n";
			pool_->Enqueue(cmds[i]);
		}
		pool_->UnlockClientMutex(event.ident);//unlock
		buffer.erase(0, offset);
	
		//	if (client.auth_)
		//	{
		//		/* Authorized Clients event handle */
		//	} else {
		//		/* Unauthorized Clients event handle */
		//	}
	}
}

void	Server::ConnectClient(void) {
	Client	client;
	if (client.set_sock(accept(sock_, (struct sockaddr*)&client.addr_, &client.addr_size_)) == -1)
		error_handling("accept() error\n");

	if (pool_->AddClientMutex(client.get_sock()) == false) {
		
		log::cout << BLUE << "client's pthread_mutex_init() fail\n" << RESET;
		return ;
	}

	/* handle new client */
	
	AddEvent(client.get_sock(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	log::cout << BOLDRED << "server->clients lock! in ConnectClient\n" << RESET;
	pthread_mutex_lock(&(pool_->s_clients_mutex_));//lock
	clients_[client.get_sock()] = client;
	pthread_mutex_unlock(&(pool_->s_clients_mutex_));//unlock
	log::cout << BOLDRED << "server->clients unlock! in ConnectClient\n" << RESET;
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
	{
		
		log::cout << "client socket event error\n";
		DisconnectClient(event.ident);//need to implement fucntion
	}
}

void	Server::AddDeleteClient(const int& sock) {
	pthread_mutex_lock(&this->del_clients_mutex_);
	del_clients_.insert(sock);
	pthread_mutex_unlock(&this->del_clients_mutex_);
}

void	Server::DeleteInvalidClient(void) {
	pthread_mutex_lock(&this->del_clients_mutex_);
	std::set<int>::iterator	itr = this->del_clients_.begin();
	while (itr != this->del_clients_.end()) {
		DisconnectClient(*itr);
		itr++;
	}
	this->del_clients_.clear();
	pthread_mutex_unlock(&this->del_clients_mutex_);
}

void	Server::DisconnectClient(const int& sock) {
	std::map<int, Client>::iterator	client_it;

	pthread_mutex_lock(&(pool_->s_clients_mutex_));//lock
	
	log::cout << BOLDRED << "server->clients lock! in DisconnectClient\n" << RESET;
	client_it = clients_.find(sock);
	if (client_it == clients_.end()) {
		log::cout << "DisconnectClient(" << sock << ") error\n";
		pthread_mutex_unlock(&(pool_->s_clients_mutex_));//unlock
		log::cout << BOLDRED << "server->clients unlock! in DisconnectClient (error)\n" << RESET;
		return;
	}

	if (pool_->LockClientMutex(client_it->second.get_sock()) == false) {
		
		log::cout << BOLDRED << "LockClientMutex(" << sock << ") fail\n";
		pthread_mutex_unlock(&(pool_->s_clients_mutex_));//unlock
		return;
	}

	clients_.erase(client_it);

	struct kevent del_evlist[2];
	EV_SET(&del_evlist[0], sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	EV_SET(&del_evlist[1], sock, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(this->kq_, del_evlist, 2, NULL, 0, NULL);
	close(sock);

	pool_->DeleteClientMutex(client_it->second.get_sock());

	pool_->UnlockClientMutex(client_it->second.get_sock());

	pthread_mutex_unlock(&(pool_->s_clients_mutex_));//unlock
	
	log::cout << BOLDRED << "server->clients unlock! in DisconnectClient\n" << RESET;

	/* Channel 에서 Client 삭제 */
	std::vector<std::string>::iterator	channel_itr = client_it->second.channels_.begin();
	while (channel_itr != client_it->second.channels_.end()) {
		if (pool_->LockChannelMutex(*channel_itr) == false) {//lock
			log::cout << CYAN << "LockChannelMutex error\n" << RESET;
			return;
		}
		this->channels_[*channel_itr].Kick(client_it->second);
		pool_->UnlockChannelMutex(*channel_itr);//unlock
	}

	log::cout << CYAN << "client " << client_it->second.get_sock() << " disconnected\n" << RESET;
}

void	Server::HandleTimeout(void) {
	/* handle timeout */
	
	log::cout << "time out!\n";
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
