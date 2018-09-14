import sys, os


def get_size(start_path='.'):
    total_size = 0
    for dirpath, dirnames, filenames in os.walk(start_path):
        for f in filenames:
            fp = os.path.join(dirpath, f)
            total_size += os.path.getsize(fp)
    return total_size

if sys.argv[1] == 'FL':
    for root, dirs, files in os.walk('.'):
        for file in files:
            if len(file) < int(sys.argv[2]):
                print(f'{root}/{file}')
if sys.argv[1] == 'GL':
    for root, dirs, files in os.walk('.'):
        for file in files:
            if len(file) > int(sys.argv[2]):
                print(f'{root}/{file}')
if sys.argv[1] == 'SL':
    for root, dirs, files in os.walk('.'):
        for file in files:
            if os.stat(root+'/'+file).st_size < int(sys.argv[2]):
                print(f'{root}/{file}')
if sys.argv[1] == 'SG':
    for root, dirs, files in os.walk('.'):
        for file in files:
            if os.stat(root+'/'+file).st_size < int(sys.argv[2]):
                print(f'{root}/{file}')
if sys.argv[1] == 'DL':
    for root, dirs, files in os.walk('.'):
        for dir in dirs:
            if get_size(f'{root}/{dir}') < int(sys.argv[2]):
                print(f'{root}/{dir}')