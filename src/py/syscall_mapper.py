import os
import platform
def get_path(arch) -> str:

    # add more OS?
    paths = {
        "x86_64": [
            "/usr/include/x86_64-linux-gnu/asm/unistd_64.h",
            "/usr/include/asm/unistd_64.h"
        ],
        "amd64": [
            "/usr/include/x86_64-linux-gnu/asm/unistd_64.h",
            "/usr/include/asm/unistd_64.h"
        ],
        "i386": [
            "/usr/include/i386-linux-gnu/asm/unistd_32.h",
            "/usr/include/asm/unistd_32.h"
        ],
        "aarch64": [
            "/usr/include/aarch64-linux-gnu/asm/unistd.h",
            "/usr/include/arm64-linux-gnu/asm/unistd.h",
            "/usr/include/asm/unistd.h"
        ]
    }

    for p in paths.get(arch, ''):
        if os.path.exists(path=p):
            return p
    return ''
    


# def main(*args):
#     arch = platform.machine().lower()
#     p = get_path(arch=arch)
#     if not p:
#         raise RuntimeError('Unsupported OS type or unknown path')
    
#     with open(p, 'r') as f:
#         lines = f.readlines()
    
#     for l in lines:
#         if l.startswith('#ifndef') or len(l.split()) < 3:
#             continue
#         l = l.strip()
#         if l.split()[2] == args[0]:

#             val = l.split()[2].replace('__NR_', '')
            
#             return val

# def main(args):
#     print(args)
#     arch = platform.machine().lower()
#     p = get_path(arch=arch)
#     if not p:
#         raise RuntimeError('Unsupported OS type or unknown path')
    
#     with open(p, 'r') as f:
#         lines = f.readlines()
    
#     for l in lines:
#         if l.startswith('#ifndef') or len(l.split()) < 3:
#             continue
#         l = l.strip()
#         parts = l.split()
#         # print(parts[1].replace('__NR_', ''))
#         if str(parts[2]) == str(args):
#             print (parts[1].replace('__NR_', ''))
#             return parts[1].replace('__NR_', '')
#         # else:
            
#         #     return 'niggas'


# if __name__ == '__main__':
#     import sys
#     # main(sys.argv[1])
#     main('23')


def main(args):
    arch = platform.machine().lower()
    p = get_path(arch=arch)
    if not p:
        raise RuntimeError('Unsupported OS type or unknown path')
    
    with open(p, 'r') as f:
        lines = f.readlines()
    
    found = False
    for l in lines:
        if l.startswith('#ifndef') or len(l.split()) < 3:
            continue
        l = l.strip()
        parts = l.split()
        if parts[0] == '#define' and parts[1].startswith('__NR_'):
            if str(parts[2]) == str(args):
                result = parts[1].replace('__NR_', '')
                print(result)
                return result
    
    if not found:
       return -1

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        main(sys.argv[1])
        # main('357')
    # else:
    #     main('23')