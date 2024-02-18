#include "Server.hpp"

Server::~Server() {
	delete pool_;
	delete_map<int, Client*>(this->clients_);
	delete_map<std::string, Channel*>(this->channels_);
}

bool	Server::IsValidPort(char *argv) {
	size_t	index = 0;
	size_t	port_number;
	
	while (argv[index]) {
		if (argv[index] >= '0' && argv[index] <= '9')
			++index;
		else
			return false;
	}
	if (index > 9)
		return false;

	port_number = atoi(argv);
	if (port_number < 1024 || port_number > 65535)
		return false;
	return true;
}

void	Server::AreValidArgs(int argc, char **argv) {
	std::string error_message;
	std::string program_name = argv[0];

	if (argc != 3) {
		error_message = "Usage: " + program_name + " <port> <password>\n";
		error_handling(error_message);
	}

	if (IsValidPort(argv[1]) == false) {
		std::string	port_number = argv[1];
		error_message = "Error: Port number " + port_number + " is invalid\n";
		error_handling(error_message);
	}
}

Server::Server(int argc, char **argv) {
	if (argc != 3) {
		std::string error_message;
		std::string program_name = argv[0];
		error_message = "Usage: " + program_name + " <port> <password>\n";
		error_handling(error_message);
	}
	AreValidArgs(argc, argv);
	ServerSocketInit(argv);
	MutexInit();
	KqueueInit();
	SignalInit();
	p_server_info();
}

void	Server::Run(void) {
	int nev;
	while (true) {
		try {
			nev = kevent(kq_, &(chlist_[0]), chlist_.size(), evlist_, FT_KQ_EVENT_SIZE, &timeout_);
			chlist_.clear();
			if (nev == -1)
				error_handling("kevent() error\n");
			else if (nev > 0)
				HandleEvents(nev);
			DeleteInvalidClient();
		} catch (std::exception& e) {
			log::cout << BOLDRED << e.what() << "\n";
		}
	}
}

void	Server::MutexInit(void) {
	this->list_mutex_.init(NULL);
	this->del_clients_mutex_.init(NULL);
	this->clients_mutex_.init(NULL);
	this->channels_mutex_.init(NULL);
}

