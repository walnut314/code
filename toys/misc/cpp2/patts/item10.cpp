#include <iostream>
#include <memory>

class Interface_Not_Good {
public:
    int* m_a;
    void set(int a) { m_a = new int(a); }
};

class Interface_Good {
private:
    Interface_Not_Good m_Not_Good;
public:
    Interface_Good(int a) {
        // either one works
        //m_Not_Good.m_a = new int(a);
        m_Not_Good.set(a);
    }
    int get() const { return *m_Not_Good.m_a; }
    ~Interface_Good() { delete m_Not_Good.m_a; }
};

int main()
{
    Interface_Good interface_Good(3);
    std::cout << interface_Good.get() << std::endl;
}

