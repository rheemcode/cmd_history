#include <cstdio>
#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <filesystem>

// simple script that excute command line and writes the given commands to a file `.cmd_hisory`
// this is because i cant remember most command if i try to reuse them later

using namespace std;

string exec(const char *cmd)
{
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);

    if (!pipe)
        throw runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    return result;
}

int main(void)
{
    string args;
    filesystem::path path = filesystem::current_path();
    cout << path.string() << ">";
    ofstream file(".cmd_history", ios_base::app);

    while (getline(cin, args))
    {
        file.write(args.c_str(), args.length());
        file.write("\n", 1);

        // writing to the file probably stores it in a buffer and will only
        // write when its full or done so if the command line was closed it ends up
        // writing nothing to the file

        file.flush();

        // if command is `cd` change the path exefute that and change dir
        // not checking if trying to execute multiple commands i.e cd .. && gcc
        if (size_t t = args.find("cd") != string::npos)
        {
            auto d = args.substr(t + 2);
            if (filesystem::is_directory(d))
            {
                filesystem::current_path(d);
                path = filesystem::current_path();
                cout << path.string() << ">";
                continue;
            }
        }

        cout << exec(args.c_str()) << "\n";
        cout << path.string() << ">";
    }
}