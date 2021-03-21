#include <iostream>
#include <string>
#include <memory>
#include <exception>

struct BuildDetails {
    std::string name;
    int m_b;
    int m_c;
    BuildDetails(int b, int c) : m_b(b), m_c(c) {}
};

class Product {
public:
    virtual std::string name() const = 0;
};

class Product1 : public Product {
public:
    int m_i;
    virtual std::string name() const { return "Product1"; }
};

class Product2 : public Product {
public:
    double m_d1;
    double m_d2;
    virtual std::string name() const { return "Product2"; }
};

template <typename T>
std::shared_ptr<T> singleton() {
    static std::shared_ptr<T> instance = std::make_shared<T>();
    return instance;
}

class AbstractFactory {
public:
    virtual std::shared_ptr<Product> create (
        const std::shared_ptr<BuildDetails>& buildDetails) const = 0;
};

class Factory1 : public AbstractFactory {
public:
    virtual std::shared_ptr<Product> create (
        const std::shared_ptr<BuildDetails>& buildDetails) const { 
        std::shared_ptr<Product1> product = std::make_shared<Product1>();
        product->m_i = buildDetails->m_b + buildDetails->m_c;
        return product; 
    }
    static std::shared_ptr<Factory1> instance() { return singleton<Factory1>(); }
};

class Factory2 : public AbstractFactory {
public:
    virtual std::shared_ptr<Product> create (
        const std::shared_ptr<BuildDetails>& buildDetails) const { 
        std::shared_ptr<Product2> product = std::make_shared<Product2>();
        product->m_d1 = buildDetails->m_b;
        product->m_d2 = buildDetails->m_c * 2;
        return product;
    }
    static std::shared_ptr<Factory2> instance() { return singleton<Factory2>(); }
};

std::shared_ptr<AbstractFactory> createFactory(
    const std::shared_ptr<BuildDetails>& buildDetails
    )
{ 
    if (buildDetails->name == "Factory1") return Factory1::instance();
    else if (buildDetails->name == "Factory2") return Factory2::instance();
    else throw std::runtime_error("can't find factory name");
}

std::shared_ptr<Product> createProduct(
    const std::shared_ptr<BuildDetails>& buildDetails
    )
{
    return createFactory(buildDetails)->create(buildDetails);
}

int main()
{
    std::shared_ptr<BuildDetails> buildDetails = std::make_shared<BuildDetails>(2, 3);
    buildDetails->name = "Factory2";

    std::shared_ptr<Product> product = createProduct(buildDetails);

    std::cout << product->name() << std::endl;
}

