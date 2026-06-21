#ifndef TEST_SUITES_H
#define TEST_SUITES_H

#include <string>

void RunRobotTests(void);
void RunObstacleTests(void);
void RunLidarTests(void);
void RunMapTests(void);
void RunNavigationTests(void);

bool RunRobotTestByName(const std::string& name);
bool RunObstacleTestByName(const std::string& name);
bool RunLidarTestByName(const std::string& name);
bool RunMapTestByName(const std::string& name);
bool RunNavigationTestByName(const std::string& name);
bool RunTestByName(const std::string& name);

#endif // TEST_SUITES_H
