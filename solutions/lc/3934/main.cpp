// LeetCode 3934. 最短唯一子数组
// https://leetcode.cn/problems/smallest-unique-subarray/
// Submit only the code between the two markers; the rest runs it locally.
// Local input uses LeetCode's test case format: one value per line.
#include "leetcode.h"
#include <string>
#include "string/suffix_array.hpp"

// ---- LeetCode submission begins ----
class Solution {
public:
    int smallestUniqueSubarray(vector<int>& nums) {
        cp::SuffixArray sa(nums);
        int n = nums.size();
        int ans = n;
        for (int i = 0; i < n; ++i) {
            int len = sa.height[i] + 1;
            if (i + 1 < n) len = max(sa.height[i], sa.height[i + 1]) + 1;
            if (len <= n - sa.sa[i]) ans = min(ans, len);
        }
        return ans;
    }
};
// ---- LeetCode submission ends ----

int main() {
    return lc::run_solution(&Solution::smallestUniqueSubarray);
}
