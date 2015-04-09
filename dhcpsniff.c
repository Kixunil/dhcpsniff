#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

volatile int running;

int main(int argc, char **argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s COMMAND\n", argv[0]);
		return 2;
	}

	char cmdbuf[strlen(argv[1]) + 19];

	int sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	if(sfd < 0) {
		perror("socket");
		return 1;
	}

	running = 1;
	while(running) {
		unsigned char buf[2048];
		ssize_t len = recvfrom(sfd, buf, 2048, 0, NULL, NULL);
		if(len < 0) {
			perror("recvfrom");
			break;
		}

		// Check length
		if(len < 40) continue;

		// Check whether destination is broadcast address
		int notbcast = 0, i;
		for(i = 0; i < 6; ++i) if(buf[i] != 0xff) { notbcast = 1; break; }
		if(notbcast) continue;
		//fprintf(stderr, "Broadcast address OK\n");

		// Check whether it's IP packet
		if(buf[12] != 0x08 || buf[13] != 0x00) continue;
		//fprintf(stderr, "It's IP packet\n");

		// Check whether it's IPv4
		if((buf[14] >> 4) != 0x04) continue;
		//fprintf(stderr, "IPv4\n");

		// Check whether protocol is UDP
		if(buf[23] != 17) continue;
		//fprintf(stderr, "UDP\n");

		// Check IP addresses
		if(*(uint32_t *)(buf + 26) != 0x00000000) continue;
		if(*(uint32_t *)(buf + 30) != 0xffffffff) continue;
		//fprintf(stderr, "IP addresses match\n");

		// Check UDP ports
		if(buf[34] != 0x00 || buf[35] != 0x44) continue;
		if(buf[36] != 0x00 || buf[37] != 0x43) continue;
		//fprintf(stderr, "UDP ports match\n");

		// Check opcode
		if(buf[38] != 0x01) continue;
		//fprintf(stderr, "Opcode OK\n");

#ifdef VERBOSE
		printf("Received packet from: %02x:%02x:%02x:%02x:%02x:%02x to %02x:%02x:%02x:%02x:%02x:%02x\n", (unsigned int)buf[6], (unsigned int)buf[7], (unsigned int)buf[8], (unsigned int)buf[9], (unsigned int)buf[10], (unsigned int)buf[11], (unsigned int)buf[0], (unsigned int)buf[1], (unsigned int)buf[2], (unsigned int)buf[3], (unsigned int)buf[4], (unsigned int)buf[5]);
		printf("Content:\n");
		for(i = 12; i < len; ++i) printf("%02x ", (unsigned int)buf[i]);
		printf("\n");
#endif

		sprintf(cmdbuf, "%s %02x:%02x:%02x:%02x:%02x:%02x", argv[1], (unsigned int)buf[6], (unsigned int)buf[7], (unsigned int)buf[8], (unsigned int)buf[9], (unsigned int)buf[10], (unsigned int)buf[11]);

		system(cmdbuf);
	}

	close(sfd);
	return 0;
}
