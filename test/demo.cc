#include <bits/stdc++.h>
#define BIG(x, y) x > y
#define S(x, y) x##y
#define STR(s) #s
using namespace std;

string parseNumber(std::string s)
{
    std::istringstream iss(s);
    string ss;
    while (iss >> ss) {
        cout << ss << endl;
    }
    return ss;
}
struct Demo {
    int num1;
    int num2;
};
int main(int argc, char** argv)
{
    unordered_map<int, Demo> um;
    um[1].num1 = 2;
    //um[1].num2 = 2;
    auto i = um.find(1);
    if (i != um.end()) {
        cout << "end" << endl;
        cout << i->second.num2 << endl;
    }
}