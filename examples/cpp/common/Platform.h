#pragma once


#if (defined(WIN64) || defined(WIN32))
#define OS_WIN
#elif (defined(__APPLE__)
#define OS_OSX
#else
#define OS_LINUX
#endif

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
};

#if defined(OS_WIN)
#define ON_WINDOWS(runnable) runnable()
#define NOT_ON_WINDOWS(runnable)
#else
#define ON_WINDOWS(runnable)
#define NOT_ON_WINDOWS(runnable) runnable()
#endif

#if defined(OS_OSX)
#define ON_MAC(runnable) runnable()
#define NOT_ON_MAC(runnable)
#else
#define ON_MAC(runnable)
#define NOT_ON_MAC(runnable) runnable()
#endif

#if defined(OS_LINUX)
#define ON_LINUX(runnable) runnable()
#define NOT_ON_LINUX(runnable)
#else
#define ON_LINUX(runnable)
#define NOT_ON_LINUX(runnable) runnable()
#endif


// Windows has a non-standard main function prototype
#ifdef OS_WIN
#define MAIN_DECL int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#define MAIN_DECL int main(int argc, char ** argv)
#endif

#define FAIL(...) Platform::fail(__FILE__, __LINE__, __VA_ARGS__)
#define SAY(...) Platform::say(std::cout, __VA_ARGS__)
#define SAY_ERR(...) Platform::say(std::cerr, __VA_ARGS__)
