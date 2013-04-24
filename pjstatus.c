/*
 * pjstatus.c
 *
 * Get and set an HP printer's status, using PJL on JetDirect (port 9100).
 *
 * Copyright (c) 2013 Charles Lehner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// get a variable from str of the form \nname="..."
// write it to value
// return 1 on success, 0 on failure
int get_var(char *str, char *name, char *value) {
	char *start = str, *end = 0;
	int name_len = strlen(name);
	while (start) {
		if (!(start = strchr(start, '\n'))) return 0;
		if (!(start = strstr(start, name))) return 0;
		start += name_len;
		if (*(start++) == '=' && *(start++) == '"') break;
	}
	if (!start) return 0;
	if (!(end = strchr(start, '"'))) return 0;
	strncpy(value, start, end-start);
	value[end-start] = 0;
	return 1;
}

int main(int argc, char *argv[]) {
	int sockfd, n, i;
	int status;
	struct addrinfo hints, *res;

	char buffer[256];
	char *setstatus_cmd = "\e%%-12345X@PJL RDYMSG DISPLAY=\"%s\"\r\n\e%%-12345X";
	char *getstatus_cmd = "\e%-12345X@PJL INFO STATUS\r\n\e%-12345X";
	char line[128];

	if (argc < 2) {
		fprintf(stderr, "Usage: %s hostname [status...]\n", argv[0]);
		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // maybe the printer is on ipv6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(argv[1], "jetdirect", &hints, &res)) != 0) {
		    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
			    exit(1);
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0) {
		perror("socket");
		return 1;
	}

	if ((n = connect(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
		perror("connect");
		return 1;
	}

	freeaddrinfo(res);

	memset(line, 0, sizeof line);

	if (argc > 2) {
		// concatenate the rest of the arguments
		for (i = 2; i < argc; i++) {
			if (i > 2) strcat(line, " ");
			strcat(line, argv[i]);
		}
		// set printer status to argument
		memset(buffer, 0, sizeof buffer);
		sprintf(buffer, setstatus_cmd, line);
		send(sockfd, buffer, strlen(buffer), 0);

	} else {
		// get printer status
		send(sockfd, getstatus_cmd, strlen(getstatus_cmd), 0);

		// get output
		if ((n = recv(sockfd, buffer, 255, 0)) < 0) {
			perror("recv");
			return 1;
		}
		buffer[n] = 0;

		// print status lines
		if (get_var(buffer, "DISPLAY", line)) printf("%s\n", line);
		if (get_var(buffer, "DISPLAY2", line)) printf("%s\n", line);
	}

	close(sockfd);
	return 0;
}
