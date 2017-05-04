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

char buf[10296];

void fatal(const char* msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char** argv) {
    int sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (sock == -1) {
        fatal("socket");
    }

    struct sockaddr_un target_name;
    memset(&target_name, 0x00, sizeof(target_name));
    target_name.sun_family = AF_UNIX;
    strncpy(&target_name.sun_path[1], "seqpacket/test", sizeof(target_name.sun_path) - 1);

    size_t total_size = total_size = sizeof(target_name.sun_family) + 1 + strlen(&target_name.sun_path[1]);
    if (connect(sock, (struct sockaddr*) &target_name, total_size) == -1) {
        fatal("connect");
    }

    while (true) {
        ssize_t consumed = read(sock, &buf, sizeof(buf));
        if (consumed == 0) {
            printf("read: end of file\n");
        } else if (consumed == -1) {
            fatal("read");
        } else if (consumed != sizeof(buf)) {
            printf("Got unexpectedly small message: %zd bytes\n", consumed);
            return 2;
        }
    }

    return 0;
}
