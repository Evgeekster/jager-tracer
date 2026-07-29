#!/usr/bin/env python3
"""
Generate NamedConst tables for setsockopt/getsockopt option names,
parsed from system headers, the same way syscall_map.py parses
unistd_64.h for syscall numbers.

Regenerate with: python3 scripts/py/gen_sockopts.py
"""
import argparse
import re
import sys
from pathlib import Path

PREAMBLE = """\
/* AUTO-GENERATED — do not edit by hand.
 * Regenerate with:  python3 scripts/py/gen_sockopt_names.py
 */
#include "args.h"
#include <stddef.h>
"""

# Each entry: (C array name, prefix to match, candidate header paths)
SOURCES = [
    (
        "sockopt_name_sol_socket",
        "SO_",
        [
            "/usr/include/asm-generic/socket.h",
            "/usr/include/x86_64-linux-gnu/bits/socket.h",
            "/usr/include/bits/socket.h",
        ],
    ),
    (
        "sockopt_name_tcp",
        "TCP_",
        [
            "/usr/include/netinet/tcp.h",
        ],
    ),
    (
        "sockopt_name_ip",
        "IP_",
        [
            "/usr/include/linux/in.h",
        ],
    ),
]

NOT_AN_OPTNAME = {
    "TCP_REPAIR_ON", "TCP_REPAIR_OFF", "TCP_REPAIR_OFF_NO_WP",
    "TCP_MD5SIG_FLAG_PREFIX", "TCP_MD5SIG_FLAG_IFINDEX",
    "TCP_COOKIE_MIN", "TCP_COOKIE_MAX", "TCP_COOKIE_PAIR_SIZE",
    "TCP_MD5SIG_MAXKEYLEN",
}
NOT_AN_OPTNAME_SUBSTR = ("_PMTUDISC_", "_DEFAULT_MULTICAST_")

DEFINE_RE = re.compile(r'^\s*#define\s+(\S+)\s+(\S+)')


def parse_header(path: str, prefix: str) -> dict[str, int]:
    result: dict[str, int] = {}
    p = Path(path)
    if not p.exists():
        return result

    for line in p.read_text(errors="ignore").splitlines():
        m = DEFINE_RE.match(line)
        if not m:
            continue
        name, raw_val = m.group(1), m.group(2)

        if not name.startswith(prefix):
            continue
        if name in NOT_AN_OPTNAME:
            continue
        if any(sub in name for sub in NOT_AN_OPTNAME_SUBSTR):
            continue
        if "(" in name:
            continue
        if not re.fullmatch(r'0[xX][0-9a-fA-F]+|\d+', raw_val):
            continue

        value = int(raw_val, 0)
        result.setdefault(name, value)

    return result


def find_header(paths: list[str]) -> str:
    for p in paths:
        if Path(p).exists():
            return p
    return ""


def gen_table(array_name: str, values: dict[str, int]) -> str:
    if not values:
        return f"const NamedConst {array_name}[] = {{\n    {{ 0, NULL }}\n}};\n"

    lines = [f"const NamedConst {array_name}[] = {{"]
    for name, val in sorted(values.items(), key=lambda kv: kv[1]):
        lines.append(f'    {{ {val}, "{name}" }},')
    lines.append("    { 0, NULL }")
    lines.append("};")
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", default="../../src/args/sockopt_names.c",
                         help="Output file path")
    args = parser.parse_args()

    chunks = [PREAMBLE]

    for array_name, prefix, candidates in SOURCES:
        header = find_header(candidates)
        if not header:
            print(f"WARNING: no header found for {array_name} "
                  f"(tried {candidates})", file=sys.stderr)
            chunks.append(gen_table(array_name, {}))
            continue

        values = parse_header(header, prefix)
        print(f"{array_name}: parsed {len(values)} constants from {header}",
              file=sys.stderr)
        chunks.append(gen_table(array_name, values))

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text("\n".join(chunks))
    print(f"Written -> {out}", file=sys.stderr)


if __name__ == "__main__":
    main()