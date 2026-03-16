#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <sstream>

using namespace std;

int main()
{
    int n;
    int d;
    cin >> n >> d;    
    vector<long long> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }
    
    long long r = (a[0] % d + d) % d;
    bool possible = true;
    
    for (int i = 1; i < n; i++)
    {
        long long current_r = (a[i] % d + d) % d;
        if (r != current_r)
        {
            possible = false;
            break;
        }
    }
    
    if (!possible)
    {
        cout << -1 << endl;
        return 0;
    }
    
    vector<long long> k(n);
    for (int i = 0; i < n; ++i)
    {
        k[i] = (a[i] - r) / d;
    }
    
    sort(k.begin(), k.end());
    long long median = k[n / 2];
    
    long long ans = 0;
    for (int i = 0; i < n; i++)
    {
        ans += llabs(k[i] - median);
    }
    
    cout << ans << endl;
    return 0;
}
