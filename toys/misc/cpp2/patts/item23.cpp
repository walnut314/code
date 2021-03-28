#include <iostream>

class TemplateMethod {
private:
    virtual void operation1() const {
        std::cout << "operation1 base" << std::endl;
    }
    virtual void operation2() const {
        std::cout << "operation2 base" << std::endl;
    }
public:
    void apply() const {
        operation1();
        operation2();
    }
};

class TemplateMethod_A : public TemplateMethod {
private:
    virtual void operation2() const override {
        std::cout << "operation2 Impl A" << std::endl;
    }
};

class TemplateMethod_B : public TemplateMethod {
private:
    virtual void operation1() const override {
        std::cout << "operation2 Impl B" << std::endl;
    }
};

int main()
{
    TemplateMethod_A method_A;
    TemplateMethod_B method_B;

    method_A.apply();
    method_B.apply();

    return 0;
}

