#include <iostream>

class Object_A;
class Object_B;

class Visitor {
public:
    virtual void visit(Object_A* a) = 0;
    virtual void visit(Object_B* b) = 0;
};

class Object {
public:
    virtual void accept(Visitor *v) = 0;
};

class Object_A : public Object {
public:
    int m_number = 0;
    void accept(Visitor* v) {
        v->visit(this);
    }
};

class Object_B : public Object {
public:
    double m_d1 = 9;
    double m_d2 = 3;
    void accept(Visitor* v) {
        v->visit(this);
    }
};

class Visitor_Imp : public Visitor {
public:
    void visit(Object_A* a) {
        std::cout << "A: " << a->m_number << std::endl;
    }
    void visit(Object_B* b) {
        std::cout << "B: " << b->m_d1 << "-" << b->m_d2 << std::endl;
    }
};

int main()
{
    Object_A object_A;
    Object_B object_B;

    Visitor_Imp v;
    object_A.accept(&v);
    object_B.accept(&v);

    return 0;
}

