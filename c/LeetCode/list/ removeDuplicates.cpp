#include <iostream>
#include <vector>


using namespace std;


class Soultion
{
public:
    int removeDuplicates(vector<int>& vec)
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
};