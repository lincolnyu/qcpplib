#include <iostream>
#include "qtl/scheme/pointers/bipointer.h"

using namespace std;
using namespace Qtl::Scheme::Pointers;

class Dog;

class Human : public BiPointedObject
{
public:
	int Id;

	BiPointer<Dog> Pet1;
	BiPointer<Dog> Pet2;
	BiPointer<Dog> Pet3;

private:
	void SetSources()
	{
		Pet1.SetSource(this);
		Pet2.SetSource(this);
		Pet3.SetSource(this);
	}

public:
	Human(int id)
	{
		SetSources();
		Id = id;
		cout << "Human " << Id << " generated" << endl;
	}

	~Human()
	{
		cout << "Human " << Id << " killed" << endl;
	}

public:
	void Speak()
	{
		cout << "hi" << endl;
	}
};

class Dog : public BiPointedObject
{
public:
	int Id;
	BiPointer<Human> Bite1;
	BiPointer<Human> Bite2;
	BiPointer<Human> Bite3;

private:
	void SetSources()
	{
		Bite1.SetSource(this);
		Bite2.SetSource(this);
		Bite3.SetSource(this);
	}

public:
	Dog(int id)
	{
		SetSources();
		Id = id;
		cout << "Dog " << Id << " generated" << endl;
	}

	~Dog()
	{
		cout << "Dog " << Id << " killed" << endl;
	}

public:
	void Bark()
	{
		cout << "woof" << endl;
	}
};

class BiPointerTestser 
{
public:
	static void TestSingleOnStack()
	{
		auto dog = BiNew<Dog>(1);
		dog->Bark();
		auto dogshadow = dog;
		dogshadow->Bark();
		dog = nullptr;
		cout << "end single on stack test" << endl;
	}

	static void TestSimpleLoop()
	{
		auto dog = BiNew<Dog>(1);
		auto human = BiNew<Human>(1);
		dog->Bite1 = human;
		human->Pet1 = dog;
		cout << "end of simple loop test" << endl;
	}
};




void TestBiPointer()
{
	//BiPointerTestser::TestSingleOnStack();
	BiPointerTestser::TestSimpleLoop();
}
