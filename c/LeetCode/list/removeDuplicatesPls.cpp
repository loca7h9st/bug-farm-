#include <iostream>
#include <vector>
#include <algorithm>

#include <string>

using namespace std;

class Solution
{
public:
    int removeDuplicates_twice(vector<int>& nums)
    {
        if (nums.size() <= 2)
        {
            return nums.size();
        }
        
        int index = 2;
        for (int i = 2; i < nums.size(); i++)
        {
            if (nums[i] != nums[index - 2])
            {
                nums[index++] = nums[i];
            }
            
        }
        return index;
    }

};