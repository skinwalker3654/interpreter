# Custom Script Language (CSL)

A minimal, readable scripting language designed for simplicity and educational use.

## Overview

This language is built to feel like readable pseudocode while still being structured and deterministic.  
It supports variables, printing, delays, loops, and conditionals with a very simple syntax.

## Features

- Variables with `set`
- Console output with `PRINT` and `println`
- Sleep/delay with `wait`
- For-loops with manual increment
- Conditional statements (`if / endif`)
- Loop statements (`for / endfor`)

## Syntax

### Variables

```bash
set number = 10;
set name = "michael";
```

### Variable assigments
```bash
varname = "new message";
varname = 23;

varname++;
varname--;
```

### Printing
```bash
print "Hello ";
println "World";
println number;
```


### Delay

```bash
wait 2;
wait number;
```

### Folder Creation
```bash
mkdir "folder name";
mkdir variable_name;
```

### Clear terminal
```bash
clear;
```

### Loops

```bash
set i = 1;
for i to 10:
    println i;
    i++;
endfor
```


### Conditionals

```bash
if number > 2:
    print number;
    println " is bigger than 2";
endif
```

---

## Example Program

```bash
set number = 10;
set name = "michael";

print "Name: ";
println namel

print "Number: ";
println number;

println "Waiting 2 seconds...";
wait 2;

wait "Waiting number seconds...";
wait number;

set i = 1;
for i to 10:
    println i;
    i++;
endfor

if number > 2:
    print number;
    println " is bigger than 2";
endif

println "Creating the folders...";
wait 2;

mkdir "FOLDER FOR PHOTOS";
mkdir name;

number++;
name = "john";

println number;
println name;

clear;
wait 2;
```
---

## Build And Run
```bash
make all # compile the program
./main <filename> # execute scripts
```

## clean executables
```bash
make clean
```

## Run Script
```bash
make script # run the demo script
```
---

## Design Goals

- Human-readable syntax
- Beginner-friendly
- Easy to parse and interpret
- Minimal keywords and rules

## Roadmap

Planned features:

- Functions
- While loops
- Arrays / lists
- User input
- Math library
- Comments support
- more shell operations

# interpiter
