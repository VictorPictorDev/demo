#include "filesystem.h"


string createDirectory(const path& pdir)
{
	if (create_directory(pdir))
		return "Directory is created";
	else
		return "Directory isn't created";
}

string deleteDirectory(const path& pdir)
{
	if (remove_all(pdir))
		return "Directory is removed";
	else
		return "Ditectory isn't removed";
}

string createFile(const path& pf)
{
	ofstream fout(pf);
	if (fout.is_open())
	{
		fout.close();
		return "File is created";
	}
	else
		return "File isn't created";
}

string deleteFile(const path& pf)
{
	if (remove(pf))
		return "File is removed";
	else
		return "File isn't removed";
}

string setToFile(const path& pf, string text)
{
	ofstream fout(pf);
	if (!fout.is_open())
	{
		return "File can't be opened";
	}
	else
	{
		fout << text;
		fout.close();
		return "Add information to file.";
	}
}

string addToFile(const path& pf, string text)
{
	ofstream fout(pf, ios::app);
	if (!fout.is_open())
	{
		return "File can't be opened";
	}
	else
	{
		fout << text;
		fout.close();
		return "Add information to file.";
	}
}

string cd(const path& p)
{
	if (exists(p) && is_directory(p))
	{
		//current_path(p);
		return "Current directory is " + p.string() + "\n";
	}
	else
	{
		return "Directory " + p.string() + " does't exist" + "\n";
	}
}

string printTree(const path& path, const int& level)
{
	string tree;
	for (const auto& entry : directory_iterator(path))
	{
		tree += string(level + 1, ' ') + "|-- " + entry.path().filename().string() + "\n";

		if (is_directory(entry))
		{
			printTree(entry.path().string(), level + 1);
		}
	}
	return tree;
}