#include "builtins.hpp"
#include "util.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <limits.h>

std::unordered_map<std::string, CmdHandler> builtins;

void init_builtins()
{
    builtins["exit"] = [](const std::vector<std::string> &args)
    {
        int code = (args.size() > 1) ? std::stoi(args[1]) : 0;
        std::exit(code);
    };
    builtins["echo"] = [](const std::vector<std::string> &args)
    {
        for (size_t i = 1; i < args.size(); ++i)
            std::cout << args[i] << (i + 1 < args.size() ? " " : "");
        std::cout << "\n";
    };
    builtins["type"] = [](const std::vector<std::string> &args)
    {
        std::string cmd = (args.size() > 1) ? args[1] : "";
        if (builtins.count(cmd))
            std::cout << cmd << " is a shell builtin\n";
        else
        {
            std::string path = execSearch(cmd);
            if (!path.empty())
                std::cout << cmd << " is " << path << "\n";
            else
                std::cout << cmd << ": not found\n";
        }
    };
    builtins["pwd"] = [](const std::vector<std::string> &args)
    {
        char buffer[PATH_MAX];
        if (getcwd(buffer, PATH_MAX) != nullptr)
        {
            std::cout << buffer << "\n";
        }
        else
        {
            std::cerr << "pwd: error getting current directory\n";
        }
    };
    builtins["cd"] = [](const std::vector<std::string> &args)
    {
        std::string dir = (args.size() > 1 ? args[1] : "~");
        char *point = const_cast<char *>(dir.c_str());
        if (dir[0] == '~')
        {
            const char *home = std::getenv("HOME");
            int rs = chdir(home);
            if (rs == -1)
                std::cout << "cd: " << dir << ": No such file or directory" << std::endl;
        }
        else
        {
            int rs = chdir(point);
            if (rs == -1)
                std::cout << "cd: " << dir << ": No such file or directory" << std::endl;
        }
    };
}