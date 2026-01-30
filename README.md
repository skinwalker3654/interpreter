# Custom Script Language (CSL)

A minimal, readable scripting language designed for simplicity and educational use.

## Overview

This language is built to feel like readable pseudocode while still being structured and deterministic.  
It supports variables, printing, delays, loops, and conditionals with a very simple syntax.

## Features

- Variables with `SET`
- Console output with `PRINT` and `PRINTLN`
- Sleep/delay with `WAIT`
- For-loops with manual increment
- Conditional statements (`IF / ENDIF`)
- Minimal syntax, no brackets or semicolons

## Syntax

### Variables

```bash
SET number = 10;
SET name = "michael";
```


### Printing
```bash
PRINT "Hello ";
PRINTLN "World";
PRINTLN number;
```


### Delay

```bash
WAIT 2;
WAIT number;
```

### Folder Creation
```bash
MKDIR "folder name";
MKDIR variable_name;
```

### Loops

```bash
SET i = 1;
FOR i TO 10:
    PRINTLN i;
    i++;
ENDFOR
```


### Conditionals

```bash
IF number > 2:
    PRINT number;
    PRINTLN " is bigger than 2";
ENDIF
```

---

## Example Program

```bash
SET number = 10;
SET name = "michael";

PRINT "Name: ";
PRINTLN namel

PRINT "Number: ";
PRINTLN number;

PRINTLN "Waiting 2 seconds...";
WAIT 2;

PRINTLN "Waiting number seconds...";
WAIT number;

SET i = 1;
FOR i TO 10:
    PRINTLN i;
    i++;
ENDFOR

IF number > 2:
    PRINT number;
    PRINTLN " is bigger than 2";
ENDIF

PRINTLN "Creating the folders...";
WAIT 2;

MKDIR "FOLDER FOR PHOTOS";
MKDIR name;
```
---

## Build And Run
```bash
make all
```

## clean executables
```bash
make clean
```

## Run Script
```bash
make script
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
