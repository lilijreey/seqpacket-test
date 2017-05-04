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

package main

import (
	"fmt"
	"io"
	"net"
	"os"
)

func fatal(msg string, err error) {
	fmt.Fprintf(os.Stderr, "%s: %s\n", msg, err.Error())
	os.Exit(1)
}

func main() {
	buf := make([]byte, 10296)

	conn, err := net.DialUnix("unixpacket", nil, &net.UnixAddr{Name: "@seqpacket/test", Net: "unixpacket"})
	if err != nil {
		fatal("DialUnix", err)
	}

	for {
		consumed, _, err := conn.ReadFromUnix(buf)
		if err == io.EOF {
			fmt.Println("ReadFromUnix: end of file")
			break
		} else if err != nil {
			fatal("ReadFromUnix", err)
		} else if consumed != len(buf) {
			fmt.Printf("Got unexpectedly small message: %d bytes\n", consumed)
		}
	}
}
