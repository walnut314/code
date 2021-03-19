#include <iostream>
#include <memory>

struct BuildDetails {
    int m_b;
    int m_c;
    BuildDetails(int b, int c) : m_b(b), m_c(c) {}
};

class Product {
public:
    int m_a;
};

class Factory {
    Factory(){}
public:
    static std::shared_ptr<Product> create(const BuildDetails& buildDetails) {
        std::shared_ptr<Product> product = std::make_shared<Product>();
        product->m_a = buildDetails.m_b + buildDetails.m_c;
        return product;
    }
};

int main()
{
    BuildDetails buildDetails(2, 3);
    std::shared_ptr<Product> product = Factory::create(buildDetails);
    std::cout << product ->m_a << std::endl;
    return 0;
}

