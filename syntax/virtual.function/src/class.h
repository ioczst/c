#include <iostream>

class Base
{
public:
    virtual void virt() { std::cout << "virt_Base" << std::endl; }
    void non_virt() { std::cout << "non_virt_Base" << std::endl; }
};

class Derived  : public Base
{
public:
    void virt() { std::cout << "virt_Derived" << std::endl; }
    void non_virt() { std::cout << "non_virt_Derived" << std::endl; }
};