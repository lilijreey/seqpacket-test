/*
 * Copyright (c) 2017, Sam Kumar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

const char msg[10296] = { 0xAF };

void fatal(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char** argv) {
    int server_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (server_socket == -1) {
        fatal("socket");
    }

    struct sockaddr_un bound_name;
    memset(&bound_name, 0x00, sizeof(bound_name));
    bound_name.sun_family = AF_UNIX;
    strncpy(&bound_name.sun_path[1], "seqpacket/test", sizeof(bound_name.sun_path) - 1);

    size_t total_size = total_size = sizeof(bound_name.sun_family) + 1 + strlen(&bound_name.sun_path[1]);
    if (bind(server_socket, (struct sockaddr*) &bound_name, total_size) == -1) {
        fatal("bind");
    }

    if (listen(server_socket, 0) == -1) {
        fatal("listen");
    }

    while (true) {
        int sock = accept(server_socket, NULL, NULL);
        if (sock == -1) {
            fatal("accept");
        }

        while (true) {
            ssize_t written = write(sock, msg, sizeof(msg));
            if (written != sizeof(msg)) {
                perror("write");
                break;
            }
        }

        close(sock);
    }
    return 0;
}
