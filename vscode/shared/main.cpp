#include <iostream>
#include <libudt/udt.h>

int main()
{
    std::cout << "Chegada do UDT";
    UDTSOCKET serv = UDT::socket(AF_INET, SOCK_STREAM, 0);
    return 0;

}
