#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#define DEFAULT_THREAD_NUM 1
#define MAX_BUFFER_SIZE 1024
#define PORT (9090)

using namespace std;
vector<chrono::milliseconds::rep> times;
vector<std::string>	msg_list;
vector<pthread_t*> thread_list;
vector<int>	socket_list;

struct sockaddr_in server_addr;
pthread_mutex_t	mutex_time;
pthread_mutex_t	mutex_socket;

void *client_routine(void *);
//void getifaddress(void);
void error_exit(const char *message);

int main(int argc, char **argv) {
	if (pthread_mutex_init(&mutex_time, NULL) != 0)
		error_exit("pthread_mutex_init error");

	int	total_thread_num;
	if (argc < 2)
		total_thread_num = DEFAULT_THREAD_NUM;
	else
		total_thread_num = atoi(argv[1]);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 서버 IP 주소
    server_addr.sin_port = htons(PORT);

	msg_list.resize(total_thread_num, "");
	std::string msg = "PASS 123\r\n";
	std::string	nick = argv[2];
	for (int i = 0; i < total_thread_num; ++i) {
		msg_list[i] = msg + "NICK ";
		nick += to_string(i);
		nick += "\r\n";
		msg_list[i] += nick;
		nick = argv[2];
	}

	for (int i = 0; i < total_thread_num; ++i) {
		pthread_t	*tid = (pthread_t *) malloc(sizeof(pthread_t));
		pthread_create(tid, NULL, client_routine, (void *)(msg_list[i].c_str()));
		thread_list.push_back(tid);
	}

	for (int i = 0; i < thread_list.size(); ++i)
		pthread_join(*(thread_list[i]), NULL);

	std::cout << "time\n";
	for (int i = 0; i < times.size(); ++i) {
		std::cout << times[i] << "\n";
	}

//	msg = "QUIT\r\n";
//	for (int & socket_fd : socket_list) {
//		send(socket_fd, msg.c_str(), msg.size(), 0);
//	}
	return 0;
}

void *client_routine(void * arg) {
	char *message = (char *)arg;
	std::string	msg = message;
	msg += "USER a a ";
	msg += "10.13.1.6 test\r\n";


	int client_socket;
    char buffer[MAX_BUFFER_SIZE];

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error_exit("socket() error");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        error_exit("connect() error");
	
	int reuse = 1;
	setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	auto start = std::chrono::system_clock::now();
	// send arg to server and check the response time
	{
		int byte;
		send(client_socket, msg.c_str(), msg.size(), 0);
		byte = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
	//	buffer[byte] = '\0';
	//	cout << buffer;
	
		msg = "JOIN #TEST\r\n";
		send(client_socket, msg.c_str(), msg.size(), 0);
	//	byte = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
	//	buffer[byte] = '\0';
	//	cout << buffer;
		//msg = "QUIT\r\n";
		//send(client_socket, msg.c_str(), msg.size(), 0);
	}

	auto end = std::chrono::system_clock::now();
	auto elapsedtime = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	pthread_mutex_lock(&mutex_time);
    times.push_back(elapsedtime);
	pthread_mutex_unlock(&mutex_time);

	pthread_mutex_lock(&mutex_socket);
	socket_list.push_back(client_socket);
	pthread_mutex_unlock(&mutex_socket);

	return NULL;
}

//struct ifaddress * ifaddress(void) {
//	struct ifaddrs * addrs, * ifloop; 
//	char buf[64];
//	struct sockaddr_in *s4;
//	const char * en0 = "en0";
//	getifaddrs(&addrs);
//	for (ifloop = addrs; ifloop != NULL; ifloop = ifloop->ifa_next)
//	{
//		s4 = (struct sockaddr_in *)(ifloop->ifa_addr);
//		inet_ntop(ifloop->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf)) == NULL;
//		const char * ifname = ifloop->ifa_name;
//		if (strncmp(ifname, en0, 2) == 1)
//		{
//			printf("%s: %s\n", ifloop->ifa_name, buf);
//		}
//	}
//	return ifaddrs;
//}
void error_exit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}
