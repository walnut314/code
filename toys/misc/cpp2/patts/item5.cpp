#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <functional>

class Product; // forward decl
class Factory {
public:
    std::map<std::string, std::function<std::shared_ptr<Product>()>> m_map_create;
    std::shared_ptr<Product> create(const std::string& name) const {
        auto fc = m_map_create.find(name);
        if (fc == m_map_create.end())
            throw std::runtime_error("can't find product name");
        return fc->second();
    }
    static std::shared_ptr<Factory> instance() { // simple singleton
        static std::shared_ptr<Factory> instance =
            std::make_shared<Factory>();
        return instance;
    }
};

class Product {
protected:
    template <typename T>
        static size_t register_fc_creation(const std::string& name) {
            Factory::instance()->m_map_create[name]
                = ([]() { return std::make_shared<T>(); });
            return Factory::instance()->m_map_create.size();
        }
public:
    virtual std::string name() const = 0;
};

class Product1 : public Product {
    static size_t m_register_fc_creation;
public:
    virtual std::string name() const { return "Product1"; }
};

class Product2 : public Product {
    static size_t m_register_fc_creation;
public:
    virtual std::string name() const { return "Product2"; }
};

// register fc creation
size_t Product1::m_register_fc_creation = Product::register_fc_creation<Product1>("Product1");
size_t Product2::m_register_fc_creation = Product::register_fc_creation<Product2>("Product2");

int main()
{
    std::shared_ptr<Product> product = Factory::instance()->create("Product2");
    std::cout << "Product: " << product->name() << std::endl;
    return 0;
}

