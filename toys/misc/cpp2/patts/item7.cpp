#include <iostream>
#include <memory>

struct BuildDetails {
    int m_b;
    int m_c;
    BuildDetails(int b, int c) : m_b(b), m_c(c) {}
    std::shared_ptr<BuildDetails> clone() const {
        return std::make_shared<BuildDetails>(*this);
    }
};

class Product {
public:
    int m_a;
    std::shared_ptr<const BuildDetails> m_buildDetails = nullptr;
};

class Factory {
public:
    static std::shared_ptr<Product>
    create(const std::shared_ptr<BuildDetails>& buildDetails) {
        std::shared_ptr<Product> product = 
            std::make_shared<Product>();
        product->m_a = buildDetails->m_b + buildDetails->m_c;
        product->m_buildDetails = buildDetails;
        return product;
    }
};

int main()
{
    std::shared_ptr<BuildDetails> buildDetails
        = std::make_shared<BuildDetails>(2, 3);
    std::shared_ptr<Product> product
        = Factory::create(buildDetails);

    std::shared_ptr<BuildDetails> buildDetails_cloned
        = buildDetails->clone();
    buildDetails_cloned->m_b += 1;
    std::shared_ptr<Product> product2
        = Factory::create(buildDetails_cloned);

    std::cout << "Product           : "
        << product->m_a << std::endl;
    std::cout << "Product_updated   : "
        << product2->m_a << std::endl;
    std::cout << "Risk              : "
        << product2->m_a - product->m_a << std::endl;

    return 0;
}

