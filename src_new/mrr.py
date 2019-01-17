import sys
import os
import struct
import pandas as pd
import numpy as np


def main():
    if(len(sys.argv) != 2):
        print("[Usage:] python3 mrr.py csv_file")

    file_path = sys.argv[1]
    hits1 = [0] * 10
    hits10 = [0] * 10
    hits50 = [0] * 10
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = [int(num) for num in line.strip().split(' ')]
            # print(line)
            # return
            for i in range(10):
                if(line[i + 2] != -1 and line[i + 2] <= 1):
                    hits1[i] += 1
                if(line[i + 2] != -1 and line[i + 2] <= 10):
                    hits10[i] += 1
                if(line[i + 2] != -1 and line[i + 2] <= 50):
                    hits50[i] += 1

        hits1 = [float(num) / len(lines) for num in hits1]
        hits10 = [float(num) / len(lines) for num in hits10]
        hits50 = [float(num) / len(lines) for num in hits50]
        print("Hits @ 1-> ", hits1)
        print("Hits @ 10-> ", hits10)
        print("Hits @ 50-> ", hits50)


if __name__ == '__main__':
    main()
