#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>

using namespace std;
string trim(string& s) {
    int pos = 0;
    while (!isspace(s[pos])) {
        pos++;
    }
    string ss = s;
    s = s.substr(0, pos);
    return ss.substr(pos + 1, ss.size() - s.size());
}
int main()
{
    int a, b, c, d;
    ifstream fin("demo.txt");
    while (!fin.eof()) {
        string s;
        getline(fin, s);
        auto ss = trim(s);
        if (s == "aaa") {
            a = atoi(ss.c_str());
        }
        cout << a << endl;
        cout << s << endl;
        cout << ss << endl;
    }
    
}