#include "user_loop_functions.h"

/****************************************/
/****************************************/

CTestUserFunctions::CTestUserFunctions() {
  std::cout << "Test user functions" << std::endl;
}

/****************************************/
/****************************************/

REGISTER_WEBVIZ_USER_FUNCTIONS(CTestUserFunctions, "test_user_functions")
