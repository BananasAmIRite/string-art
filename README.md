# String Art Generator

A comprehensive C++ string art generator with Arduino automation and analysis tools.

## Overview

This project generates string art from images and provides complete automation tools for physical string art creation using an Arduino-controlled machine.

# Python Tools

-   **generate.py**: Compiles the C++ code and generates Arduino instructions
-   **send_commands.py**: Sends serial commands to the Arduino stringer
-   **analyze_pegs.py**: Analyzes peg usage patterns and generates frequency plots

## Usage

### Compilation

```bash
make
```

### C++ String Art Generator

```bash
string-art input.pgm num_pins opacity threshold skipped_neighbors num_windings scale_factor output.pgm windings.txt
```

**Parameters:**

-   `input.pgm` - Square P5 portable graymap image (suggested: `512x512`)
-   `num_pins` - Number of pegs around the circle (suggested: `200-300`)
-   `opacity` - Line opacity factor (suggested: `0.2`, lower = darker lines)
-   `threshold` - Scoring threshold (suggested: `255`)
-   `skipped_neighbors` - Minimum distance between consecutive pegs (suggested: `20`)
-   `num_windings` - Maximum number of string segments (suggested: `10000`)
-   `scale_factor` - Output image scaling factor (suggested: `4`)
-   `output.pgm` - Output string art image file
-   `windings.txt` - windings output file

### Python Workflow

#### Generate String Art and Instructions

```python
python generate.py
```

This script:

1. Compiles the C++ code
2. Converts input images to proper format (cropped, resized, grayscale)
3. Runs the string art generator
4. Creates Arduino instruction files

#### Send Commands to Arduino

```python
python send_commands.py
```

Sends the generated instructions to the Arduino stringer via serial communication.

#### Analyze Peg Usage

```python
python analyze_pegs.py
```

Generates frequency plots and statistics showing how often each peg was used.
