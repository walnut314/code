#include <iostream>
#include <string>
#include <memory>

class Product {
public:
    virtual std::string name() const = 0;
};

class Product1 : public Product {
public:
    std::string name() const { return "Product1"; }
};

class Product2 : public Product {
public:
    std::string name() const { return "Product2"; }
};

class AbstractFactory {
public:
    virtual std::shared_ptr<Product> create() const = 0;
};

class Factory1 : public AbstractFactory {
public:
    std::shared_ptr<Product> create() const {
        return std::make_shared<Product1>();
    }
};

class Factory2 : public AbstractFactory {
public:
    std::shared_ptr<Product> create() const {
        return std::make_shared<Product2>();
    }
};

int main()
{
    std::shared_ptr<AbstractFactory> factory1 =
        std::make_shared<Factory1>();
    std::shared_ptr<AbstractFactory> factory2 =
        std::make_shared<Factory2>();

    std::shared_ptr<Product> product1 = factory1->create();
    std::shared_ptr<Product> product2 = factory2->create();

    std::cout << product1->name() << std::endl;
    std::cout << product2->name() << std::endl;

    return 0;
}


