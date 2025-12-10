import os
import subprocess
import concurrent.futures
from threading import Lock

# Thread lock for printing
print_lock = Lock()

import os
import subprocess
import concurrent.futures
from threading import Lock
import threading

# Thread lock for printing and compilation
print_lock = Lock()
compile_lock = Lock()

def process_image(entry):
    """Process a single image with multiple configurations"""
    name, t = entry.split(".")
    thread_id = threading.current_thread().ident
    
    with print_lock:
        print(f"Starting processing: {entry} (Thread: {thread_id})")
    
    # Use thread-specific file names to avoid conflicts
    input_file = f"./bin/input_{thread_id}.pgm"
    output_file = f"./bin/output_{thread_id}.pgm"
    windings_file = f"./bin/windings_{thread_id}.txt"
    
    # Configuration 1: Jason method
    subprocess.run(['python3', 'generate.py', entry, 
                   f"christian_finalists/out/out_{name}{t}_jason.png", 
                   f"christian_finalists/outpins/cmd_{name}{t}_jason.txt", 
                   f'["./bin/string-art", "{input_file}", "200", "0.2", "255", "5", "2000", "4", "{output_file}", "{windings_file}", "1"]'])

    # Configuration 2: Original method 2000 windings
    subprocess.run(['python3', 'generate.py', entry, 
                   f"christian_finalists/out/out_{name}{t}_orig_2000.png", 
                   f"christian_finalists/outpins/cmd_{name}{t}_orig_2000.txt", 
                   f'["./bin/string-art", "{input_file}", "200", "0.2", "255", "5", "2000", "4", "{output_file}", "{windings_file}", "0"]'])

    # Configuration 3: Original method 3000 windings
    subprocess.run(['python3', 'generate.py', entry, 
                   f"christian_finalists/out/out_{name}{t}_orig_3000.png", 
                   f"christian_finalists/outpins/cmd_{name}{t}_orig_3000.txt", 
                   f'["./bin/string-art", "{input_file}", "200", "0.2", "255", "5", "3000", "4", "{output_file}", "{windings_file}", "0"]'])
    
    with print_lock:
        print(f"Completed processing: {entry}")

if __name__ == "__main__":
    # Compile once before starting threads
    with compile_lock:
        print("Compiling C++ code...")
        subprocess.run(["make"])
        print("Compilation complete!")
    
    # Get list of images to process
    images = os.listdir("./bin/inputs")
    
    # Use ThreadPoolExecutor for multithreading
    # Adjust max_workers based on your CPU cores (recommended: 2-4)
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
        # Submit all tasks
        futures = [executor.submit(process_image, entry) for entry in images]
        
        # Wait for all tasks to complete
        concurrent.futures.wait(futures)
    
    print("All images processed successfully!")

