#include <iostream>
#include <memory>

class Product {
public:
    int m_i = 1;
    std::shared_ptr<int> m_p = std::make_shared<int>(3);
    // clone product
    Product *clone() const {
        Product *product = new Product(*this);
        product->m_p = std::make_shared<int>(*m_p);
        return product;
    }
};

template <typename T1, typename T2>
void print(const T1& t1, const T2& t2)
{
    std::cout << t1 << t2 << std::endl;
}

int main()
{
    Product* product = new Product();
    Product* clone   = product->clone();

    print("Product: m_i = ", product->m_i);
    print("Product: m_i = ", clone->m_i);
    print("Product: m_p = ", product->m_p);
    print("Product: m_p = ", clone->m_p);
    print("Product:*m_p = ", *product->m_p);
    print("Product:*m_p = ", *clone->m_p);

    return 0;
}

