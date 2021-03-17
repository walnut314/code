#include <iostream>
#include <memory>

class Product {
private:
    int m_i = 1;
    std::shared_ptr<int> m_p = std::make_shared<int>(3);
public:
    int i() const { return m_i; }
    std::shared_ptr<const int> p() const { return m_p; }
    virtual std::shared_ptr<const double> p2() const = 0;
    virtual Product *clone() const = 0;
protected:
    static void clone_imp(Product* product) {
        product->m_p = std::make_shared<int>(*product->m_p);
    }
};

class Product2: public Product {
private:
    std::shared_ptr<double> m_p2 = std::make_shared<double>(5.2);
public:
    std::shared_ptr<const double> p2() const { return m_p2; }
    virtual Product2* clone() const {
        Product2 *product = new Product2(*this);
        clone_imp(product);
        return product;
    }
protected:
    static void clone_imp(Product2* product) {
        Product::clone_imp(product);
        product->m_p2 = std::make_shared<double>(*product->m_p2);
    }
};

template <typename T1, typename T2>
void print(const T1& t1, const T2& t2)
{
    std::cout << t1 << t2 << std::endl;
}

int main()
{
    Product* product = new Product2();
    Product* clone = product->clone();

    print("Product: m_i = ",  product->i());
    print("Cloned : m_i = ",  clone  ->i());
    print("Product: m_p = ",  product->p());
    print("Cloned : m_p = ",  clone  ->p());
    print("Product:*m_p = ", *product->p());
    print("Cloned :*m_p = ", *clone  ->p());
    print("Product: m_p2= ",  product->p2());
    print("Cloned : m_p2= ",  clone  ->p2());
    print("Product:*m_p2= ", *product->p2());
    print("Cloned :*m_p2= ", *clone  ->p2());

    return 0;
}

