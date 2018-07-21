#include <iostream>
#include "stdio.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "GMockPlus.h"

using namespace std;

int testFunc(int a, int b) {
	return a + b;
}

int testFunc(int a, int b, int c) {
	return a + b + c;
}

int testFunc_Custom(int a, int b) {
	return a - b;
}

class ITest {
public:
	virtual int testMethod(int a, int b) = 0;
};

class MyCalss {
public:
	int testMethod(int a, int b) {
		return a * b;
	}
};

class CTest : public ITest {
public:
	int testMethod(int a, int b) {
		return a * b;
	}
};

class MyGMock {	// �����Ѿ�����Ҫ�ټ̳����κ�����
public:
	MOCK_METHOD2(testFunc, int(int, int));
	MOCK_METHOD2(testMethod, int(int, int));
	MOCK_METHOD2(test64bits, __int64(__int64, __int64));
};

TEST(FuncTest, testFunc_SetReturn) {

	MyGMock mockObj;
	int (*funcAddr)(int, int) = testFunc;
	GMOCKPLUS_CDECL(funcAddr, mockObj, &MyGMock::testFunc, 2);

	EXPECT_CALL(mockObj, testFunc(testing::Gt(0), testing::Gt(0)))
		.Times(2)
		.WillOnce(testing::Return(123))	// ��һ�ε��÷��� 123
		.WillOnce(testing::Invoke(testing::QueryNewSrcAddr(funcAddr)));	// �ڶ��ε���ֱ�ӵ���ԭ����ʵ��

	EXPECT_EQ(123, testFunc(1, 2));
	EXPECT_EQ(7, testFunc(3, 4));
}

typedef int (*testFuncPtr)(int, int);
TEST(FuncTest, testFunc_GetFuncAddrByName) {
	// ͨ������pdb����ȡ������ַ
	testing::SetSymSearchPath(L"./../");
	testing::OverloadFuncNameSet funcAddrSet;
	testing::QueryOverloadFuncNameSet("GMockPlusDemo.exe", "testFunc", funcAddrSet);

	testFuncPtr funcPtr = (testFuncPtr)testing::GetSymbolAddr("GMockPlusDemo.dll", "testFunc(int,int)");
	EXPECT_EQ(3, funcPtr(1, 2));
}

TEST(FuncTest, testFunc_ReplaceWithCustomFunc) {
	int (*funcAddr)(int, int) = testFunc;
	testing::GMockPlus objPtr(funcAddr, testFunc_Custom);	// ��Ҫ��֤ testFunc_Custom �ĺ���ԭ���� testFunc ԭ��һ��

	EXPECT_EQ(-1, testFunc(1, 2));
	EXPECT_EQ(-1, testFunc(3, 4));
}

TEST(MethodTest, testMethod_SetReturn) {
	MyGMock mockObj;

	GMOCKPLUS_THISCALL(&MyCalss::testMethod, mockObj, &MyGMock::testMethod, 2);

	MyCalss myC;
	EXPECT_CALL(mockObj, testMethod(testing::_, testing::_))
		.Times(testing::AtLeast(2))
		.WillOnce(testing::Return(111))	// ��һ�ε��÷��� 111
		.WillRepeatedly(testing::Invoke(&myC, testing::QueryNewSrcAddr(&MyCalss::testMethod)));	// ʣ�µ�ֱ�ӵ���ԭʵ��

	EXPECT_EQ(111, myC.testMethod(3, 4));
	EXPECT_EQ(2, myC.testMethod(1, 2));
	EXPECT_EQ(12, myC.testMethod(3, 4));
}

TEST(VirtualFunctionTest, testMethod_SetReturn) {
	MyGMock mockObj;
	CTest testObj;
	testObj.testMethod(1, 2);
	ITest* itest = &testObj;

	// ������麯����������ַ����ֱ��д &CTest::testMethod����Ҫ������ķ�ʽ
	// д &CTest::testMethod�õ��Ľ����Ǵ����ֵ
	int (CTest::*testMethodAddr)(int, int) = NULL;
	testing::GetSymbolAddr("GMockPlusDemo.dll", "CTest::testMethod(int,int)");
	__asm mov testMethodAddr, eax	// ���ܹ�ֱ�ӽ�void*����ת�ɳ�Ա��������Ҫ����������

		GMOCKPLUS_THISCALL(testMethodAddr, mockObj, &MyGMock::testMethod, 2);


	EXPECT_CALL(mockObj, testMethod(testing::_, testing::_))
		.Times(testing::AtLeast(2))
		.WillOnce(testing::Return(111))	// ��һ�ε��÷��� 111
		.WillRepeatedly(testing::Invoke(&testObj, testing::QueryNewSrcAddr(testMethodAddr)));	// ʣ�µ�ֱ�ӵ���ԭʵ��

	EXPECT_EQ(111, itest->testMethod(3, 4));
	EXPECT_EQ(2, itest->testMethod(1, 2));
	EXPECT_EQ(12, itest->testMethod(3, 4));
}

__int64 __stdcall test64bits(__int64 a, __int64 b) {
	return a + b;
}
TEST(Test64Bits, test64bits_SetReturn) {
	MyGMock mockObj;
	__int64 (__stdcall *funcAddr)(__int64, __int64) = test64bits;
	GMOCKPLUS_STDCALL(funcAddr, mockObj, &MyGMock::test64bits, 4);

	EXPECT_CALL(mockObj, test64bits(testing::Gt(0), testing::Gt(0)))
		.Times(2)
		.WillOnce(testing::Return(123))	// ��һ�ε��÷��� 123
		.WillOnce(testing::Invoke(testing::QueryNewSrcAddr(funcAddr)));	// �ڶ��ε���ֱ�ӵ���ԭ����ʵ��

	EXPECT_EQ(123, test64bits(1, 2));
	EXPECT_EQ(7, test64bits(3, 4));
}
