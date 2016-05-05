#include <bipointer>

using Qtl::Scheme::Pointers;

class Human : public BiPointedObject
{
public:
	BiPointer<Dog> Pet1;
	BiPointer<Dog> Pet2;
	BiPointer<Dog> Pet3;
}

class Dog : public BiPointedObject
{
public:
	BiPointer<Human> Bite1;
	BiPointer<Human> Bite2;
	BiPointer<Human> Bite3;
}


