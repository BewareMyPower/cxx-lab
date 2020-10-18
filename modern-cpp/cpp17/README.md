[toc]

## cpp17

C++17 的新语法/库，这里根据 **个人主观偏向** 进行重要性分类，分为 3 个优先级。

## 语言特性

### High

- [constexpr-lambda.cc](./language/constexpr-lambda.cc)：使用 `constexpr` 实现编译期 lambda

  增强了 `constexpr` 的表达力和 lambda 的泛用性，简单易用。可以结合 `static_assert` 在编译期做更多检查。
  
- [lambda-capture-this-by-value.cc](./language/lambda-capture-this-by-value.cc)：lambda 捕获 `*this` 表示拷贝一份当前对象

  非常棒的特性，以前想要延长当前对象的声明周期的话，得手写函数对象，直接用 lambda 表达式做不到（但是可以手动捕获类的一些字段，不太优雅）。

- 内联变量

  也是很棒的特性，能利用 `inline` 的 ODR（One Definition Rule）的同时，不用特意再额外写个函数来返回变量。最棒的就是不用再在 `.cc` 文件去定义类静态成员了，比如：

  ```c++
  struct Counter {
      int id = 0;
      Counter() : id(kNumTotal++)
      static inline std::atomic_int kNumTotal{0}; 
  };
  ```

- 嵌套命名空间

  以前为了区分嵌套命名空间的首尾，我的一个习惯是：

  ```c++
  namespace A {
    namespace B {
      namespace C {
      }  // namespace A::B::C
    }  // namespace A::B
  }  // namespace A
  ```

  但有时候，`A` 和 `A::B` 中其实只是作为区分的前缀，该文件可能只是 `A/B/C.cc`，并不会定义额外的命名空间成员，所以 C++17 里可以直接这么写了：

  ```c++
  namespace A::B::C {
  }  // namespace A::B::C
  ```

- [structured-binding.cc](./language/structured-binding.cc)：结构化绑定

  整个 C++17 中**我最喜欢的语言特性，没有之一。**

- if 语句初始化

  这个特性很 Golang。比如在原先需要调用大量 C API（返回错误码）的场合，很多人喜欢 C 风格地提前定义一个错误码，然后之后返回：

  ```c++
  int ret;
  ret = f1();
  if (ret != 0) { /* ... */ }
  ret = f2();
  if (ret != 0) { /* ... */ }
  ```

  但这种写法影响了可读性，以及从局部性原理的角度，我们应该在不使用某个变量的时候就避免它能访问到，但像下面这样写：

  ```c++
  {
    int ret = f1();
    if (ret != 0) { /* ... */ }
  }
  {
    int ret = f2();
    if (ret != 0) { /* ... */ }
  }
  ```

  又不太优雅。C++17 中就可以这么干了：

  ```c++
  if (int ret = f1(); ret != 0) { /* ... */ }
  if (int ret = f2(); ret != 0) { /* ... */ }
  ```

  这很 Golang。

  另外，也可以用到 `switch` 语句中，还能够结合 RAII，也就是类似 Java 7 引入的 try-with-resources 语法。

- fallthrough，nodiscard，maybe_unused 属性（attribute）：帮助编译器提示错误

  - fallthrough：用于 switch-case，提示编译器 case 语句不 break 的话，会继续执行下一个 case；
  - nodiscard：修饰函数时，若函数的返回值不被使用时，编译器会给出警告；修饰类时，若该类为函数的返回类型且对应函数的某处调用没取得其返回值，编译器会给出警告
  - maybe_unused：修饰函数的某个参数表示它可能不被使用，从而抑制某些警告

  补充一句，在语言标准支持之前，编译器比如 GCC 已经支持了 **属性（attribute）** 这个概念。它虽然不改变代码的正确性，但是可以增加警告信息以及代码本身的可读性，从工程的角度有胜于无，了解它们是很有必要的。

### Middle

- [deduction-class.cc](./language/deduction-class.cc)：针对类模板的类型推断

    感觉早就该有了，但是不同于函数模板，进行类型声明时往往还是需要关注模板类型，感觉从使用者的角度，大多数情况下还是显式声明类型更易读，也不会增加多少工作量。

- [auto-non-type-tmpl-param.cc](./language/auto-non-type-tmpl-param.cc)：使用 auto 声明非类型模板参数

    实现 `index_sequence` 这种编译期结构更简单吧，增强了模板推断能力。
    
