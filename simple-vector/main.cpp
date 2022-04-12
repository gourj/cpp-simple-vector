#include "simple_vector.h"

// Tests
#include "tests.h"
#include <iostream>

int main() {
    Test1();
    std::cout << "Finished Test1()!" << std::endl;
    Test2();
    std::cout << "Finished Test2()!" << std::endl;
    TestReserveConstructor();
    TestReserveMethod();
    TestTemporaryObjConstructor();
    TestTemporaryObjOperator();
    TestNamedMoveConstructor();
    TestNamedMoveOperator();
    TestNoncopiableMoveConstructor();
    TestNoncopiablePushBack();
    TestNoncopiableInsert();
    TestNoncopiableErase();
    return 0;
}