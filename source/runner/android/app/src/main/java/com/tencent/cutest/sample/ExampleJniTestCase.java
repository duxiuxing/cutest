
package com.tencent.cppunit.sample;

public class ExampleJniTestCase {
    public ExampleJniTestCase() {
    }

    public void example() {
        isZero(0);
    }

    public void anotherExample() {
        isZero(100);
    }

    native void isZero(int value);
}
