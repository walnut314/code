#include <iostream>

class Handler {
public:
    virtual void apply(int number) const = 0;
};

class Handler1 : public Handler {
public:
    void apply(int number) const {
        std::cout << "Handler1 :" << 2*number << std::endl;
    }
};

class Handler2 : public Handler {
public:
    void apply(int number) const {
        std::cout << "Handler2 :" << number+1 << std::endl;
    }
};

class Object {
private:
    int m_number = 0;
    const Handler* m_state = 0;
public:
    Object(int number) : m_number(number) {}
    void set_state(const Handler* state) {
        m_state = state;
    }
    void execute() {
        m_state->apply(m_number);
    }
};

int main()
{
    Object object(3);
    object.set_state(new Handler1());
    object.execute();
    object.set_state(new Handler2());
    object.execute();
    return 0;
}

