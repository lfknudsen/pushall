#include <stdio.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>

using namespace std;
namespace fs = filesystem;

FILE* open(const char* cmd) {
#if WIN32
	return _popen(cmd, "r");
#else
	return popen(cmd, "r");
#endif
}

void close(FILE* p) {
#if WIN32
	_pclose(p);
#else
	pclose(p);
#endif
}

const regex rx(" *\\[remote \"([^\"]+)\"\\] *");

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
	smatch matcher;
	while (getline(f, line)) {
		if (regex_match(line, matcher, rx)) {
			string prefix = "git push ";
			const char* cmd = prefix.append(matcher[1].str()).c_str();
			FILE* p = open(cmd);
			char stdout_buffer[1024];
			fgets(stdout_buffer, sizeof(stdout_buffer), p);
			const string stdout_str = string(stdout_buffer);
			if (stdout_str._Starts_with("remote: Credentials are incorrect or have expired. Retry your command")) {
				close(p);
				// `system` is more portable than (_)popen, but doesn't let you capture stdout
				// Since we only retry once, we don't capture output on the second attempt.
				system(cmd);
			}
		}
	}

	f.close();
}
