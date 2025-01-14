# ASCII Graphing Calculator

This is an ASCII graphing calculator application built using C++ and the ncurses library. It provides a simple graphing calculator application entirely in your terminal. What follows is a guide on the various features the application provides and how they can be used.

## Function Window

This is the window that appears when you open the application. This is the screen where you input the functions and expressions which will be graphed. Here is a list of the availible commands in this window:
- *select X*, where X is an integer in [1,99]. This command will take you to that line and along you to input your expression on that entry in the formula window. Press ESC to return to the command line.


## Expression Types

There are 4 types of expressions which the application allows users to define:
- Named functions of _ _x_ _ or y, eg. _ _f(x) = 2x + 3_ _ or _ _g(y) = 2y^2 - 1/y
- Un-named functions of _ _x_ _ or _ _y_ _, eg. _ _x = y^2 + 1_ _ or _ _y = x^(1/2)_ _. We note here that the left-hand side of the expression must only contain either _ _x_ _ or _ _y_ _, while the right-hand side is a general expression in terms of the other variable.
- Constants eg. _ _a = 5_ _ or _ _b = 5^2 - 11*4_ _. These will not be graphed but may be used in other expressions, such as with _ _f(x) = ax^2 + b_ _.
- Parameters, of the form T = [A,B], S. Here, T denotes the name of the parameter, A,B are numerical constants denoted the range of the parameter, and S is an optional argument which denotes the speed at which the parameter changes through that range. For example, t = [1,5],2

We note that all formulas can be defined in terms of other formulas, provided there are no circular defintions (which are detected and produce an error). The only excpetions to this are that constants and parameters cannot be defined in terms of parameters, as parameters are intended to vary. As well, un-named expressions can naturally not be used in the defintions of other formulas. Below are some examples of defining formulas in terms of other formulas:
- *a = f(3) + g(1) + 5*
- *f(x) = g(x^2) + h(x - 2) + 11*
- *t = [f(1), a], 2*
- *y = f(x - a) + t * g(1)*
