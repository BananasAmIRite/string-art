from collections import Counter
import numpy as np
import matplotlib.pyplot as plt

analysis_file = './bin/peg_analysis.txt'

occurrences = None

with open(analysis_file, 'r') as f:
    occurrences = [int(line.strip()) for line in f if line.strip().isdigit()]

occurrence_counter = Counter(occurrences)

# Extract data for plotting
frequencies = list(occurrence_counter.keys())
counts = list(occurrence_counter.values())

# Create frequency distribution plot
plt.figure(figsize=(12, 6))
plt.bar(frequencies, counts, alpha=0.7, color='blue')
plt.xlabel('Number of Times Peg Was Used')
plt.ylabel('Number of Pegs')
plt.title('Frequency Distribution of Peg Usage')
plt.grid(True, alpha=0.3)

# Add statistics
total_pegs = len(occurrences)
unique_usage_levels = len(occurrence_counter)
most_common_usage = occurrence_counter.most_common(1)[0]

plt.text(0.6, 0.9, f'Total pegs: {total_pegs}', transform=plt.gca().transAxes)
plt.text(0.6, 0.85, f'Unique usage levels: {unique_usage_levels}', transform=plt.gca().transAxes)
plt.text(0.6, 0.8, f'Most common: {most_common_usage[1]} pegs used {most_common_usage[0]} times', 
         transform=plt.gca().transAxes)

plt.tight_layout()
plt.show()

