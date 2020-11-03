#include <iostream>
#include <string>
#include <memory>
#include <exception>

using namespace std;

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

class Factory {
    private:
        Factory() {}
    public:
        // return a product depending on the input
        static std::shared_ptr<Product>
            create(const std::string& name) {
                if (name == "Product1")
                    return std::make_shared<Product1>();
                else if (name == "Product2")
                    return std::make_shared<Product2>();
                else {
                    throw std::runtime_error("Can't find product name");
                }
                return nullptr;
            }
};

int main()
{
    std::shared_ptr<Product> product = 
        Factory::create("Product2");
    
    cout << "Product  : " << product->name() << endl;
}
