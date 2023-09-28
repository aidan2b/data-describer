# CSV Analyzer

This program is crafted to analyze and extract insights from CSV files. It segregates the CSV file data into two types, numeric and non-numeric, and implements diverse approaches to process and analyze each type effectively. The methodology involves the creation of dynamic data structures such as trees for non-numeric data to facilitate the search and analysis of unique values and their occurrences. A set of tailored functions for numeric data calculates intricate statistical measures including mean, variance, standard deviation, skewness, and kurtosis, providing a comprehensive perspective of the data distribution and its characteristics. This program embodies robust memory management, ensuring optimal utilization and release of allocated memory, and maintains precision in handling quoted data and managing missing values, rendering it a versatile tool for intricate data analysis tasks. Additionally, the user-friendly interface aids in the seamless operation of the program, promoting enhanced user interaction and experience.

## Features
- Parses CSV files and categorizes columns into numeric and non-numeric data.
- For numeric data, it calculates:
  - Mean
  - Variance
  - Standard Deviation
  - Min and Max values
  - Median
  - Mode
  - Range
  - Interquartile Range (IQR)
  - Skewness
  - Kurtosis
  - Count of Missing Values
- For non-numeric data, it determines:
  - Most Common Value
  - Occurrences of the Most Common Value
  - Unique Values
  - Max Length
  - Min Length
  - Average Length
  - Count of Missing Values
- Efficient memory management, releasing allocated memory after use.
- Handles data in quotes and manages missing data efficiently.

## Requirements
- C compiler (e.g. GCC)
- Standard C Libraries

## Compilation
```sh
gcc -o csv_analyzer main.c
```
```sh
./csv_analyzer file.csv
```
