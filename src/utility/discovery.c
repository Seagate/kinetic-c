/*
* kinetic-c
* Copyright (C) 2015 Seagate Technology.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#define _GNU_SOURCE

#include <stdio.h>

#include <err.h>
#include <errno.h>

#include <netinet/in.h>

#include "kinetic_client.h"
#include "socket99.h"
#include "json.h"

static int discover_service(char *host, int port);


//------------------------------------------------------------------------------
// Main Entry Point Definition
int main(int argc, char** argv)
{

    char *default_host = "239.1.2.3";
    int   default_port = KINETIC_PORT;

    char *host = NULL;
    int   port = 0;

    // TODO: CLI args?

    switch(argc) {
      case 3: {
        host = argv[1];
        port = atoi(argv[2]);
        break;
      }

      case 1: {
        host = default_host;
        port = default_port;
        break;
      }

      default: {
        // TODO: usage();
        break;
      }
    }

    return discover_service(host, port);
}
 
   
//------------------------------------------------------------------------------
// Service discovery

static int discover_service(char *host, int port) {

    struct in_addr mcastAddr;
    struct ip_mreq mreq;

    struct hostent *host_struct;

    int v_true = 1;
    socket99_config cfg = {
        .host = INADDR_ANY,
        .port = port,
        .server = true,
        .datagram = true,
        .sockopts = {
            {/*SOL_SOCKET,*/ SO_BROADCAST, &v_true, sizeof(v_true)},
        },
    };
    socket99_result res;

    if (!socket99_open(&cfg, &res)) {
        errno = res.saved_errno;
        printf("res %d, %d\n", res.status, res.getaddrinfo_error);
        if (res.status == SOCKET99_ERROR_GETADDRINFO) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res.getaddrinfo_error));
            return 1;
        }
        err(1, "socket99_open");
        return 1;
    }

    if((host_struct = gethostbyname(host)) == NULL) {
        err(1, "gethostbyname");
        // usage();
    }

    memcpy(&mcastAddr, host_struct->h_addr_list[0], host_struct->h_length);

    if(!IN_MULTICAST(ntohl(mcastAddr.s_addr))) {
        err(1, "invalid mcast address");
        // usage();
    }

    mreq.imr_multiaddr.s_addr = mcastAddr.s_addr;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (0 != setsockopt(res.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq, sizeof(mreq))) {
        err(1, "failed to join multicast group");
        // usage();
    }

    char buf[1024];
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);

    /* TODO:
     *     + nonblocking, with max timeout
     *     + set up multicast on 239.1.2.3
     *     + if we receive any info, print it */

    for (;;) {
        ssize_t received = recvfrom(res.fd, buf, sizeof(buf), 0,
            (struct sockaddr *)&client_addr, &addr_len);

        if (received > 0) {

            struct json_object  *obj = NULL;
            struct json_object  *val = NULL;
            struct json_tokener *tok = NULL;

            tok = json_tokener_new();

            buf[received] = '\0';

            obj = json_tokener_parse_ex(tok, buf, received);

            if(obj == NULL) {
                if(json_tokener_get_error(tok) != json_tokener_error_parse_eof) {
                    printf("JSON error %d", json_tokener_get_error(tok));
                }
            }

            if(json_object_object_get_ex(obj, "world_wide_name", &val)) {
                printf("World Wide Name: %s\n", json_object_to_json_string(val));
            }


            if(json_object_object_get_ex(obj, "firmware_version", &val)) {
                printf("Firmware Ver: %s\n", json_object_to_json_string(val));
            }

            if(json_object_object_get_ex(obj, "manufacturer", &val)) {
                printf("Manufacturer: %s\n", json_object_to_json_string(val));
            }

            if(json_object_object_get_ex(obj, "model", &val)) {
                printf("Model: %s\n", json_object_to_json_string(val));
            }

            if(json_object_object_get_ex(obj, "serial_number", &val)) {
                printf("Serial Number: %s\n", json_object_to_json_string(val));
            }

            if(json_object_object_get_ex(obj, "protocol_version", &val)) {
                printf("Protocol Ver: %s\n", json_object_to_json_string(val));
            }


            if(json_object_object_get_ex(obj, "network_interfaces", &val)) {
                int i, len = json_object_array_length(val);

                struct json_object *array_obj = NULL;
                struct json_object *array_val = NULL;

                printf("Network Interfaces [%d]\n", len);

                for(i = 0; i < len; i++) {
                    array_obj = json_object_array_get_idx(val, i);

                    if(json_object_object_get_ex(array_obj, "name", &array_val)) {
                        printf("    Name: %s\n", json_object_to_json_string(array_val));
                    }

                    if(json_object_object_get_ex(array_obj, "ipv4_addr", &array_val)) {
                        printf("    IPv4 Address: %s\n", json_object_to_json_string(array_val));
                    }

                    if(json_object_object_get_ex(array_obj, "ipv6_addr", &array_val)) {
                        printf("    IPv6 Address: %s\n", json_object_to_json_string(array_val));
                    }

                    if(json_object_object_get_ex(array_obj, "mac_addr", &array_val)) {
                        printf("    HW Address: %s\n", json_object_to_json_string(array_val));
                    }

                    if((i + 1) < len) {
                      printf("\n");
                    }

                    json_object_put(array_obj);
                }

            }

            if(json_object_object_get_ex(obj, "port", &val)) {
                printf("Port: %s\n", json_object_to_json_string(val));
            }

            if(json_object_object_get_ex(obj, "tlsPort", &val)) {
                printf("TLS Port: %s\n", json_object_to_json_string(val));
            }

            printf("\n\n");

            json_object_put(obj);
        }
    }
    
    return 0;
}
