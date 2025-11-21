from PIL import Image
import subprocess
from collections import Counter

def convert_to_pgm(input_path, output_path):
    """
    Converts an image file to PGM format, crops it to square, and resizes to 512x512.

    Args:
        input_path (str): The path to the input image file.
        output_path (str): The path where the PGM image will be saved.
    """
    try:
        # Open the input image
        img = Image.open(input_path)

        # Get image dimensions
        width, height = img.size
        
        # Crop to square (center crop)
        min_dimension = min(width, height)
        left = (width - min_dimension) // 2
        top = (height - min_dimension) // 2
        right = left + min_dimension
        bottom = top + min_dimension
        
        img = img.crop((left, top, right, bottom))
        
        # Resize to 512x512
        img = img.resize((512, 512), Image.Resampling.LANCZOS)

        # Convert the image to 'L' mode (8-bit grayscale) for PGM
        img = img.convert('L')

        # Save the image in PGM format
        img.save(output_path)
        print(f"Image successfully converted to PGM (512x512) and saved at: {output_path}")

    except FileNotFoundError:
        print(f"Error: Input file not found at {input_path}")
    except Exception as e:
        print(f"An error occurred during conversion: {e}")

def convert_pgm_to_png(pgm_path, png_path):
    """
    Converts a PGM image file to PNG format.

    Args:
        pgm_path (str): The path to the input PGM file.
        png_path (str): The path where the output PNG file will be saved.
    """
    try:
        img = Image.open(pgm_path)
        img.save(png_path, "PNG")
        print(f"Successfully converted '{pgm_path}' to '{png_path}'")
    except FileNotFoundError:
        print(f"Error: PGM file not found at '{pgm_path}'")
    except Exception as e:
        print(f"An error occurred during conversion: {e}")

def analyze_windings(windings_file="./bin/windings.txt"):
    """
    Analyzes windings.txt to count peg occurrences and find the maximum.
    """
    try:
        with open(windings_file, 'r') as f:
            # Read all lines and convert to integers
            pegs = [int(line.strip()) for line in f if line.strip().isdigit()]
        
        # Count occurrences of each peg
        peg_counts = Counter(pegs)
        
        # Find the maximum occurrence
        max_occurrence = max(peg_counts.values())
        most_used_pegs = [peg for peg, count in peg_counts.items() if count == max_occurrence]
        
        print(f"Total windings: {len(pegs)}")
        print(f"Unique pegs used: {len(peg_counts)}")
        print(f"Maximum occurrences: {max_occurrence}")
        print(f"Most used peg(s): {most_used_pegs}")
        
        return max_occurrence, most_used_pegs, peg_counts
        
    except FileNotFoundError:
        print(f"Error: Windings file not found at '{windings_file}'")
        return None, None, None
    except Exception as e:
        print(f"An error occurred while analyzing windings: {e}")
        return None, None, None

subprocess.run(["make"])
convert_to_pgm("./bin/inputs/einstein.png", "./bin/input.pgm")
subprocess.run(["./bin/string-art", "./bin/input.pgm",  "100" ,"0.2" ,"255", "0", "10000", "4", "./bin/output.pgm", "./bin/windings.txt"])
# convert_pgm_to_png("./output-100-10000.pgm", "output-100-10000.png")
convert_pgm_to_png("./bin/output.pgm", "./bin/output.png")
analyze_windings()
subprocess.run(['rm', '-rf', './bin/output.pgm'])
subprocess.run(['rm', '-rf', './bin/input.pgm'])