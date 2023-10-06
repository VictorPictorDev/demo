#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;
using namespace filesystem;

string createDirectory(const path& pdir);
string deleteDirectory(const path& pdir);
string createFile(const path& pf);
string deleteFile(const std::filesystem::path& pf);
string setToFile(const path& pf, string text);
string addToFile(const path& pf, string text);
string cd(const path& p);
string printTree(const path& path, const int& level);
