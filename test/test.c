#include <stdio.h>
#include <stdlib.h>

#include <sys/net.h>


int main(int argc, char* argv[], char* envp[]) {

	union ip_u ip = dns_resolve("www.gsz-zak.de", 0);
	printf("%d.%d.%d.%d\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
	char* http_request = "GET /robots.txt HTTP/1.1\r\nHost: www.gsz-zak.de\r\n\r\n";

	int socket = connect(ip.ip, 80, TCP_SOCKET);
	send(socket, http_request, strlen(http_request));

	char* buffer = malloc(1024);
	int bytes_read = recv(socket, buffer, 1024);
	buffer[bytes_read] = 0;
	printf("%s\n", buffer);
	free(buffer);

	disconnect(socket);

    return 0;
}
