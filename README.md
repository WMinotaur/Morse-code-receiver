# Morse Code Interpreter for MKL05Z4

This project is an implementation of a Morse Code interpreter designed to work with the MKL05Z4 microcontroller. It uses ADC readings, SysTick timer, UART, and button inputs to decode Morse signals and display messages.

## Features
- Decodes light signals into Morse code (dots and dashes) using ADC.
- Converts Morse code to readable alphanumeric characters.
- Supports clearing the message buffer via a button press.
- Outputs the decoded message via UART.
- Real-time decoding with SysTick timer (100 ms intervals).

## Components
- **MKL05Z4 Microcontroller**: The core of the project.
- **UART Communication**: Displays decoded messages on a terminal.
- **ADC Module**: Detects light signals.
- **Buttons S2 and S3**:
  - S2: Displays the decoded message.
  - S3: Clears the message buffer.

## Code Overview
### Key Modules
1. **SysTick Timer**:
   - Tracks timing for decoding signals.
   - Processes data every 100 ms.

2. **ADC (Analog-to-Digital Converter)**:
   - Reads light signals.
   - Converts signals to voltage and determines if it's a dot, dash, or pause.

3. **Morse Code Decoder**:
   - Maps Morse code sequences to alphanumeric characters.
   - Converts signals (e.g., `.-`) to their respective letters (`A`).

4. **Button Handling (S2 and S3)**:
   - S2: Outputs the current message via UART.
   - S3: Clears the message buffer.

### Example Workflow
1. ADC detects a light signal and determines its duration.
2. Signal durations are translated into dots (`.`) or dashes (`-`).
3. The Morse code sequence is decoded into characters.
4. The decoded message is displayed or cleared upon button press.

## Hardware Configuration
- **ADC Channel**: Channel 8 for light signal detection.
- **Buttons**:
  - S2 connected to port `PA`, with interrupt handling.
  - S3 connected similarly for clearing the message buffer.
- **UART**: Configured for communication with a terminal.

## Morse Code Mapping
The project uses the standard international Morse code table to map sequences like `.-` to characters like `A`.

## Installation & Setup
1. Clone the repository:
   git clone https://github.com/WMinotaur/morse-code-interpreter.git
