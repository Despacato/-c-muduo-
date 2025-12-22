#include "Timestamp.h"
#include <iostream>
int main()
{
    Timestamp ts = Timestamp::now();
    std::cout << "Current time: " << ts.toString() << std::endl;
    return 0;
}