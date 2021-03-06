#!/usr/bin/env python
# Copyright (C) 2017 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import re
import sys

def fix_guards(fpath):
  with open(fpath) as f:
    lines = [l.strip('\n') for l in f.readlines()]
  res = []
  guard = re.sub(r'[^a-zA-Z0-9_-]', '_', fpath.upper()) + '_'
  replacements = 0

  endif_line_idx = -1
  for line_idx in xrange(len(lines) - 1, -1, -1):
    if lines[line_idx].startswith('#endif'):
      endif_line_idx = line_idx
      break
  assert(endif_line_idx > 0)

  line_idx = 0
  for line in lines:
    if replacements == 0 and line.startswith('#ifndef '):
      line = '#ifndef ' + guard
      replacements = 1
    elif replacements == 1 and line.startswith('#define '):
      line = '#define ' + guard
      replacements = 2
    elif line_idx == endif_line_idx and replacements == 2:
      assert(line.startswith('#endif'))
      line = '#endif  // ' + guard + '\n'
    res.append(line)
    line_idx += 1
  if res == lines:
    return 0
  with open(fpath, 'w') as f:
    f.write('\n'.join(res))
  return 1

def main():
  if len(sys.argv) < 2:
    print('Usage: %s src include' % sys.argv[0])
    return 1

  num_files_changed = 0
  for topdir in sys.argv[1:]:
    for root, dirs, files in os.walk(topdir):
      for name in files:
        if not name.endswith('.h'):
          continue
        fpath = os.path.join(root, name)
        num_files_changed += fix_guards(fpath)
  print '%d files changed' % num_files_changed

if __name__ == '__main__':
  sys.exit(main())
