
#include <iostream>
#include <variant>
#include <typeinfo>

int main()
{
    std::variant<int, double, float> var1 = 1;
    std::variant<int, double, float> var2 = 'c';

    std::visit([](auto&& item, auto&& item2)
        {
            std::cout << "1" << "\n";

        }, var1, var2);

    return 0;
}
