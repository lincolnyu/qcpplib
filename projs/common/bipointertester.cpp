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
		cout << "wof" << endl;
	}
};

class BiPointerTestser 
{
public:
	static void TestSingleOnStack()
	{
		BiPointer<Dog> dog = new Dog(1);
		dog->Bark();
		//dog = nullptr;
		cout << "end of test" << endl;
	}
};


void TestBiPointer()
{
	BiPointerTestser::TestSingleOnStack();
}
