#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int PORT = 9090;

void	DummyClient(int i) {
	// Child process (client)
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1) {
        perror("Socket creation failed");
        return ;
    }
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        perror("Connection failed");
        close(clientSocket);
        return ;
    }
	sleep(1);
	std::string	index = std::to_string(i);
    std::string message = "CAP LS\r\n";
	message += "PASS 1234\r\n";
	message += "NICK number" + index + "\r\n";
	message += "USER number" + index + " number" + index + " localhost :Wooseok KIM\r\n";
	message += "PING Happy\r\n";
	message += "CAP END\r\n";
    ssize_t bytesSent = send(clientSocket, message.c_str(), message.size(), 0);

	/*
	sleep(2);
	message = "";
	message += "JOIN DummyChannel\r\n";
	message += ":number" + index +" privmsg #DummyChannel :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel :It's dummy chat\r\n";
    bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
	
	sleep(2);
	std::string next_index = std::to_string(i + 1);
	message = "";

	message += "invite number" + next_index + " #DummyChannel\r\n";
	message += "JOIN DummyChannel2\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += ":number" + index +" privmsg #DummyChannel2 :It's dummy chat\r\n";
	message += "PART #DummyChannel\r\n";

    bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
*/
	if (bytesSent == -1) {
        perror("Sending data failed");
        return ;
    }

    close(clientSocket);
    return ;

}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <number_of_clients>" << std::endl;
		return 1;
	}

	int num_clients = std::stoi(argv[1]);

	for (int i = 0; i < num_clients; ++i) {
		pid_t pid = fork();

		if (pid == -1) {
			perror("Fork failed");
			return 1;
		} else if (pid == 0) {
			DummyClient(i);
		}
	}
	for (int i = 0; i < num_clients; ++i) {
		int status;
		wait(&status);
	}

	return 0;
}
