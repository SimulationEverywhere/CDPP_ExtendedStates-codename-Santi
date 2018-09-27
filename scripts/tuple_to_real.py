#!/usr/bin/python2.7

import re
import sys


def tuple_to_real(filename, tuple_idx):
    i = len(filename) - filename[::-1].find('.') - 1
    name, ext = filename[:i], filename[i+1:]

    pattern1 = '(\[(?P<fst>([e\-+\.\d]+,[\s]*){%d})(?P<number>[e\-+\.\d]+).*?\])' % tuple_idx
    pattern2 = '(\[(?P<fst>([e\-+\.\d]+,[\s]*){0})(?P<number>[e\-+\.\d]+).*?\])'
        
    with open(filename, 'r') as file:
        content = file.read()
        content = re.sub(pattern1, '\g<number>', content)
        content = re.sub(pattern2, '\g<number>', content)

    out_filename = '%s.out' % name
    with open(out_filename, 'w') as file:
        file.write(content)
    

if __name__ == '__main__':
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        raise Exception
    tuple_idx = int(sys.argv[2]) if len(sys.argv) == 3 else 0
    tuple_to_real(sys.argv[1], tuple_idx)
