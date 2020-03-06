#ifdef __GNUC__
#include <bits/stdc++.h>
#else
#include <iostream>
#include <vector>
#endif

#define DEBUG

using namespace std;
/*************************
 * @brief :判断这n个整数是否连续
 * @args  :nums--要判断的数组
 *         n-----数组的大小
 *************************/
bool foo(vector<int>& nums, int n)
{
    // 给数组排序，时间复杂度O(lgn)
    sort(nums.begin(), nums.end());
    // 记录０出现的个数
    int zeroNums = 0;
    // 记录两两数字差的和,差要大于一，一为连续
    int flag = 0;
    for (int i = 0; i < n - 1; i++) {
        if (nums[i] == 0) {
            zeroNums++; // 记录0个数
        } else {
            if (nums[i] == nums[i + 1]) {
                continue; // 如果遇到一样的数跳过，因为重复的数是允许的
            } else {
                flag += (nums[i + 1] - nums[i] - 1); // 记录两两数字差的和,差要大于一，一为连续
            }
        }
    }
#ifdef DEBUG
    cout << flag << endl;
#endif
    // 判断0的个数和数字差的和，如果0的个数大于等于则返回true
    if (flag <= zeroNums) {
        return true;
    } else {
        return false;
    }
}

int main()
{
    int n;
    // 赋值长度为n的数组
    cin >> n;
    vector<int> nums(n);
    for (int i = 0; i < n; i++) {
        cin >> nums[i];
    }
    auto res = foo(nums, n);
#ifdef DEBUG
    cout << res << endl;
#endif
}