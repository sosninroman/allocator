#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
int main()
{
    try
    {
        int i = 1;
        std::cout << i;

    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
