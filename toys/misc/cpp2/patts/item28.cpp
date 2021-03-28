#include <iostream>
#include <vector>
#include <string>

struct Memento {
    int m_number;
    Memento(int number) : m_number(number) {}
};

struct Records {
    std::vector<const Memento*> m_mementos;
    int m_pos = -1;
    void add(const Memento* memento) {
        if (m_pos + 1 == m_mementos.size()) {
            m_mementos.push_back(memento);++m_pos;
        } else {
            m_mementos[++m_pos] = memento;
            m_mementos.resize(m_pos + 1);
        }
    }
    const Memento* undo() {
        if (m_pos == 0)
            return m_mementos[0];
        return m_mementos[--m_pos];
    }
    const Memento* redo () {
        if (m_pos + 1 == m_mementos.size())
            return m_mementos.back();
        else
            return m_mementos[++m_pos];
    }
    void print() const {
        std::cout << "{";
        for (size_t i = 0; i < m_mementos.size(); ++i) {
            if (i > m_pos) std::cout << "(";
            std::cout << m_mementos[i]->m_number;
            if (i > m_pos) std::cout << ")";
            if (i + 1 != m_mementos.size()) std::cout << ", ";
        }
        std::cout << " }";
    }
};

class Object {
private:
    int m_number = 0;
    Records m_Records;
    Memento* create_Memento() const {
        return new Memento(m_number);
    }
    void set_Memento(const Memento* memento) {
        m_number = memento->m_number;
    }
public:
    Object() {
        m_Records.add(create_Memento());
    }
    void update(int num) {
        m_number += num;
        m_Records.add(create_Memento());
    }
    void undo() {
        set_Memento(m_Records.undo());
    }
    void redo() {
        set_Memento(m_Records.redo());
    }
    void print(const std::string& msg) {
        std::cout << msg;
        std::cout << m_number << "   // ";
        m_Records.print();
        std::cout << std::endl;
    }
};

int main()
{
    Object object;
    object.print("start  : ");
    for (int i = 1; i < 6; ++i) {
        object.update(1);
        object.print("update : ");
    }

    object.redo();
    object.print("redo   : ");
    object.undo();
    object.print("undo   : ");
    object.undo();
    object.print("undo   : ");
    object.redo();
    object.print("redo   : ");
    object.update(5);
    object.print("update : ");
    object.redo();
    object.print("redo   : ");
    object.undo();
    object.print("undo   : ");
    object.undo();
    object.print("undo   : ");
    object.undo();
    object.print("undo   : ");
    object.undo();
    object.print("undo   : ");
    object.undo();
    object.print("undo   : ");

    return 0;
}

