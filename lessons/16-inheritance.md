# 第 16 课:继承与多态

## 这一课你会学到

- 继承:在已有的类上扩展出新类
- 虚函数:同一个调用,不同对象有不同表现
- 为什么基类的析构函数要写成 `virtual`
- 用智能指针管理不同种类的对象

## 1. 继承:新类拿来旧类的东西

动物有名字,会叫。狗是动物,猫也是动物,它们都有名字,但叫声不同。如果每个类都把"名字"重写一遍,很重复。

**继承**让新类直接拥有已有类的成员。被继承的叫**基类**,新写的叫**派生类**。

```cpp
// file: inherit_basic.cpp
#include <iostream>
#include <string>

class Animal {
public:
    Animal(std::string n) : name(n) {}

    void show_name() const {
        std::cout << "我叫 " << name << std::endl;
    }

protected:
    std::string name;
};

class Dog : public Animal {
public:
    Dog(std::string n) : Animal(n) {}

    void bark() const {
        std::cout << name << ":汪汪" << std::endl;
    }
};

int main() {
    Dog d("旺财");
    d.show_name();
    d.bark();
    return 0;
}
```

运行结果:

```text
我叫 旺财
旺财:汪汪
```

逐处看:

- `class Dog : public Animal` 表示 `Dog` 继承自 `Animal`。`public` 先照写,不用深究。
- `Dog` 没有写 `show_name`,但可以直接调用,因为是从 `Animal` 继承来的。
- `protected` 是介于公开和私有之间的权限:外面不能用,但派生类可以用。所以 `Dog` 的 `bark` 里能直接用 `name`。如果 `name` 是 `private`,派生类也碰不到。
- `Dog(std::string n) : Animal(n) {}`:创建狗时,先要创建它里面的动物部分。冒号后面的 `Animal(n)` 就是调用基类的构造函数。

## 2. 问题:想让同一个调用,有不同表现

假设动物都会叫,但狗是"汪汪",猫是"喵喵"。我们希望写一个函数,不管传进来的是狗还是猫,调用 `speak()`,就各叫各的。

先试试普通写法:

```cpp
// file: no_virtual.cpp
#include <iostream>

class Animal {
public:
    void speak() const {
        std::cout << "动物叫" << std::endl;
    }
};

class Dog : public Animal {
public:
    void speak() const {
        std::cout << "汪汪" << std::endl;
    }
};

void talk(const Animal& a) {
    a.speak();
}

int main() {
    Dog d;
    talk(d);
    return 0;
}
```

运行结果:

```text
动物叫
```

我们传的是狗,却输出"动物叫"。原因:`talk` 的参数是 `Animal` 的引用,编译器只看到了类型 `Animal`,就直接调用了 `Animal` 的 `speak`。

## 3. 虚函数:运行时看真实身份

在基类的函数前加 `virtual`,告诉编译器:"这个函数在运行时,要看对象真正是谁,再决定调用哪个版本。"

```cpp
// file: virtual_demo.cpp
#include <iostream>

class Animal {
public:
    virtual void speak() const {
        std::cout << "动物叫" << std::endl;
    }
};

class Dog : public Animal {
public:
    void speak() const override {
        std::cout << "汪汪" << std::endl;
    }
};

class Cat : public Animal {
public:
    void speak() const override {
        std::cout << "喵喵" << std::endl;
    }
};

void talk(const Animal& a) {
    a.speak();
}

int main() {
    Dog d;
    Cat c;
    talk(d);
    talk(c);
    return 0;
}
```

运行结果:

```text
汪汪
喵喵
```

同一个函数 `talk`,传狗叫汪汪,传猫叫喵喵。这就是**多态**:同一个调用,根据对象的真实类型,表现不同。

派生类里的 `override` 表示"我是在改写基类的虚函数"。它不是必须写的,但写了以后,如果你把函数名或参数写错了,编译器会报错提醒你,不会悄悄变成另一个新函数。**建议总是写上。**

## 4. 基类的析构函数要写成 virtual

多态常常搭配堆内存和指针使用:用基类的指针,指着派生类的对象。这时有一个容易忽略的陷阱。

<!-- expect: crash
flags: -fsanitize=address -g -->
```cpp
// file: no_virtual_dtor.cpp
#include <iostream>

class Animal {
public:
    ~Animal() { std::cout << "Animal 销毁" << std::endl; }
};

class Dog : public Animal {
public:
    Dog() : tag(new int(1)) {}
    ~Dog() {
        delete tag;
        std::cout << "Dog 销毁" << std::endl;
    }

private:
    int* tag;
};

int main() {
    Animal* p = new Dog;
    delete p;
    return 0;
}
```

