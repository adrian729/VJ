#!/usr/bin/python3
import os
import argparse
from functools import reduce

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--fname', required=False, default='lvl.csv',
                        help='CSV file name')
    args = parser.parse_args()

    csv_file_name = args.fname
    print('csv file name', csv_file_name)
    lvl_file = 'lvl.txt'

    with open(csv_file_name, mode='r', encoding='utf-8') as csv_file:
        lines = csv_file.readlines()
        with open(lvl_file, mode='w', encoding='utf-8') as lvl_file:        
            for line in lines:
                line = line.replace(' ', '').replace('\n', '').split(',')
                line = list(map(lambda x: int(x) + 1, line))
                line = reduce((lambda x, y: str(x) + ',' + str(y)), line) + ',\n'
                # line = line.replace('0', ' ')
                lvl_file.write(line)
