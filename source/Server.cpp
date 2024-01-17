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
	ServerSocketInit();
	KqueueInit();
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
			DisconnectClient(event);//need to implement
		else if (event.filter == EVFILT_READ && event.ident == (uintptr_t)sock_)
			ConnectClient();
		else if (event.filter == EVFILT_READ)
			HandleClientEvent(event);
	}
}

void	Server::HandleClientEvent(struct kevent event) {
	std::map<int, Client>::iterator	it = clients_.find(event.ident);
	Client client;

	if (it != clients_.end()) {
		client = it->second;
		char buff[FT_BUFF_SIZE];
		int n = read(event.ident, buff, sizeof(buff));
		if (n == -1)
			std::cerr << "client read error\n";
		else if (n == 0)
			DisconnectClient(event);
		else {
			buff[n] = 0;
			client.buffer_ += buff;
			std::vector<Command *> cmds;
			int	offset;
			cmds = Request::ParseRequest(this, &client, client.buffer_, &offset);
			for (size_t i = 0; i < cmds.size(); ++i) {
				std::cout << "index : " << i << "\n";
				pool_->Enqueue(cmds[i]);
			}
		//	if (client.auth_)
		//	{
		//		/* Authorized Clients event handle */
		//	} else {
		//		/* Unauthorized Clients event handle */
		//	}
		}
	}
}

void	Server::ConnectClient(void) {
	Client	client;
	if (client.set_sock(accept(sock_, (struct sockaddr*)&client.addr_, &client.addr_size_)) == -1)
		error_handling("accept() error\n");

	/* handle new client */
	std::cout << "accent new client: " << client.sock_ << "\n";
	AddEvent(client.sock_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	clients_[client.sock_] = client;
}

bool	Server::AuthClient(Client& client) {
	// client가 보낸 메시지를 확인한다.
	//		1. client가 보낸 password 와 Server의 password 의 일치
	//		2. client가 보낸 nick이 기존 clients의 nick과 겹치지 않아야함.
	//	1, 2 조건을 만족하는 client에 한해서 참을 반환.
	std::cout << RED << "client authenticate call: " << client.sock_ << RESET << "\n";
	return true;
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
		std::cerr << "client socket event error\n";
		DisconnectClient(event);//need to implement fucntion
	}
}

void	Server::DisconnectClient(struct kevent event) {
	std::cout << RED << "client " << event.ident << " disconnected\n" << RESET;
	struct kevent delete_event;
	EV_SET(&delete_event, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	kevent(kq_, &delete_event, 1, NULL, 0, NULL);
	close(event.ident);
	clients_.erase(event.ident);
	/* channel.. clients... etc */
}

void	Server::HandleTimeout(void) {
	/* handle timeout */
	std::cout << "time out!\n";
}

/* wooseoki functions */
void	Server::print_event(struct kevent *event, int i) {
	std::cout << "=============================\n";
	std::cout << "index : " << i << "\n";
	std::cout << "ident : " << event->ident << "\n";
	p_event_filter(event);
	p_event_flags(event);
	std::cout << "=============================\n";
}

void	Server::p_event_filter(struct kevent *event) {
	std::cout << "filter : " << GREEN;
	if (event->filter == EVFILT_READ) std::cout << "EVFILT_READ";
	else if (event->filter == EVFILT_WRITE) std::cout << "EVFILT_WRITE";
	else if (event->filter == EVFILT_AIO) std::cout << "EVFILT_AIO";
	else if (event->filter == EVFILT_VNODE) std::cout << "EVFILT_VNODE";
	else if (event->filter == EVFILT_PROC) std::cout << "EVFILT_PROC";
	else if (event->filter == EVFILT_SIGNAL) std::cout << "EVFILT_SIGNAL";
	else if (event->filter == EVFILT_EXCEPT) std::cout << "EVFILT_EXCEPT";
	else std::cout << "unknown EVFILT";
	std::cout << "\n" << RESET;
}

void	Server::p_event_flags(struct kevent *event) {
	std::cout << "flags : " << GREEN;
	if (event->flags & EV_ADD) std::cout << "EV_ADD | ";
	if (event->flags & EV_DELETE) std::cout << "EV_DELETE | ";
	if (event->flags & EV_ENABLE) std::cout << "EV_ENABLE | ";
	if (event->flags & EV_DISABLE) std::cout << "EV_DISABLE | ";
	if (event->flags & EV_ONESHOT) std::cout << "EV_ONESHOT | ";
	if (event->flags & EV_CLEAR) std::cout << "EV_CLEAR | ";
	if (event->flags & EV_RECEIPT) std::cout << "EV_RECEIPT | ";
	if (event->flags & EV_OOBAND) std::cout << "EV_OOBAND | ";
	if (event->flags & EV_ERROR) std::cout << "EV_ERROR | ";
	if (event->flags & EV_EOF) std::cout << "EV_EOF | ";
	std::cout << RESET << "\n";
}
