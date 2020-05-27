#include "plugins/simulator/visualizations/webviz/utility/PortCheck.h"

#include <sys/socket.h>

#include "gtest/gtest.h"

/* Currently these tests consider we have Port 3000 available,
  if not, change the following variable */

const unsigned int PORT = 3000;

TEST(UtilityPortcheck, CheckEmptyPort) {
  ASSERT_TRUE(PortChecker::CheckPortTCPisAvailable(PORT));
};

/****************************************/
/****************************************/

TEST(UtilityPortcheck, CheckUsedPort) {
  /* using a socket */
  int n_fdSocket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in sServAddr;

  sServAddr.sin_family = AF_INET;
  sServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  sServAddr.sin_port = htons(PORT);

  int n_bindSock =
    bind(n_fdSocket, (struct sockaddr *)&sServAddr, sizeof(sServAddr));

  EXPECT_FALSE(PortChecker::CheckPortTCPisAvailable(PORT));

  close(n_bindSock);
  close(n_fdSocket);
};

/****************************************/
/****************************************/

TEST(UtilityPortcheck, CheckReuseOfPort) {
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

  EXPECT_TRUE(PortChecker::CheckPortTCPisAvailable(PORT));
  EXPECT_TRUE(PortChecker::CheckPortTCPisAvailable(PORT));

  close(n_bindSock2);
  close(n_bindSock);
  close(n_fdSocket);
};
