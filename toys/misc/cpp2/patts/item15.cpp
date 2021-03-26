#include <iostream>
#include <memory>

// Header
class Implementation;

class Interface {
protected:
    std::unique_ptr<Implementation> m_pimpl;
public:
    virtual int value() const = 0;
};

class Interface_derived_1 : public Interface {
public:
    Interface_derived_1();
    int value() const;
};

class Interface_derived_2 : public Interface {
public:
    Interface_derived_2();
    int value() const;
};

// C++ - body
class Implementation {
public:
    virtual int value() const = 0;
};

class Implementation_derived_1 : public Implementation {
private:
    int m_a = 0;
public:
    Implementation_derived_1(int a) { m_a = a; }
    int value() const { return m_a; }
};

class Implementation_derived_2 : public Implementation {
private:
    double m_d1;
    double m_d2;
public:
    Implementation_derived_2(double d1, double d2)
        : m_d1(d1), m_d2(d2) {}
    int value() const { return (int) (m_d1 + m_d2); }
};

Interface_derived_1::Interface_derived_1() {
    m_pimpl = std::make_unique<Implementation_derived_1>(3);
}
int Interface_derived_1::value() const {
    return m_pimpl->value();
}

Interface_derived_2::Interface_derived_2() {
    m_pimpl = std::make_unique<Implementation_derived_2>(1.0, 4.0);
}

int Interface_derived_2::value() const {
    return m_pimpl->value();
}

template<typename T1>
void print(const T1& t1) {
    std::cout << t1 << std::endl;
}

int main()
{
    Interface_derived_1 interface_1;
    Interface_derived_2 interface_2;

    print(interface_1.value());
    print(interface_2.value());

    return 0;
}

