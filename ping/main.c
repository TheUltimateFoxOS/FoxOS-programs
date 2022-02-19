#include <stdio.h>
#include <stdlib.h>

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
				printf("-n requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-i") == 0) {
			if (idx + 1 < argc) {
				nic_id = atoi(argv[idx + 1]);
				idx++;
			} else {
				printf("-i requires an argument\n");
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
		printf("no domain specified\n");
		abort();
	}

	union ip_u ip = dns_resolve(domain, nic_id);

	for (int i = 0; i < num_ping; i++) {
		printf("[%d.%d.%d.%d] %d / %d: %s\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3], i + 1, num_ping, icmp_echo_request(ip.ip, nic_id) ? "got response" : "no response");
	}

    return 0;
}
