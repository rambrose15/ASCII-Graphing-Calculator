# ASCII Graphing Calculator

This is an ASCII graphing calculator application built using C++ and the ncurses library. It provides a simple graphing calculator application entirely in your terminal. What follows is a guide on the various features the application provides and how they can be used.

![Graphing Calculator Demo Recording](https://github.com/user-attachments/assets/bfdb3265-a871-420c-a6c6-37716c0ca365)

## Function Window

This is the window that appears when you open the application. You can use this to input the functions and expressions that will be graphed. Here is a list of the available commands in this window:
- *select X*, where X is an integer in [1,99]. This command will take you to that line and allow you to input your expression on that entry in the formula window. Press ESC to return to the command line.
- *clear* Clears the current set of formulas
- *saveas X* where *X* is a filename. This will save your formulas to the file you specified.
- *load X* where *X* is a filename. This will load the formulas stored in X, assuming they follow the format used by the save functions.
- *save* This will save your formulas to the most recent filename given in a saveas or load command. It will throw an error if used before any saveas or load commands have been executed.


## Expression Types

There are 4 types of expressions which the application allows users to define:
- Named functions of _ _x_ _ or y, e.g. _ _f(x) = 2x + 3_ _ or _ _g(y) = 2y^2 - 1/y
- Un-named functions of _ _x_ _ or _ _y_ _, e.g. _ _x = y^2 + 1_ _ or _ _y = x^(1/2)_ _. We note here that the left-hand side of the expression must only contain either _ _x_ _ or _ _y_ _, while the right-hand side is a general expression in terms of the other variable.
- Constants e.g. _ _a = 5_ _ or _ _b = 5^2 - 11*4_ _. These will not be graphed but may be used in other expressions, such as with _ _f(x) = ax^2 + b_ _.
- Parameters, of the form T = [A,B], S. Here, T denotes the name of the parameter, A,B are numerical constants denoted the range of the parameter, and S is an optional argument which denotes the speed at which the parameter changes through that range. For example, t = [1,5],2

We note that all formulas can be defined in terms of other formulas, provided there are no circular definitions (which are detected and produce an error). The only exceptions to this are that constants and parameters cannot be defined in terms of parameters, as parameters are intended to vary. As well, un-named expressions can naturally not be used in the definitions of other formulas. Below are some examples of defining formulas in terms of other formulas:
- *a = f(3) + g(1) + 5*
- *f(x) = g(x^2) + h(x - 2) + 11*
- *t = [f(1), a], 2*
- *y = f(x - a) + t * g(1)*

## Graphing Window

This is the window that is opened when the *swap* command is run in the formula window. This is where the formulas defined in the formula window are graphed using ASCII characters. Here is a list of commands you can use in this window to interact with the graphs:
- *hide F* hides the function *F*, effectively changing its colour to none.
- *show F* reveals *F* if *F* has previously been hidden by the above *hide* command.
- *trace fn* where *fn* is either the name of a named function or its index in the formula window. This allows you to move a cursor across the graph of the specified function using the arrow keys. As you move, the (x,y) value of your cursor is displayed.
- *vline X* where *X* is a number. Creates a vertical line at x-coordinate *X* on the graph.
- *hline Y* where *Y* is a number. Creates a horizontal line at y-coordinate *Y* on the graph.
- *removelines* Removes all lines created by the *vline* and *hline* commands
- *zi X* Zoom in on the graph. *X* is an optional parameter to specify the zoom, default is 2. The point you zoom in on is determined by moving the cursor with the arrow keys.
- *zo X* Same as *zi* but zooms out instead of in.
- *play PARAMS* where *PARAMS* is a sequence of parameter names which have been defined in the formula window. This causes the specified parameters to vary, meaning graphs which depend on them will also vary in real time.
- *stop PARAMS* stops the specified parameters from varying. If no parameters are specified, all are stopped.
- *reset PARAMS* Resets the specified parameters to their default values. If no parameters are specified, all are stopped.
- *seeparam P* displays the value of the parameter *P*. If *P* is in the play state, the value will update continuously.
- *setparam P V* sets the parameter *P* to have value *V*. *V* must be inside of *P*'s defined range, otherwise the command will fail.
- *eval F V* evaluates the defined function *F* at value *V* and displays the result.
