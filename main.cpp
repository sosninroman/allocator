#include <iostream>
#include <map>
#include "slist.h"
#include "allocator.h"

int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

template<class Key, class Value>
std::ostream& operator<<(std::ostream& stream, const std::pair<const Key,Value>& pair)
{
    stream << pair.first << " " << pair.second;
    return stream;
}

template<class T>
void printContent(const T& container)
{
    for(const auto& val : container)
        std::cout << val << std::endl;
}

int main()
{
    const size_t dataSz = 10;

    std::map<int,int> standardAllocMap;
    for(size_t i = 0; i < dataSz; ++i)
        standardAllocMap.emplace(i, factorial(i) );

    std::map<int, int, std::less<int>, allocator::Allocator<std::pair<const int, int>, 10>> customAllocMap;
    for(size_t i = 0; i < dataSz; ++i)
        customAllocMap.emplace(i, factorial(i) );

    //printContent(standardAllocMap);
    printContent(customAllocMap);

    slist::SList<int> standardAllocSList;
    for(size_t i = 0; i < dataSz; ++i)
        standardAllocSList.addItem(i);

    slist::SList<int, allocator::Allocator<int, 10>> customAllocSList;
    for(size_t i = 0; i < dataSz; ++i)
        customAllocSList.addItem(i);

    //printContent(standardAllocSList);
    printContent(customAllocSList);

    slist::SList<int> mc2s(std::move(customAllocSList) );
    std::cout << "slist::SList<int> mc2s(std::move(customAllocSList) ) result" << std::endl;
    printContent(mc2s);

    slist::SList<int, allocator::Allocator<int,10>> s2c(standardAllocSList);
    std::cout << "slist::SList<int, allocator::Allocator<int,10>> s2c(standardAllocSList) result" << std::endl;
    printContent(s2c);

    slist::SList<int, allocator::Allocator<int,10>> c2c(std::move(customAllocSList) );
    std::cout << "slist::SList<int, allocator::Allocator<int,10>> c2c(std::move(customAllocSList) )" << std::endl;
    printContent(c2c);

    return 0;
}
