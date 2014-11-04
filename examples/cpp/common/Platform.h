#pragma once

class Platform {
public:
  static void sleepMillis(int millis);
  static long elapsedMillis();
  static float elapsedSeconds();
  static void fail(const char * file, int line, const char * message, ...);
  static void say(std::ostream & out, const char * message, ...);
  static std::string format(const char * formatString, ...);
  static std::string getResourceData(Resource resource);
  static std::vector<uint8_t> getResourceByteVector(Resource resource);
  static std::stringstream getResourceStream(Resource resource);
  static std::string replaceAll(const std::string & in, const std::string & from, const std::string & to);
  
  static std::string readFile(const std::string & filename);
  static time_t lastModifiedTime(const std::string & filename);
  static bool fileExists(const std::string & filename);

  static void addShutdownHook(std::function<void()> f);
  static void runShutdownHooks();
};

#define FAIL(...) Platform::fail(__FILE__, __LINE__, __VA_ARGS__)
#define SAY(...) Platform::say(std::cout, __VA_ARGS__)
#define SAY_ERR(...) Platform::say(std::cerr, __VA_ARGS__)