- UTF-8 字符字面值：加个 `u8` 前缀就行，早就该有了，增加了 C++ 处理 UTF-8 字符串的能力。

### Low

- [auto-braced-list.cc](./language/auto-braced-list.cc)：对大括号初始化列表的推断规则改变

    似乎从 GCC 5 开始规则就不遵循旧标准了，估计 C++17 才正式在标准里修正，简直是委员会没想好的设计，新标准擦屁股。参考 [Why does auto x{3} deduce an initializer_list?](https://stackoverflow.com/questions/25612262/why-does-auto-x3-deduce-an-initializer-list)

- [folding-expressions.cc](./language/folding-expressions.cc)：折叠表达式

    用 `...` 来取代之前写递归实现编译期计算，但是语法比较诡异，主要方便模板库的作者。
    
- constexpr if：通过 `constexpr` 修饰 `if` 使得编译期计算更容易，主要还是方便模板库的作者。

- 大括号初始化枚举：枚举本身用得不是很多，这个特性也像是擦屁股，而不是多显著的改进。

### 总结

总的来说，个人观点，C++17 引入的语言特性最为出色的还是结构化绑定和 if 语句初始化，有不少特性是简化模板元编程的，但个人看法这东西比较鸡肋。所谓鸡肋，就是食之无味弃之可惜。模板元编程实现了编译期计算，尽最大可能地减少运行时的开销，但其编程的复杂度和换来的收益，其实不成正比。所以对于 `constexpr` 这种能顺手一加就能简化模板元编程代码的，我是认为很有意义的。

## 库特性

### High

- [optional.cc](./library/optional.cc)：`std::optional`，一般的函数返回值要么是正确值，要么是错误值，但是由于返回类型是固定的，所以往往用一个特殊值代表错误结果，比如 C API 中返回 `int` 作为错误码，用 -1 代表错误结果，由于 `int` 的范围足够大，大到能容纳几乎所有错误码，所以不会存在用尽的情况。但是如果返回一个 `std::string`，有时候用空字符串来代表错误返回结果会有点奇怪，因为空字符串不一定代表错误。所以此时要么用异常，要么用比较丑陋的 API，比如加一个返回类型表示是否错误。

  有了 `std::optional` 之后，可以在返回错误时，将错误信息写入日志，然后返回一个空的 `std::optional`。

- [string_view.cc](./library/string_view.cc)：`std::string_view`

  整个 C++17 中**我最喜欢的库特性，没有之一**。示例给出了一个简单的分割字符串的操作，由于返回的是视图（view），因此对应的字符串生命周期结束时，视图的周期也结束了。

- `std::filesystem`：终于不用手动去包装系统 API 了，虽然想吐槽下这类 API 其他语言基本上早就做了，但是，有胜于无。

- [splicing_maps.cc](./library/splicing_maps.cc)：对 map/set（包括 unordered）的剪切操作，也就是 `extract` 和 `merge` 方法，没有拷贝（堆分配）的开销，在此之前用户端去实现大概是要找到对应的节点并 `std::move` 后插入新的 map/set。

- 并行算法：比如传入额外的 `std::is_execution_policy<T>` 来设置 execution 策略，比如 `std::execution::par`，能加速常见的算法，比如 `find`/`sort`。屏蔽了底层细节，简单易用

### Middle

- [variant.cc](./library/variant.cc)：`std::variant`，说是类型安全的 `union`，实际上很多时候 `union` 是不去麻烦地调用 `reinterpret_cast` 的，一样需要小心使用，这时候也用不了 `std::variant`。不过从省空间的角度，还是有一定意义的。

### Low

- [any.cc](./library/any.cc)：`std::any`，可以容纳任意类型，并且能类型安全地转换，少量情况下可以取代 `void*`，比如以前 C 风格的代码中回调函数的签名会有一个 `void*`，然后需要用户传入时把 `T*` 转换成 `void*`，回调函数的实现时又会把 `void*` 转换回去。但真的，我能想到的使用场景很小。
- `std::invoke`：包装回调比较试用，比如 `std::thread` 的实现，有了 `std::invoke` 就更简单了。但是个人认为应用面还是比较窄，趋于非常非常基础的库的实现。
- `std::apply`：类似 `std::invoke`，只不过可以接受一个 tuple 作为参数列表。

### 总结

C++17 的新增库大体看下来都不错，但还是 `optional` 和 `string_view` 最吸引人。