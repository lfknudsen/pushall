#include <stdio.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>

using namespace std;
namespace fs = filesystem;

int main()
{
	const char* filepath = "./.git/config";
	if (!fs::exists(filepath)) {
		cerr << "This is not a valid Git repository." << endl;
		exit(1);
	}

	ifstream f(filepath);
	if (!f) {
		cerr << "Error opening Git information file '" << filepath << "'." << endl;
		exit(1);
	}

	string line;
	const regex rx(" *\\[remote \"([^\"]+)\"\\] *");
	smatch matcher;
	while (getline(f, line)) {
		if (regex_match(line, matcher, rx)) {
			const string cmd = string("git push ").append(matcher[1]);
			system(cmd.c_str());
		}
	}

	f.close();
}