这里 `p` 的类型是 `Animal*`,但指着的是一只 `Dog`。`delete p` 时,因为 `Animal` 的析构函数不是虚函数,编译器只看类型,只调用 `Animal` 的析构函数。`Dog` 的析构函数没有运行,`tag` 指向的堆内存就泄漏了。

输出只有:

```text
Animal 销毁
```

没有"Dog 销毁"。用 AddressSanitizer 运行,它会报 `new-delete-type-mismatch`,意思是:申请时是 8 字节的 `Dog`,归还时却按 1 字节的 `Animal` 来处理,两边对不上。如果用 valgrind 检查,还会看到 `tag` 指向的那块内存没有归还(泄漏)。

修复办法:基类的析构函数加 `virtual`。

```cpp
class Animal {
public:
    virtual ~Animal() { std::cout << "Animal 销毁" << std::endl; }
};
```

这样 `delete p` 会先调用 `Dog` 的析构函数,再调用 `Animal` 的,输出:

```text
Dog 销毁
Animal 销毁
```

**规则:只要一个类里有虚函数,或者打算被继承,就把它的析构函数写成 `virtual`。**

## 5. 用智能指针管理多种对象

第 13 课的结论是:尽量不要手写 `new` 和 `delete`。多态也一样。把不同种类的动物放进同一个容器时,用 `unique_ptr`:

```cpp
// file: zoo.cpp
#include <iostream>
#include <memory>
#include <vector>

class Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() const = 0;
};

class Dog : public Animal {
public:
    void speak() const override { std::cout << "汪汪" << std::endl; }
};

class Cat : public Animal {
public:
    void speak() const override { std::cout << "喵喵" << std::endl; }
};

int main() {
    std::vector<std::unique_ptr<Animal>> zoo;
    zoo.push_back(std::make_unique<Dog>());
    zoo.push_back(std::make_unique<Cat>());
    zoo.push_back(std::make_unique<Dog>());

    for (const auto& a : zoo) {
        a->speak();
    }
    return 0;
}
```

运行结果:

```text
汪汪
喵喵
汪汪
```

几个新写法:

- `virtual void speak() const = 0;`:末尾的 `= 0` 表示**纯虚函数**,基类不提供实现,只规定"所有动物都必须会叫"。有纯虚函数的类叫**抽象类**,不能直接创建对象,只能被继承。这很合理:"一只纯粹的动物"在现实中不存在。
- `virtual ~Animal() = default;`:析构函数写成虚函数,`= default` 表示用编译器自动生成的版本,不用自己写内容。
- `a->speak()`:用指针调用成员函数,写箭头 `->`。它等同于 `(*a).speak()`。
- `const auto& a`:`auto` 让编译器猜类型(第 2 课提过)。这里写完整类型太长,所以用 `auto`。`const &` 是只读引用,因为 `unique_ptr` 不能复制,所以遍历时必须用引用。

`zoo` 销毁时,里面每个 `unique_ptr` 销毁,各自归还堆内存。因为析构函数是虚函数,`Dog` 和 `Cat` 都会被正确销毁。

## 练习

1. 在 `zoo.cpp` 里再添加一个 `Bird` 类,叫"啾啾",放进 `zoo` 里。
2. 给 `Animal` 增加一个纯虚函数 `std::string name() const`,让每个派生类返回自己的名字,遍历时一起输出。
3. 把 `zoo.cpp` 里的 `virtual ~Animal() = default;` 改成普通析构函数,用 AddressSanitizer 看看会不会报错。注意:这个程序里 `Dog` 和 `Cat` 没有自己的堆成员,所以可能看不出问题。思考:为什么在第 4 节的例子里会泄漏?

## 小结

- 继承:`class Dog : public Animal`,派生类拥有基类的成员。
- `protected`:外面不能用,派生类可以用。
- `virtual` 让函数在运行时按对象的真实类型调用,这就是多态。
- 派生类改写虚函数时,加上 `override`。
- 有纯虚函数(`= 0`)的类是抽象类,不能直接创建对象。
- 会被继承的类,析构函数要写成 `virtual`。
- 多态对象放进容器时,用 `unique_ptr` 管理。
