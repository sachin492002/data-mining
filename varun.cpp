#include <bits/stdc++.h>
using namespace std;
int count = 0;
void printStars(int n) {
    int idx = 0;
    if(n > 1) printStars(n - 1);
    for(idx = 0; idx < n; idx++) count++;;
}
int main() {
    printStars(10);
    cout << count << endl;
}