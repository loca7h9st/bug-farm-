#include <iostream>
#include <vector>
#include <algorithm>

#include <string>

using namespace std;

class Soultion
{
public:
    int removeDuplicates(vector<int> &vec)
    {
        if (vec.empty())
        {
            return 0;
        }
        int index = 0;
        for (int i = 1; i < vec.size(); i++)
        {
            if (vec[index] != vec[i])
            {
                vec[++index] = vec[i]; // 使用前置++ 确保了不会覆盖 e.g. index = 0; ++index: 1； i 从1开始
            }
        }
        return index + 1;
    }

    int removeDuplicates1(vector<int> &vec)
    {
        return distance(vec.begin(), unique(vec.begin(), vec.end()));
    }


    int removeDuplicates2(vector<int>& vec)
    {
        return distance(vec.begin(), removeDuplicates(vec.begin(), vec.end(),vec.begin()));
    }


    template<typename InIt, typename OutIt>
    OutIt removeDuplicates(InIt first, InIt last, OutIt output)
    {
        while (first != last)
        {
            *output++ = *first; // *first在调用upper_bound之前就已经被解引用并输出了
            first = upper_bound(first, last, *first); // upper_bound的返回值只是用来更新first  ，为下一次循环做准备
        }
        return output;
    }

};
