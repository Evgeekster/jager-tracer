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


def get_map (data: list[list]) -> str:

    s = '''
extern const char* syscall_names_arr[] = { 
'''
    for i in range(len(data)):
        s += f'   [{i}] = "{data[i][1]}",\n'
    
    s += '};\n'
    return s

def main():
    arch = platform.machine().lower()
    p = get_path(arch=arch)
    if not p:
        raise RuntimeError('Unsupported OS type or unknown path')
    
    with open(p, 'r') as f:
        lines = f.readlines()

    res = []
    for l in lines:
        parts = l.strip().split()
        if l.startswith('#ifndef') or len(l.split()) < 3:
            continue
        if parts[0] == '#define' and parts[1].startswith('__NR_'):
            res.append([parts[2], parts[1].replace('__NR_', '')])
    
    return(get_map(res))




if __name__ == '__main__':
    main()
