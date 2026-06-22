#ifndef TEST_SUITES_H
#define TEST_SUITES_H

#include <string>

void RunRobotTests(void);
void RunPiControllerTests(void);
void RunBlockingRobotManagerTests(void);
void RunProximitySensorTests(void);
void RunMapTests(void);
void RunNavigationTests(void);

bool RunRobotTestByName(const std::string& name);
bool RunPiControllerTestByName(const std::string& name);
bool RunBlockingRobotManagerTestByName(const std::string& name);
bool RunProximitySensorTestByName(const std::string& name);
bool RunMapTestByName(const std::string& name);
bool RunNavigationTestByName(const std::string& name);
bool RunTestByName(const std::string& name);

#endif // TEST_SUITES_H
