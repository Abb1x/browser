#include <ctime>
#include <utils/log.hpp>

void __log(int level, const char *file, int line, const char *format, ...)
{
    va_list vl;
    va_start(vl, format);

    char *target = new char[strlen(format) + 128];

    vsprintf(target, format, vl);

    time_t tt = time(NULL);

    tm *current_time = localtime(&tt);

    std::string time = std::to_string(current_time->tm_hour) + ":" + std::to_string(current_time->tm_min) + ":" + std::to_string(current_time->tm_sec);

    std::string color = "";

    switch (level)
    {
    case INFO:
        color = "\033[1;34mINFO";
        break;
    case WARNING:
        color = "\033[1;33mWARNING";
        break;
    case ERROR:
        color = "\033[1;31mERROR";
        break;
    case DEBUG:
        color = "\033[1;36mDEBUG";
        break;
    }
    printf("\033[1;30m%s %s (%s:%d) \033[0m%s \n", time.c_str(), color.c_str(), file, line, target);
    printf("Please realize your proposedChanges and become one with the universe...");
    va_end(vl);

    delete[] target;
}
