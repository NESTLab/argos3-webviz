/**
 * @file <argos3/plugins/simulator/visualizations/webviz/utility/PortCheck.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_PORTCHECK_H
#define ARGOS_WEBVIZ_PORTCHECK_H

#include <arpa/inet.h>
#include <netinet/in.h>
// #include <netdb.h>
#include <sys/socket.h>
// #include <sys/types.h>
#include <unistd.h> /* close() */

#include <iostream>

class PortChecker {
 public:
  static bool CheckPortTCPisAvailable(unsigned int un_port) {
    /* Create socket */
    int n_fdSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (0 > n_fdSocket) {
      std::cerr << "Cannot open socket" << std::endl;
      return false;
    } else {
      struct sockaddr_in sServAddr;

      sServAddr.sin_family = AF_INET;
      sServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      sServAddr.sin_port = htons(un_port);

      /* To reuse the port, if recently closed */
      int enable = 1;
      if (
        setsockopt(n_fdSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) <
        0) {
        std::cerr << "setsockopt(SO_REUSEADDR) failed " << std::endl;
        return false;
      }

      int n_bindSock =
        bind(n_fdSocket, (struct sockaddr *)&sServAddr, sizeof(sServAddr));
      if (0 > n_bindSock) {
        close(n_fdSocket);

        std::cerr << "Cannot bind to port " << un_port << std::endl;
        return false;
      } else {
        close(n_bindSock);
        close(n_fdSocket);

        return true;
      }
    }
  }
};
#endif