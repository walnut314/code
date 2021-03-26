#include <iostream>
#include <memory>

struct Base {
    virtual ~Base() {};
};

class Product : public Base {
private:
    int m_a;
public:
    Product(int a) : m_a(a) {}
    int value() const { return m_a; }
};

class Decorator : public Base {
private:
    std::shared_ptr<Product> m_product;
    int m_add;
public:
    Decorator(int a, int add=0)
        : m_product(std::make_shared<Product>(a)), m_add(add) {}
    int value() const { return m_product->value(); }
    int value_new() const { return m_product->value() + m_add; }
};

template <typename T1, typename T2>
void print(const T1& t1, const T2& t2) {
    std::cout << t1 << t2 << std::endl;
}

int main()
{
    Product product(4);
    print("Product(a).value=", product.value());
    Decorator decorator(4, 5);
    print("Decorator(4,5).value=", decorator.value());
    print("Decorator(4,5).value_new=", decorator.value_new());
    return 0;
}
