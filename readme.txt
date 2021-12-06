UCF COP3330 Fall 2021 Assignment 6 Solution
Copyright 2021 Jesse Johnson

******** calculator08buggy project ********

- To compile: 
    g++ calculator08buggy.cpp
    ./a.out

- Ignore warnings as std_lib_facilities.h uses alias declarations that are a C++11 extension

- This calculator allows the following properties
    - Operators: (, ), +, -, *, /, %
    - Declarations: let
    - Constants: const
    - Integers & Decimal numbers

- This calculator functions as a normal calculator. When given the prompt ">", enter the function
  you want to solve, input ";", then press enter.

ex1.
    > 5 + 4;
    = 9
ex2.
    > let y = -5.5;
    = -5.5
    > y * 2;
    = -11;
ex3.
    > const _odd = 39;
    = 39
    > const ev3n = 4;
    = 4
    > _odd + ev3n;
    = 43
