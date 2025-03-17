import pandas as pd
import numpy as np
import os

# Function to check if a file exists
def check_file_exists(filename):
    if not os.path.isfile(filename):
        print(f"Error: File '{filename}' not found in the current directory.")
        exit(1)

# Check if the original file exists
check_file_exists('us_postal_codes .xlsx')

# Read the original file
df_original = pd.read_excel('us_postal_codes .xlsx')

# Add a column of random values
df_original['RandomColumn'] = np.random.rand(len(df_original))

# Randomize the rows
df_randomized = df_original.sample(frac=1).reset_index(drop=True)

# Drop the random column
df_randomized = df_randomized.drop(columns=['RandomColumn'])

# Save the randomized file
df_randomized.to_excel('randomized.xlsx', index=False)

# Convert original XLSX to CSV
df_original.to_csv('zip_codes.csv', index=False)

# Convert row-randomized XLSX to CSV
df_randomized.to_csv('randomized.csv', index=False)

print("Files generated successfully:")
print("- zip_codes.csv")
print("- randomized.xlsx")
print("- randomized.csv")