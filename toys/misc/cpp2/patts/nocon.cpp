#include <bits/stdc++.h>

using namespace std;

void lambda()
{
    auto lambda = [](const double& a_) { return 2. * a_; };
    double res = lambda(3.);
    cout << res << endl;

    double c = 3.14;
    auto lambda2 = [=](const double& a_) { return c * a_; };
    res = lambda2(4.);
    cout << res << endl;

    auto lambda3 = [&](const double& a_) { return c * a_; };
    res = lambda3(6.);
    cout << res << endl;

    auto lambda4 = [&](const double& a_)->int { return c * a_; };
    int ires = lambda4(6.);
    cout << ires << endl;
}


int main()
{
    lambda();
    return 0;
}

