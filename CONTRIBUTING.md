# Style Guide

## General rules

* **Don't repeat yourself (DRY):** Avoid repeating code wherever possible by using variables to store data and functions for common routines
* **Document your code:** Methods and classes should be documented using the [Doxygen](https://www.stack.nl/~dimitri/doxygen/manual/commands.html) syntax
* **Minimise comments:** Good code is self-explanatory and shouldn't need many comments. Try using more descriptive names for variables and functions to keep the code easy to follow for other contributors
* **Avoid unnecessary includes:** Only include the headers that you need to write your code

## Formatting

* **Write in English:** All code and comments should be written in English
* **Use spaces instead of tabs:** Tabs should be set to 4 spaces (use [*Insert Spaces*](https://msdn.microsoft.com/en-us/library/aa265718(v=vs.60).aspx) in Visual Studio)
* **Functions are actions:** Function names should begin with a verb (e.g. `transformX()`)
* **Use descriptive names:** Names should be precise rather than short (e.g. `carLength` vs. `x`)
* **Use proper capitalisation:**
  * **Variables** and **functions** use camel case (e.g. `gainFactor` or `applyGain()`)
  * **Classes** start with a capital letter (e.g. `AudioPlugin`)
* **Always use curly brackets(`{}`):** Always include curly brackets in a loop, even if it's only one line long
* **Place curly brackets (`{}`) on a new line:** Curly brackets should go on the line after a function or loop declaration

## Version control

* **Describe your commits:** Tell other contributors what your commit changed in the commit message
* **Don't push without adding unit tests:** Make sure to write any unit tests necessary to cover new code before pushing your commits to master
* **Don't push if it doesn't compile:** Make sure the project builds correctly and all unit tests pass before pushing any commits to the master branch