void	Server::ServerSocketInit(char **argv) {
	name_ = FT_SERVER_NAME;
	pool_ = new ThreadPool(FT_THREAD_POOL_SIZE);
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

void	Server::SignalInit(void) {
	this->act.sa_handler = HandleSIGPIPE;
	sigemptyset(&this->act.sa_mask);
	this->act.sa_flags = 0;
	sigaction(SIGPIPE, &act, 0);
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

std::map<std::string, Channel*>& Server::get_channels(void) {
	return this->channels_;
}

Channel*	Server::get_channel_ptr(const std::string& name) {
	Channel *ret = NULL;

	LockChannelListMutex();
	std::map<std::string, Channel*>::iterator	itr = this->channels_.find(name);
	if (itr != this->channels_.end())
		ret = itr->second;
	UnlockChannelListMutex();
	return ret;
}

//need Fix
std::string	Server::SearchClientBySock(const int& sock) {
	std::string	nick;

	LockClientListMutex();
	std::map<int, Client*>::iterator	itr = this->clients_.find(sock);
	if (itr != this->clients_.end())
		nick = (itr->second)->get_nick();
	UnlockClientListMutex();
	return nick;
}

int	Server::SearchClientByNick(const std::string& nick) {
	int	ret = FT_INIT_CLIENT_FD;
	std::string	tmp_nick;

	LockClientListMutex();
	std::map<int, Client*>::iterator	iter = this->clients_.begin();
	while (iter != this->clients_.end()) {
		LockClientMutex(iter->first);
		tmp_nick = (iter->second)->get_nick();
		UnlockClientMutex(iter->first);
		if (nick.compare(tmp_nick) == 0) {
			ret = iter->first;
			break ;
		}
		iter++;
	}
	UnlockClientListMutex();
	return ret;
}

bool	Server::SearchChannelByName(const std::string& name) {
	bool	search_ret = false;
	this->channels_mutex_.lock();
	std::map<std::string, Channel*>::iterator	itr = this->channels_.find(name);
	if (itr != this->channels_.end())
		search_ret = true;
	this->channels_mutex_.unlock();
	return search_ret;
}

//Mutex done
bool	Server::AddClientMutex(const int& sock) {
	Mutex *mutex_ptr = new Mutex();
	int init_result = mutex_ptr->init(NULL);
	if (init_result != 0) {
		log::cout << "AddClientMutex fail by mutex init fail\n";
		delete mutex_ptr;
		return false;
	}

	this->list_mutex_.lock();//lock
	std::map<int, Mutex*>::iterator	mutex_it = client_mutex_list_.find(sock);
	if (mutex_it != client_mutex_list_.end()) {
		this->list_mutex_.unlock();//unlock
		log::cout << "AddClientMutex fail by already exist socket\n";
		return false;
	}

	client_mutex_list_.insert(std::make_pair(sock, mutex_ptr));
	this->list_mutex_.unlock();//unlock
	return true;
}

//Mutex done
bool	Server::AddChannelMutex(const std::string& name) {
	Mutex *mutex_ptr = new Mutex();
	int init_result = mutex_ptr->init(NULL);
	if (init_result != 0) {
		delete mutex_ptr;
		return false;
	}

	this->list_mutex_.lock();//lock
	std::map<std::string, Mutex*>::iterator	mutex_it = channel_mutex_list_.find(name);
	if (mutex_it != channel_mutex_list_.end()) {
		this->list_mutex_.unlock();//unlock
		return false;
	}
	channel_mutex_list_.insert(std::make_pair(name, mutex_ptr));
	this->list_mutex_.unlock();//unlock
	return true;
}

//Mutex done
bool	Server::DeleteClientMutex(const int& sock) {
	Mutex *del_ptr;

	this->list_mutex_.lock();//lock
	std::map<int, Mutex*>::iterator	mutex_it = client_mutex_list_.find(sock);
	if (mutex_it == client_mutex_list_.end()) {
		this->list_mutex_.unlock();//unlock
		return false;
	}
	del_ptr = mutex_it->second;
	client_mutex_list_.erase(mutex_it);
	this->list_mutex_.unlock();//unlock
	delete del_ptr;
	return true;
}

//Mutex done
bool	Server::DeleteChannelMutex(const std::string& name) {
	Mutex *del_ptr = NULL;

	this->list_mutex_.lock();//lock
	std::map<std::string, Mutex*>::iterator	mutex_it = channel_mutex_list_.find(name);
	if (mutex_it != channel_mutex_list_.end()) {
		del_ptr = mutex_it->second;
		channel_mutex_list_.erase(mutex_it);
		delete del_ptr;
	}
	this->list_mutex_.unlock();//unlock
	if (del_ptr == NULL)
		return false;
	return true;
}

//Mutex done
bool	Server::LockClientMutex(const int& sock) {
	this->list_mutex_.lock();//lock
	std::map<int, Mutex*>::iterator	mutex_it = client_mutex_list_.find(sock);
	if (mutex_it == client_mutex_list_.end()) {
		this->list_mutex_.unlock();//unlock
		return false;
	}
	this->list_mutex_.unlock();//unlock
	if (mutex_it->second->lock() == 0) {
		return true;
	}
	return false;
}

bool	Server::LockClientListMutex(void) {
	return (this->clients_mutex_.lock());
}

void	Server::UnlockClientListMutex(void) {
	this->clients_mutex_.unlock();
}

bool	Server::LockChannelListMutex(void) {
	return (this->channels_mutex_.lock());
}

void	Server::UnlockChannelListMutex(void) {
	this->channels_mutex_.unlock();
}

//Mutex done
bool	Server::LockChannelMutex(const std::string& name) {
	this->list_mutex_.lock();//lock

	std::map<std::string, Mutex*>::iterator	mutex_it = channel_mutex_list_.find(name);
	std::map<std::string, Mutex*>::iterator	end = channel_mutex_list_.end();
	
	this->list_mutex_.unlock();//unlock

	if (mutex_it != end) {
		mutex_it->second->lock();
		return true;
	}
	return false;
}

//Mutex done
void	Server::UnlockClientMutex(const int& sock) {
	this->list_mutex_.lock();//lock

	std::map<int, Mutex*>::iterator	mutex_it = client_mutex_list_.find(sock);
	std::map<int, Mutex*>::iterator	end = client_mutex_list_.end();

	this->list_mutex_.unlock();//unlock

	if (mutex_it != end) 
		mutex_it->second->unlock();
}

//Mutex done
void	Server::UnlockChannelMutex(const std::string& name) {
	this->list_mutex_.lock();//lock
	std::map<std::string, Mutex*>::iterator	mutex_it = channel_mutex_list_.find(name);
	std::map<std::string, Mutex*>::iterator end_it = channel_mutex_list_.end();
	this->list_mutex_.unlock();//unlock

	if (mutex_it != end_it)
		(mutex_it->second)->unlock();
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
	}
}

void	Server::HandleClientEvent(struct kevent event) {
	this->clients_mutex_.lock();//lock
	Client	*client = clients_[event.ident];
	this->clients_mutex_.unlock();//lock
	
	if (LockClientMutex(event.ident) == false) {//lock
		log::cout << "HandleClientEvent() error\n";
		UnlockClientMutex(event.ident);
		return ;
	}

	char	buff[FT_BUFF_SIZE];
	std::string& buffer = buffers_[client->get_sock()];
	int read_byte = read(event.ident, buff, sizeof(buff));
	if (read_byte == -1) {
		UnlockClientMutex(event.ident);//unlock
		log::cout << "client read error\n";
	}
	else if (read_byte == 0) {
		UnlockClientMutex(event.ident);//unlock
		DisconnectClient(event.ident);
	}
	else {
		buff[read_byte] = '\0';
		buffer += buff;
		std::vector<Command *> *cmds;
		int	offset;
		cmds = Request::ParseRequest(this, client, buffer, &offset);
		for (size_t i = 0; i < cmds->size(); ++i) 
			pool_->Enqueue((*cmds)[i]);
		UnlockClientMutex(event.ident);//unlock
		delete cmds;
		buffer.erase(0, offset);
	}
}

void	Server::ConnectClient(void) {
	Client			*new_client = NULL;
	ClientNetInfo	info;

	info = AcceptClient();
	new_client = new Client(info.sock);
	
	if (info.sock == FT_INIT_CLIENT_FD || AddClient(new_client) == false) {
		delete new_client;
		return;
	}
	AddEvent(new_client->get_sock(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

ClientNetInfo	Server::AcceptClient(void) {
	ClientNetInfo	info;

	info.sock = FT_INIT_CLIENT_FD;
	info.sock = accept(this->sock_, (struct sockaddr*)&info.addr, &info.addr_size);
	if (info.sock == -1)
		error_handling("accept() error\n");
	return info;
}

bool	Server::AddClient(Client *client) {
	this->clients_mutex_.lock();//lock
	this->clients_.insert(std::make_pair(client->get_sock(), client));
	this->clients_mutex_.unlock();//unlock
	buffers_[client->get_sock()] = "";
	if (AddClientMutex(client->get_sock()) == false)
		return false;
	return true;
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
	this->del_clients_mutex_.lock();
	del_clients_.insert(sock);
	this->del_clients_mutex_.unlock();
}

bool	Server::AddChannel(Channel *channel) {
	LockChannelListMutex();
	std::map<std::string, Channel*>::iterator itr = this->channels_.find(channel->get_name());
	if (itr == this->channels_.end())
		this->channels_.insert(make_pair(channel->get_name(), channel));
	UnlockChannelListMutex();

	if (AddChannelMutex(channel->get_name()) == false) {
		DeleteChannel(channel->get_name());
		return false;
	}
	return true;
}

void	Server::CreateChannel(const channel_info& info) {
	Channel *channel_ptr = new Channel(info.name);
	if (channel_ptr == NULL)
		return;

	if (info.mode & MODE_KEY)
		channel_ptr->set_mode(MODE_KEY, true);
	channel_ptr->set_key(info.key);
	channel_ptr->set_host(info.host);
	channel_ptr->set_host_sock(info.host_sock);
	
	if (AddChannel(channel_ptr) == false)
		delete channel_ptr;
}

void	Server::DeleteInvalidClient(void) {
	this->del_clients_mutex_.lock();
	std::set<int>::iterator	itr = this->del_clients_.begin();
	while (itr != this->del_clients_.end()) {
		DisconnectClient(*itr);
		itr++;
	}
	this->del_clients_.clear();
	this->del_clients_mutex_.unlock();
}

//free
void	Server::CeaseChannel(const std::string& channel_name) {
	Channel *channel_ptr = DeleteChannel(channel_name);
	if (channel_ptr != NULL)
		delete channel_ptr;
}

Channel*	Server::DeleteChannel(const std::string& channel_name) {
	std::map<std::string, Channel*>::iterator	ch_itr;
	Channel	*channel_ptr = NULL;

	this->channels_mutex_.lock();
	ch_itr = this->channels_.find(channel_name);
	if (ch_itr != this->channels_.end()) {
		LockChannelMutex(channel_name);
		channel_ptr = ch_itr->second;
		this->channels_.erase(ch_itr);
		UnlockChannelMutex(channel_name);
	}
	this->channels_mutex_.unlock();

	if (channel_ptr != NULL)
		DeleteChannelMutex(channel_name);

	return channel_ptr;
}

void	Server::DisconnectClient(const int& sock) {
	DeleteClientEvent(sock);
	Client *client_ptr = DeleteClient(sock);
	if (client_ptr != NULL) {
		DeleteClientInChannel(sock, client_ptr);
		delete client_ptr;
	}
}

Client*	Server::DeleteClient(const int& sock) {
	std::map<int, Client*>::iterator	client_it;
	Client *client_ptr = NULL;

	this->clients_mutex_.lock();//lock
	client_it = clients_.find(sock);
	if (client_it != clients_.end())
		client_ptr = client_it->second;
	this->clients_mutex_.unlock();//unlock
	if (client_ptr == NULL)
		return NULL;
	
	this->clients_mutex_.lock();
	this->clients_.erase(client_it);
	this->clients_mutex_.unlock();

	DeleteClientMutex(sock);
	return client_ptr;
}

void	Server::DeleteClientInChannel(const int& sock, Client *client)
{
	LockClientMutex(sock);
	std::vector<std::string> channels_of_client = client->get_channels();
	
	std::vector<std::string>::iterator channel_itr = channels_of_client.begin();
	int	left_client_num = 0;
	while (channel_itr != channels_of_client.end()) {
		if (LockChannelMutex(*channel_itr))
			left_client_num = this->channels_[*channel_itr]->Kick(sock);
		UnlockChannelMutex(*channel_itr);//unlock

		if (left_client_num < 1)
			CeaseChannel(*channel_itr);
		channel_itr++;
	}
	UnlockClientMutex(sock);
}

void	Server::DeleteClientEvent(const int& sock){
	struct kevent del_evlist[2];

	LockClientMutex(sock);
	EV_SET(&del_evlist[0], sock, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	kevent(this->kq_, del_evlist, 2, NULL, 0, NULL);
	close(sock);
	UnlockClientMutex(sock);
}

void	Server::p_server_info(void) {
	log::cout << "IRC server:" << GREEN << this->name_ << RESET << " started at ";
	log::cout << GREEN << inet_ntoa(this->addr_.sin_addr) << RESET;
	log::cout << " :: " << GREEN << this->port_ << RESET << "\n";
}
