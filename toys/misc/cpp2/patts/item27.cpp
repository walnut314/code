#include <iostream>

struct Memento {
    int m_number;
    Memento(int number) : m_number(number) {}
};

class Object {
private:
    int m_number = 0;
public:
    int get() const { return m_number; }
    void update() { ++m_number; }
    Memento* create_Memento() const {
        return new Memento(m_number);
    }
    void set_Memento(const Memento* memento) {
        m_number = memento->m_number;
    }
};

int main()
{
    Object object;
    object.update();

    Memento* memento1 = object.create_Memento();
    std::cout << "memento at: " << object.get() << std::endl;
    object.update();
    object.update();
    std::cout << "before set: " << object.get() << std::endl;
    object.set_Memento(memento1);
    std::cout << "after set: " << object.get() << std::endl;
    
    return 0;
}

