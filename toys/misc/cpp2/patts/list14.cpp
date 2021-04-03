#include <memory>
#include <iostream>

class Simple {
private:
    int variable{0};
public:
    Simple() {
        std::cout << "constructed" << std::endl;
    } ~Simple() {
        std::cout << "destructed" << std::endl;
    }
};

int main()
{
    using UniqueSimplePtr = std::unique_ptr<Simple>;
    UniqueSimplePtr pSimple1{ new Simple() };
    // gets memory address
    std::cout << "addr p1: " << pSimple1.get() << std::endl;

    UniqueSimplePtr pSimple2;
    pSimple2.swap(pSimple1);
    // p1 should be vacated by move since unique
    std::cout << "addr p1: " << pSimple1.get() << std::endl;
    std::cout << "addr p2: " << pSimple2.get() << std::endl;
    std::cout << std::endl;

    using IntSharedPtr = std::shared_ptr<int>;
    IntSharedPtr pIntArray1{ new int[16] };
    IntSharedPtr pIntArray2{ pIntArray1 };

    std::cout << "addr s1: " << pIntArray1.get() << std::endl;
    std::cout << "addr s2: " << pIntArray2.get() << std::endl;

    return 0;
}



