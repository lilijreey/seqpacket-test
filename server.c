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

/*
 * Taken from Wikipedia (https://en.wikipedia.org/wiki/Unix_domain_socket) on May 4, 2017.
 * This text is licensed under CC BY-SA (http://creativecommons.org/licenses/by-sa/3.0/).
 */
const char* title = "Unix domain socket";
const char* paragraph1 = "A Unix domain socket or IPC socket (inter-process communication socket) is a data communications endpoint for exchanging data between processes executing on the same host operating system. Like named pipes, Unix domain sockets support transmission of a reliable stream of bytes (SOCK_STREAM, compare to TCP). In addition, they support ordered and reliable transmission of datagrams (SOCK_SEQPACKET), or unordered and unreliable transmission of datagrams (SOCK_DGRAM, compare to UDP). The Unix domain socket facility is a standard component of POSIX operating systems.";
const char* paragraph2 = "The API for Unix domain sockets is similar to that of an Internet socket, but rather than using an underlying network protocol, all communication occurs entirely within the operating system kernel. Unix domain sockets use the file system as their address name space. Processes reference Unix domain sockets as file system inodes, so two processes can communicate by opening the same socket.";
const char* paragraph3 = "In addition to sending data, processes may send file descriptors across a Unix domain socket connection using the sendmsg() and recvmsg() system calls. This allows the sending processes to grant the receiving process access to a file descriptor for which the receiving process otherwise does not have access. This can be used to implement a rudimentary form of capability-based security. For example, this allows the Clam AntiVirus scanner to run as an unprivileged daemon on Linux and BSD, yet still read any file sent to the daemon's Unix domain socket.";

void fatal(const char* msg) {
    perror(msg);
    exit(1);
}

void checked_send(int sockfd, const void* buf, size_t len, int flags) {
    ssize_t rv = send(sockfd, buf, len, flags);
    if (rv == -1) {
        fatal("send");
    } else if (rv != len) {
        fprintf(stderr, "send: full message not sent (%zd bytes out of %zu bytes)\n", rv, len);
        exit(1);
    }
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

        checked_send(sock, title, strlen(title), 0);
        checked_send(sock, NULL, 0, 0);
        checked_send(sock, paragraph1, strlen(paragraph1), 0);
        checked_send(sock, NULL, 0, 0);
        checked_send(sock, paragraph2, strlen(paragraph2), 0);
        checked_send(sock, NULL, 0, 0);
        checked_send(sock, paragraph3, strlen(paragraph3), 0);

        close(sock);
    }
    return 0;
}
