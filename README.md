# Virtual Hardware Microprogram
🚨 **The rewrite of this project is not finished. If you wish to use it with Qt and OpenCV simulation, please use the archived code from the old project in the `archived` folder.** 🚨
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

## Explanation of `MIP` Struct Signals

The `MIP` struct contains a set of signals that control the flow of data and the execution of the microprogram. Each signal has a specific role in managing memory, the ALU, registers, and branching behavior.

#### A-bus and ALU Control Signals
- **Al**: Latch control for `a_reg`. When set, the result from the ALU (`C`) is latched into `a_reg`.
- **Ao**: Selects `a_reg` as the input for the A-side of the ALU.
- **A1o**: Selects constant `1` as the input for the A-side of the ALU.
- **A0o**: Selects constant `0` as the input for the A-side of the ALU.
- **MA1i**: Selects the output of the ALU (C) or the emit field as the input for `ma1_reg`.
- **MA1l**: Latch control for `ma1_reg`. When set, stores the value from the selected input.
- **A1I**: Increments `ma1_reg` by 1 when set (used for loop-like behavior).
- **MA1o**: Selects `ma1_reg` as the input for the A-side of the ALU.

#### B-bus and ALU Control Signals
- **Byo**: Selects the output of the ALU (C) as the input for the A-side of the ALU.
- **MD1i**: Determines whether to use the emit field or ALU output (C) as the input for the memory data register `md1_reg`.
- **MD1l**: Latch control for `md1_reg`, determining when data from memory or another source is stored in `md1_reg`.
- **BMD1o**: Selects `md1_reg` as the input for the B-side of the ALU.
- **MD2i**: Determines whether the emit field, ALU output, or another source will be used for `md2_reg`.
- **MD2l**: Latch control for `md2_reg`. Stores a value into `md2_reg` when set.
- **MA2l**: Latch control for `ma2_reg`. Stores the selected input value when set.
- **A2I**: Increments `ma2_reg` by 1 when set.
- **BMPYo**: Selects the output of the multiplication unit (`D`) as the input for the B-side of the ALU.

#### Pipeline and Loop Control
- **Lpl**: Latch control for the loop register (`loop_reg`). Stores the `emit` field value into the loop register.
- **Lpc**: Decrements the `loop_reg` by 1, used for managing loops or repeated instructions.

#### Memory Control
- **r_w**: Read/write flag. `1` for memory read, `0` for memory write.
- **memo**: Selects the memory data output, helping to manage data bus conflicts.
- **MMD1o**: Memory Data Output 1. When cleared (`0`), selects `md1_reg` for writing to memory.
- **MMD2o**: Memory Data Output 2. When cleared (`0`), selects `md2_reg` for writing to memory.

#### ALU and Arithmetic Control
- **alu**: ALU operation control, which determines the operation (addition, subtraction, etc.) to be performed by the ALU.
- **Deo**: Data emission output control, used to emit data from the ALU or other sources into specific registers.

#### Condition Flags and Control
- **Beo**: Emit control for the B-side of the ALU, which can use the emit field as the input for `B`.
- **ovf**: Overflow flag. Set when the result of an arithmetic operation in the ALU has overflowed.
- **zf**: Zero flag. Set when the result of an ALU operation is zero.
- **mf**: Minus flag. Set when the result of an ALU operation is negative.

#### Branch Control and Execution
- **ncld**: No-Condition Load, used for unconditional jumps or branches.
- **stp**: Stop flag, halts the execution of the microprogram when set.
- **emit**: Emit field, a value that can be emitted to registers, memory, or ALU inputs. Acts as an immediate data value for various control operations.

### Microprogram Instruction Explanation

The following microprogram instructions are defined in the `MIP mip[]` array, which controls the operations of the processor. Each line in the program represents a step in the microprogram's execution. Below is a detailed breakdown of the signals and the actions taken during each step, as well as the purpose of each step in the overall program.

#### Instruction Breakdown:

1. **PC:1 → `MA1_reg` ← emit(-1)**
    - The `MA1_reg` is loaded with the emitted value `-1`. This sets up the memory address for the upcoming memory operations.
    - **Purpose**: Initialize the memory address register (`MA1_reg`) with `-1` to prepare for subsequent memory operations.

2. **PC:2 → `MA2_reg` ← emit(7)**
    - The `MA2_reg` is loaded with the emitted value `7`. This is another address register setup.
    - **Purpose**: Set the second memory address register (`MA2_reg`) to the value `7`.

3. **PC:3 → `loop_reg` ← emit(8)**
    - The `loop_reg` is loaded with the value `8`.
    - **Purpose**: Initialize the loop register for an iteration that will repeat for `8` cycles.

4. **PC:4 → ALU ← `A_reg + emit(0)`**
    - The ALU (Arithmetic Logic Unit) computes the sum of the `A_reg` and an emitted value of `0`.
    - **Purpose**: Perform a simple addition using the ALU. This can act as a placeholder or setup for further ALU operations.

5. **PC:5 → `MD1_reg` ← ALU**
    - The result from the ALU is stored in `MD1_reg` (Memory Data Register 1).
    - **Purpose**: Store the ALU result into the first memory data register (`MD1_reg`).

6. **PC:6 → `MA1_reg` ← emit(63)**
    - The `MA1_reg` is updated with the emitted value `63`.
    - **Purpose**: Set the memory address register (`MA1_reg`) to point to memory address `63`.

7. **PC:7 → `mem[MA1_reg]` ← `MD1_reg`**
    - The contents of `MD1_reg` are stored in memory at the address contained in `MA1_reg`.
    - **Purpose**: Write the data in `MD1_reg` to the memory address `63`.

---

#### Summary of Operations:

- **MA1_reg**: Holds the memory address for the first operand or result.
- **MA2_reg**: Holds the memory address for the second operand or result.
- **MD1_reg**: Stores data that will be written into memory.
- **ALU**: Performs arithmetic or logical operations.
- **Loop Register**: Controls the repetition of operations over a specified number of cycles.

This microprogram is set up to calculate the sum of products of `a` and `b` register values and store the result at memory address `63`.


## How to Use
1. Clone the repository.
2. Install the necessary dependencies (OpenCV, Qt).
3. Set up the project in your preferred IDE (e.g., CLion, QtCreator) and link the required libraries.
4. Compile the project.
5. Run the executable. The microprogram will calculate the sum of products for the values in the A and B registers and store the result in memory.

## License
Feel free to use, modify, and experiment with this project for personal or educational purposes. The code in this project is provided "as is" without any warranties.
