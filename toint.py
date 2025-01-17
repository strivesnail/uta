import csv
import struct
from decimal import Decimal

# Helper function to calculate max precision
def get_max_precision(num):
    str_num = str(num).rstrip('0')
    if '.' in str_num:
        return len(str_num.split('.')[1])
    return 0

# Helper function to convert float to int
def convert_to_int(num, multiplier):
    num_mul = Decimal(str(num)) * Decimal(multiplier)
    return int(num_mul.quantize(Decimal('1')))

# Input/Output filenames
input_filename = "/tmp/trace/alex_datasets/data.bin"
output_filename = "/tmp/trace/alex_datasets/converted_data.csv"

# Step 1: Read binary file
data = []
with open(input_filename, "rb") as binfile:
    while True:
        # Read 8 bytes (float64)
        bytes_read = binfile.read(8)
        if not bytes_read:
            break
        # Unpack bytes to a float64
        num = struct.unpack('d', bytes_read)[0]
        data.append(num)

# Step 2: Find maximum precision
max_precision = 0
for num in data:
    precision = get_max_precision(num)
    if precision > max_precision:
        max_precision = precision

# Step 3: Determine multiplier
multiplier = 10 ** max_precision

# Step 4: Convert floats to integers
int_data = []
seen_keys = {}
for line_number, num in enumerate(data, start=1):
    converted_key = convert_to_int(num, multiplier)
    if converted_key in seen_keys:
        print(f"Duplicate found:")
        print(f"Current original key: {num} (Line {line_number})")
        print(f"Previous original key: {seen_keys[converted_key]} (Line {seen_keys[converted_key][1]})")
        break
    else:
        seen_keys[converted_key] = (num, line_number)
        int_data.append(converted_key)

# Step 5: Write results to CSV
else:  # Executes only if no duplicates were found
    with open(output_filename, mode='w', newline='') as outfile:
        writer = csv.writer(outfile)
        for num in int_data:
            writer.writerow([num])
    print(f"Conversion complete. Results saved to {output_filename}")