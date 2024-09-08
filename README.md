# Virtual Hardware Microprogram

This project is a rewrite of a personal project I created a few months ago. The original version had some sloppy code, and I am now rewriting it to make the code more readable, modular, and maintainable.

## Overview

The project simulates a microprogram that calculates the sum of products for values stored in the A and B registers. Specifically, it multiplies the corresponding values from the A and B registers and sums the results consecutively (i.e., product of the first values from A and B, plus product of the second values from A and B, etc.). The final result is stored in memory.

## Features
- **Microprogram control**: Executes the sum of products for register values.
- **Customizable**: You can modify the microinstructions to experiment with other operations.
- **Memory Management**: A and B registers are set and their values are updated dynamically into memory.
- **Dynamic Linking**: The project requires OpenCV and Qt to be dynamically linked.

## Prerequisites
- **OpenCV**: Make sure you have OpenCV installed and dynamically linked.
- **Qt**: Ensure that Qt is installed and linked appropriately.

You'll need to modify the compilation settings to match your versions of OpenCV and Qt.

## Project Structure
- **src/**: Contains the source code for the project.
- **include/**: Contains the header files for the project.
- **archived/**: Contains files from the old version of the project. These files are used as references during the rewrite. Feel free to experiment with these if you'd like.

## How to Use
1. Clone the repository.
2. Install the necessary dependencies (OpenCV, Qt).
3. Set up the project in your preferred IDE (e.g., CLion, QtCreator) and link the required libraries.
4. Compile the project.
5. Run the executable. The microprogram will calculate the sum of products for the values in the A and B registers and store the result in memory.

## License
Feel free to use, modify, and experiment with this project for personal or educational purposes. The code in this project is provided "as is" without any warranties.
