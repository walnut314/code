#include <iostream>
#include <string>
#include <vector>

struct Interface {
    virtual void show(const std::string& end="\n") const = 0;
};

class Single : public Interface {
private:
    int m_a;
public:
    Single(int a=0) : m_a(a) {}
    void show(const std::string& endl="\t") const override {
        std::cout << m_a << endl;
    }
};

class Collection : public Interface {
private:
    std::vector<Interface*> m_vec;
public:
    void add(const int& a) {
        m_vec.emplace_back(new Single(a));
    }
    void show(const std::string& endl="\t") const override {
        for (const Interface* single: m_vec)
            single->show(",");
        std::cout << endl;
    }
};

int main()
{
    Single single(4);
    single.show();
    Collection collection;
    collection.add(2);
    collection.add(4);
    collection.add(1);
    collection.show();
    return 0;
}

