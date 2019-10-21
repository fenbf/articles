![IIFE for complex initialization of const variables in C++](https://1.bp.blogspot.com/-9WW-Z66fu78/WB-KKBbQ7SI/AAAAAAAACwI/JhwPEgx4L9sF18ypVHBUJSPSY-X4n7DXACLcB/s1600/iife.png)

What do you do when the code for a variable initialization is complicated? Do you move it to another method or write inside the current scope?

In this blog post, I'd like to present a trick that allows computing a value for a variable, even a const variable, with a compact notation.

<!-- more -->

[TOC]

**Last Update:** 21st October 2019

## Intro

I hope you're initializing most of variables as `const` (so that the code is more verbose, explicit, and also compiler can reason better about the code and optimize). 

For example, it's easy to write:

```cpp
const int myParam = inputParam * 10 + 5;
```

or even:

```
const int myParam = bCondition ? inputParam*2 : inputParam + 10;
```

But what about complex expressions? When we have to use several lines of code, or when the  `?` operator is not sufficient.

'It's easy' you say: you can wrap that initialization into a separate function.

While that's the right answer in most cases, I've noticed that in reality a lot of people writes code in the current scope. That forces you to stop using `const` and code is a bit uglier. 

You might see something like this:

```cpp
int myVariable = 0; // this should be const...

if (bFirstCondition)
    myVariable = bSecondCindition ? computeFunc(inputParam) : 0;
else
    myVariable = inputParam * 2;

// more code of the current function...
// and we assume 'myVariable` is const now
```

The code above computes `myVariable` which should be `const`. But since we cannot initialize it in one line, then the `const` modifier is dropped. 

I highly suggest wrapping such code into a separate method, but recently I've come across a new option.

I've got the idea from a great talk by [Jason Turner](https://twitter.com/lefticus) about ["Practical Performance Practices"](https://www.youtube.com/watch?v=lNnBExDoNSQ) where among various tips I've noticed [**"IIFE"**](https://en.wikipedia.org/wiki/Immediately-invoked_function_expression). 

The **IIFE** acronym stands for *"Immediately-invoked function expression"*. Thanks to lambda expression, it's now available in C++. We can use it for complex initialization of variables. 

How does it look like?

## IIFE

The main idea behind IIFE is to write a small lambda that computes the value:

```cpp
const auto var = [&] { 
    return /* some complex code here */; 
}(); // call it!
```

`var` is `const` even when you need several lines of code to initialize it!

The critical bit is to call the lambda at the end. Otherwise, you only define such a lambda.

The imaginary code from the previous section could be rewritten to:

```cpp
const int myVariable = [&] {
    if (bFirstContidion)
        return bSecondCondition ? computeFunc(inputParam) : 0;
    else
       return inputParam * 2;
}(); // call!

// more code of the current function...
```

We've enclosed the original code with a lambda.

The expression takes no parameters but captures the current scope by reference. Also, look at the end of the code - there's  `()`  - so we're invoking the function immediately.

Additionally, since this lambda takes no parameters, we can skip `()` in the declaration. Only `[]` is required at the beginning, since it's the *lambda-introducer* .

## Improving Readability of IIFE

One of the main concerns behind IIFE is readability. Sometimes it's not easy to see that `()` at the end.

How can we fix that?

Some people suggest declaring a lambda above the variable declaration and just calling it later:

```cpp
auto initialiser = [&] { 
    return /* some complex code here */; 
};
const auto var = initialiser(); // call it
```

The issue here is that you need to find a name for the initializer lambda, but I agree that's easy to read.

And another technique involves `std::invoke()` that is expressive and shows that we're calling something:

```cpp
const auto var = std::invoke([&] { 
    return /* some complex code here */; 
});
```

**Note**: `std::invoke()` is located in the `<functional>` header and it's available since C++17.

In the above example, you can see that we clearly express our intention, so it might be easier to read such code.

Now back to you:

Which method do you prefer?

* just calling `()` at the end of the anonymous lambda?
* giving a name to the lambda and calling it later?
* using `std::invoke()`
* something else?

Ok, but the previous examples were all super simple, and maybe even convoluted... is there a better and more practical example?

How about building a simple HTML string?

## Use Case of IIFE

Our task is to produce an HTML node for a link:

As input, you have two strings:  `URL` and `text` (might be empty).

The output: a new string:

`<a href="url">text</a>`

or

`<a href="url">url</a>` (when text is empty)

We can write a following function:

```cpp
void BuildStringTest(std::string link, std::string text) {
    std::string html;
    html = "<a href=\"" + link + "\">";
    if (!text.empty())
        html += text;
    else
        html += link;
    html += "</a>";
    
    std::cout << html << '\n';
}
```

The code is long, and we can compact it:

```cpp
void BuildStringTest2(std::string link, std::string text) {
    std::string html;
    const auto& inText = text.empty() ? link : text;
    html = "<a href=\"" + link + "\">" + inText + "</a>";
    
    std::cout << html << '\n';
}
```

Ideally, we'd like to have `html` as `const`, so we can rewrite it as:

```cpp
void BuildStringTestIIFE(std::string link, std::string text) {
    const std::string html = [&] {
        std::string out = "<a href=\"" + link + "\">";
        if (!text.empty())
            out += text;
        else
            out += link;
        out += "</a>"; 
        return out;
    }(); // call ()!
    
    std::cout << html << '\n';
}
```

Or with a more compact code:

```cpp
void BuildStringTestIIFE2(std::string link, std::string text) {
    const std::string html = [&] {
        const auto& inText = text.empty() ? link : text;
        return "<a href=\"" + link + "\">" + inText + "</a>";
    }(); // call!
    
    std::cout << html << '\n';
}
```

Do you think that's acceptable? 

Try rewriting the example below (todo, techio)

http://coliru.stacked-crooked.com/a/bff9680b88c72d91

## Benchmark of IIFE

With IIFE, we not only get a  clean way to initialize `const` variables, but since we have more `const` objects, we might get better performance.  

Is that true? Or maybe longer code and creation of lambda makes things slower?

For the HTML example, I wrote a benchmark that tests all four version:

http://quick-bench.com/_DCQLNBmbXH50IJLnVRqXjJ7jlY

And it looks like we're getting 10% with IIFE!

![IIFE C++ Benchmark](https://4.bp.blogspot.com/-jeVzCwfF3wg/XaqVRQbUQQI/AAAAAAAAD-w/eo7RfqxUxCUsn6e6ppGt1sa-1PJ4p5uRgCLcBGAsYHQ/s1600/iifebench.png)
Some notes:

* This code shows the rough impact of the IIFE technique, but it was not written to get the super-fast performance. We're manipulating string here so many factors can affect the final result.
* it seems that if you have less temporary variables, the code runs faster (so `StringBuild` is slightly faster than `StringBuild2` and similarly IIFE and IIFE2)
* We can also use `string::reserve` to preallocate memory, so that each new string addition won't cause reallocation.

You can check other tests here: http://quick-bench.com/9nCgWpht-ZUOlI6sTBiQET70RZY

But as a perf summary, it looks like IIFE doesn't harm your code. 

## Summary

Would you use such a thing in your code?

In C++ Coding Guideline we have a suggestion that it's viable to use it for complex init code:

[C++ Core Guidelines - ES.28: Use lambdas for complex initialization,](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-lambda-init)

I am a bit sceptical to such expression, but I probably need to get used to it. I wouldn't use it for a long code. It's perhaps better to wrap some long code into a separate method and give it a proper name. But if the code is 2 or three lines long... maybe why not.

I want to thank [Mariusz Jaskółka](https://jm4r.github.io/about/) for the hints about compacting the code and also perf improvements with `reserve()`.

## Your turn

* What do you think about such syntax? Have you used it in your projects?
* Do you have any guidelines about such thing?
* Is such expression better than having lots of small functions?

BTW: maybe I should ask Java Script guys since this concept comes from their world mostly :)

## References

* [Herb Sutter Blog: Complex initialization for a const variable](https://herbsutter.com/2013/04/05/complex-initialization-for-a-const-variable/)
* [C++ Weekly - Ep 32 - Lambdas For Free](https://www.youtube.com/watch?v=_CbBfuQQQI8)
* [Complex Object Initialization Optimization with IIFE in C++11](http://articles.emptycrate.com/2014/12/16/complex_object_initialization_optimization_with_iife_in_c11.html) - from Jason Turner's Blog
* [C++ IIFE in quick-bench.com](https://youtu.be/d4nmNYTM1j8)
* [C++ Core Guidelines - ES.28: Use lambdas for complex initialization,](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-lambda-init)
