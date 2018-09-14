import os
import sys
import re
import fractions

if len(sys.argv) > 2:
    out = open(sys.argv[2], 'w')
else:
    out = sys.stdout


if len(sys.argv) > 1 and os.path.exists(sys.argv[1]):
    with open(sys.argv[1], 'r') as fd:
        lines = fd.readlines()
        for line in lines:
            out.write(str(eval(re.sub(r'([0-9]+)','fractions.Fraction(\\1)',line))) + '\n')
else:
    out.write('file does not exist')