# Kawa Programming Language

Kawa is a high-performance programming language that supports both compilation and interpretation. With its fast execution speed, it rivals that of C, and also includes web support.

## Key Features

- Compilation support 📚
- Interpretation support 💻
- Fast execution speed, similar to C 🚀
- Web support 🌐


## Usage

The Kawa compiler can be used to compile source code into an executable file. It can also be used in an interpreted mode for quicker development and testing.

To compile a Kawa program, run the following command:
```css
kawa [input file] -o [output file]
```

To run a Kawa program in interpreted mode, run the following command:
```css
kawa [input file]
```

## Syntax

Kawa is a type-based programming language. Here are some of the basic syntax rules of Kawa:

- Variables can be declared using the `mut` or `var` keyword, or they can be created without a keyword, similar to Python. For example: `x = 10` or `var x = 10`.
- Functions are declared using the `func` keyword, followed by the function name and a list of arguments enclosed in parentheses. For example: `func add(x, y) { return x + y }`.
- Conditional statements are created using the `if` keyword, followed by a condition and a block of code to be executed if the condition is true. For example: `if (x > 10) { ... }`.
- Loops are created using the `for` or `while` keywords, followed by a condition and a block of code to be executed repeatedly. For example: `for (var i = 0; i < 10; i++) { ... }`.

## ToDo 📝

- [x] Add more advanced features, such as error handling 💔
- [ ] Improve the web support to make it easier to use Kawa for web development 🌐💻
- [ ] Optimize the compiler and interpreter to make Kawa even faster 🚀
- [ ] Expand the documentation and provide more examples to help users get started 📚💡