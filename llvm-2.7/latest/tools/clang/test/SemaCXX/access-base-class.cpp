// RUN: %clang_cc1 -fsyntax-only -faccess-control -verify %s
namespace T1 {
  
class A { };
class B : private A { }; // expected-note {{declared private here}}

void f(B* b) {
  A *a = b; // expected-error{{cannot cast 'class T1::B' to its private base class 'class T1::A'}}
}

}

namespace T2 { 

class A { };
class B : A { }; // expected-note {{implicitly declared private here}}

void f(B* b) {
  A *a = b; // expected-error {{cannot cast 'class T2::B' to its private base class 'class T2::A'}}
}

}

namespace T3 {

class A { };
class B : public A { }; 

void f(B* b) {
  A *a = b;
}

}

namespace T4 {

class A {};

class B : private virtual A {};
class C : public virtual A {};

class D : public B, public C {};

void f(D *d) {
  // This takes the D->C->B->A path.
  A *a = d;
}

}

namespace T5 {
  class A {};
    
  class B : private A {
    void f(B *b) {
      A *a = b;
    }
  };    
}

namespace T6 {
  class C;
  
  class A {};
  
  class B : private A { // expected-note {{declared private here}}
    void f(C* c);
  };
  
  class C : public B { 
    void f(C *c) {
      A* a = c; // expected-error {{cannot cast 'class T6::C' to its private base class 'class T6::A'}}
    }
  };
  
  void B::f(C *c) {
    A *a = c;
  }
}

namespace T7 {
  class A {};
  class B : public A {};
  class C : private B { 
    void f(C *c) {
      A* a = c; // okay
    }
  };
}

