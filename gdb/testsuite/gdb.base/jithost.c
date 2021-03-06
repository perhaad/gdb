/* Copyright (C) 2009-2016 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

#include JIT_READER_H  /* Please see jit-reader.exp for an explanation.  */
#include "jithost.h"
#include "jit-protocol.h"

void __attribute__((noinline)) __jit_debug_register_code () { }

struct jit_descriptor __jit_debug_descriptor = { 1, 0, 0, 0 };
struct jit_code_entry only_entry;

typedef void (jit_function_t) ();

int main (int argc, char **argv)
{
  char *code = mmap (NULL, getpagesize (), PROT_WRITE | PROT_EXEC,
		     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  jit_function_t *function = (jit_function_t *) code;

  code[0] = 0xcc; /* SIGTRAP  */
  code[1] = 0xc3; /* RET  */

  struct jithost_abi *symfile = malloc (sizeof (struct jithost_abi));
  symfile->begin = code;
  symfile->end = code + 2;

  only_entry.symfile_addr = symfile;
  only_entry.symfile_size = sizeof (struct jithost_abi);

  __jit_debug_descriptor.first_entry = &only_entry;
  __jit_debug_descriptor.relevant_entry = &only_entry;
  __jit_debug_descriptor.action_flag = JIT_REGISTER;
  __jit_debug_descriptor.version = 1;
  __jit_debug_register_code ();

  function ();

  return 0;
}
