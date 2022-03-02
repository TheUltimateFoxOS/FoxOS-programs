#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/net.h>


int main(int argc, char* argv[], char* envp[]) {
	int num_ping = 4;
	char* domain = NULL;
	int nic_id = 0;

	int idx = 1;
	while (idx < argc) {
		if (strcmp(argv[idx], "-n") == 0) {
			if (idx + 1 < argc) {
				num_ping = atoi(argv[idx + 1]);
				idx++;
			} else {
				printf("Error: -n requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-i") == 0) {
			if (idx + 1 < argc) {
				nic_id = atoi(argv[idx + 1]);
				idx++;
			} else {
				printf("Error: -i requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-n <num_ping>] [-i <nic_id>] <domain>\n", argv[0]);
			exit(0);
		} else {
			if (domain == NULL) {
				domain = argv[idx];
			}
		}

		idx++;
	}

	if (domain == NULL) {
		printf("Error: No domain specified. See %s -h\n", argv[0]);
		abort();
	}

	union ip_u ip = dns_resolve(domain, nic_id);
	if (ip.ip == 0) {
		printf("Error: Could not resolve %s\n", domain);
		abort();
	}

	for (int i = 0; i < num_ping; i++) {
		//Need to sleep for a bit to prevent flooding the network
		printf("[%d.%d.%d.%d] %d / %d: %s\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3], i + 1, num_ping, icmp_echo_request(ip.ip, nic_id) ? "got response" : "no response");
	}

    return 0;
}
