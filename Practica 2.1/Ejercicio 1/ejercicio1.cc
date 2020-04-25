#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *res;


	memset((void*)&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;


	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (rc != 0) {	std::cout << gai_strerror(rc); return -1; }


	for (struct addrinfo* it = res; it != 0; it = it->ai_next)
	{
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		getnameinfo(it->ai_addr, it->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

		std::cout << "IP: " << host << " Familia: " << it->ai_family << " Tipo de socket: " << it->ai_socktype << '\n';
	}

	freeaddrinfo(res);
	return 0;
}