#include <stdio.h>
#include <stdlib.h>

#include <sys/net.h>


int main(int argc, char* argv[], char* envp[]) {

	union ip_u ip = dns_resolve("discord.com", 0);

	printf("ip: %d.%d.%d.%d\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
	printf("%s\n", icmp_echo_request(ip.ip, 0) ? "got response" : "no response");
    return 0;
}
