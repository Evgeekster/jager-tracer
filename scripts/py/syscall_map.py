import argparse
import os
import platform
import sys
from pathlib import Path

PREAMBLE = """\
/* AUTO-GENERATED — do not edit by hand.
 * Regenerate with:  python3 scripts/gen_syscall_names.py
 */
#include "syscall_map.h"
#include <stddef.h>
"""

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


def get_map (data: dict[int, str]) -> str:
    s = f'{PREAMBLE}\n'
    s += '''
extern const char* syscall_names_arr[] = { 
'''
    for i in range(max(data) + 1):
        name = data.get(i);
        if name:
            s += f'   [{i}] = "{name}",\n'
        else:
            s += f'   [{i}] = NULL,\n'
    s += '};\n'

    s += '''
const size_t syscall_names_arr_size =\n
    sizeof(syscall_names_arr) / sizeof(syscall_names_arr[0]);\n\n
'''
    s += '''
const char *syscall_name(long nr)\n
{\n
    if (nr < 0 || (size_t)nr >= syscall_names_arr_size)\n
        return \"unknown\";\n
    const char *n = syscall_names_arr[nr];\n
    return n ? n : \"unknown\";\n
}\n
    
    '''
    return s

def main():
    arch = platform.machine().lower()
    p = get_path(arch=arch)
    if not p:
        raise RuntimeError('Unsupported OS type or unknown path')
    
    with open(p, 'r') as f:
        lines = f.readlines()

    res: dict[int, str] = {}
    for l in lines:
        parts = l.strip().split()
        item = None
        if l.startswith('#ifndef') or len(l.split()) < 3:
            continue
        if parts[0] == '#define' and parts[1].startswith('__NR_'):
            #res.append([parts[2], parts[1].replace('__NR_', '')])
            num,  item = int(parts[2]), str(parts[1].replace('__NR_', ''))
            res.setdefault(num, item)


    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--arch", default=platform.machine().lower(),
                        help="Target architecture (default: current machine)")
    parser.add_argument("--out", default="../src/syscall_map/syscall_map.c",
                        help="Output file path")
    args = parser.parse_args()

    print(f"Parsing {p} ...", file=sys.stderr)
    print(f"Found {res} syscalls (max nr={max(res)})", file=sys.stderr)
    code = get_map(res)
    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(code)
    print(f"Written → {out}", file=sys.stderr)




if __name__ == '__main__':
   main()
