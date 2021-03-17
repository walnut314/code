#include <iostream>
#include <memory>

class Factory {
private:
    Factory() { std::cout << "factory created" << std::endl; }
public:
    static std::shared_ptr<Factory> instance() {
        static std::shared_ptr<Factory> instance(new Factory());
        std::cout << instance << std::endl;
        return instance;
    }
};

int main()
{
    std::shared_ptr<Factory> factory1 = Factory::instance();
    std::shared_ptr<Factory> factory2 = Factory::instance();
    std::shared_ptr<Factory> factory3 = Factory::instance();
    return 0;
}

