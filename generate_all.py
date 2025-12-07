import os
import subprocess

for entry in os.listdir("./bin/inputs"):
    print(entry)
    name, t = entry.split(".")
    subprocess.run(['python3', 'generate.py', entry, f"out/out_{name}{t}_orig.png", f"cmdout/cmd_{name}{t}_orig.txt"])