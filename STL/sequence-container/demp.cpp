#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>

int main(int argc, char **argv) {

    int ia[6] = {23, 567, 34, 77, 57, 108};
    std::vector<int, std::allocator<int> > vi(ia, ia+6);
    vi.insert(99);
    vi.push_back(3);
    std::cout<<std::count_if(vi.begin(), vi.end(),
                not1(bind2nd(std::less<int>(), 40))) << std::endl;
    //找出大于等于40的元素个数
    return 0;
}

/*
[1]第 9 行就分别使用了 container 和 allocator, iterator
[2]第 10 行 count_if使用了 algorithm； (vi.begin(), vi.end())就使用了 iterator;
not1 和 bind2nd 使用了 function adapter ；std::less 使用了function object
*/