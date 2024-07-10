
#include "class.h"

void print_cast_value(Base value)
{
    value.virt();
    value.non_virt();
    std::cout << "--------------------" << std::endl;
}
void print_cast_ptr(Base *ptr)
{
    ptr->virt();
    ptr->non_virt();
    std::cout << "--------------------" << std::endl;
}
void print_cast_ref(Base &ref)
{
    ref.virt();
    ref.non_virt();
    std::cout << "--------------------" << std::endl;
}

int main()
{
    Derived d = Derived();
    print_cast_value(d);
    print_cast_ptr(&d);
    print_cast_ref(d);

    return 0;
}

