#include <libtech/hashmap.hpp>
#include <string>
#include <iostream>

int main() {
    std::cout << "nullptr == " << static_cast<void*>(nullptr) << '\n';
    tech::HashMap<std::string, int> map;
    map["somename"];
    std::cerr << "done\n";
    std::cout << map["somename"] << '\n';
    return 0;
}