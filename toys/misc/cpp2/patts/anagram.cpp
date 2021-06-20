#include <bits/stdc++.h>

using namespace std;

// simulated input - assumption lower case ascii
vector<string> words = { "pans", "pots", "opt", "snap", "stop", "tops", "stor", "rots" };

#define WORDMAX (100)
int charcomp(const void *x, const void *y) { return *((char *) x) - *((char *) y); }

vector<pair<string,string>> signout;
void sign()
{
    string sig;
    for (auto word : words) {
        sig = word;
        sort(sig.begin(), sig.end());
        cout << word << ":" << sig << endl;
        signout.push_back(make_pair(sig, word));
    }
}

int main()
{
    sign();
    sort(signout.begin(), signout.end());
    cout << "sorted sig/word anagrams\n";
    for (auto p : signout) {
        cout << p.first << " - " << p.second << endl;
    }
    cout << "now squash each anagram on single line\n";
    string curr_sign;
    for (auto p : signout) {
        if (curr_sign.size() == 0) {
            curr_sign = p.first;
            cout << p.first << ": ";
        } else
        if (curr_sign != p.first) {
            curr_sign = p.first;
            cout << endl;
            cout << p.first << ": ";
        }
        cout << p.second << ", "; // << p.second << endl;
    }
    return 0;
}

