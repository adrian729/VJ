#!/usr/bin/python3
import os
import argparse
from functools import reduce

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--X', required=False, default='37',
                        help='numero de tiles eix x')
    parser.add_argument('--Y', required=False, default='27',
                        help='numero de tiles eix y')
    args = parser.parse_args()

    x = int(args.X)
    y = int(args.Y)
    print('xy:', x, y)
    lvl_file = 'empty.txt'

    with open(lvl_file, mode='w', encoding='utf-8') as lvl_file:
        for _ in range(0, y):
            line = ''
            for _ in range(0, x):
                line += '0,'
            line += '\n'
            lvl_file.write(line)
