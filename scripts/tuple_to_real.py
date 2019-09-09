#!/usr/bin/python2.7

import re
import sys
import os


def tuple_to_real(path, tuple_idx):
    base, filename = os.path.split(path)
    if '.' in filename:
        i = len(filename) - filename[::-1].find('.') - 1
        name = filename[:i]
    else:
        name = filename

    pattern1 = '(\[(?P<fst>([e\-+\.\d]+,[\s]*){%d})(?P<number>[e\-+\.\d]+).*?\])' % tuple_idx
    pattern2 = '(\[(?P<fst>([e\-+\.\d]+,[\s]*){0})(?P<number>[e\-+\.\d]+).*?\])'
        
    with open(path, 'r') as file:
        content = file.read()
        content = re.sub(pattern1, '\g<number>', content)
        content = re.sub(pattern2, '\g<number>', content)

    out_filename = '%s/%s.out' % (base, name)
    with open(out_filename, 'w') as file:
        file.write(content)
    

if __name__ == '__main__':
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        raise Exception
    tuple_idx = int(sys.argv[2]) if len(sys.argv) == 3 else 0
    tuple_to_real(sys.argv[1], tuple_idx)
