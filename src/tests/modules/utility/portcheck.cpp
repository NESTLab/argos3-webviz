#include "plugins/simulator/visualizations/webviz/utility/PortCheck.h"

#include <sys/socket.h>

#include "gtest/gtest.h"

/* Currently these tests consider we have Port 3000 available,
  if not, change the following variable */

const unsigned int PORT = 3000;

TEST(modules_utility_portcheck, Check_Empty_Port) {
  EXPECT_EQ(true, PortChecker::CheckPortTCPisAvailable(PORT));
};

/*********************************
 *********************************/

TEST(modules_utility_portcheck, Check_Used_Port) {
  /* using a socket */
  int n_fdSocket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in sServAddr;

  sServAddr.sin_family = AF_INET;
  sServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  sServAddr.sin_port = htons(PORT);

  int n_bindSock =
    bind(n_fdSocket, (struct sockaddr *)&sServAddr, sizeof(sServAddr));

  EXPECT_EQ(false, PortChecker::CheckPortTCPisAvailable(PORT));

  close(n_bindSock);
  close(n_fdSocket);
};

/*********************************
 *********************************/

TEST(modules_utility_portcheck, Check_Reuse_of_Port) {
  /* using a socket */
  int n_fdSocket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in sServAddr;

  sServAddr.sin_family = AF_INET;
  sServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  sServAddr.sin_port = htons(PORT);

  /* Disable reuse */
  int enable = 1;
  setsockopt(n_fdSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

  int n_bindSock =
    bind(n_fdSocket, (struct sockaddr *)&sServAddr, sizeof(sServAddr));

  int n_bindSock2 =
    bind(n_fdSocket, (struct sockaddr *)&sServAddr, sizeof(sServAddr));

  EXPECT_EQ(true, PortChecker::CheckPortTCPisAvailable(PORT));
  EXPECT_EQ(true, PortChecker::CheckPortTCPisAvailable(PORT));

  close(n_bindSock2);
  close(n_bindSock);
  close(n_fdSocket);
};
