#!/usr/bin/env python3
#
# SPDX-FileCopyrightText: Copyright (c) 2024-2026 Infineon Technologies AG
# SPDX-License-Identifier: MIT

import sys

def main():
    if len(sys.argv) < 2:
        sys.exit(1)

    with open(sys.argv[1], 'r', encoding='utf-8') as f:
        lines = f.readlines()

    # Skip lines until we find the closing -->
    start_index = 0
    for i, line in enumerate(lines):
        if '-->' in line:
            start_index = i + 1
            break

    # Print remaining lines
    print(''.join(lines[start_index:]), end='')

if __name__ == "__main__":
    main()
