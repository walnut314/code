#include <iostream>
#include <memory>

class Interface {
private:
    class Implementation;
    std::unique_ptr<Implementation> m_pimpl;
public:
    Interface();
    int value() const;
};

class Interface::Implementation {
private:
    int m_a = 0;
public:
    void setValue(int a) { m_a = a; }
    int value() const { return m_a; }
};

Interface::Interface() : m_pimpl(
    std::make_unique<Interface::Implementation>())
{
    m_pimpl->setValue(10);
};

int Interface::value() const {
    return m_pimpl->value();
}

int main()
{
    Interface interface;
    std::cout << interface.value() << std::endl;
    return 0;
}

