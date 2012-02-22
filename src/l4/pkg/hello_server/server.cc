/*{ LICENCE
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */ /*}*/
/*{*/
#include <stdio.h>
#include <stdlib.h>
//#include <iostream>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/re/util/object_registry>
#include <l4/cxx/ipc_server>

#include "shared.h"
#include "server.h"/*}*/

static L4Re::Util::Registry_server<> server;

int main() {

  static Hello_server hserv;
  // int *a = new int();
  // int *b = new int();
  //                             capacity : 4096 - SH(16) - fCH(16) =  4064
  printf("calling malloc -> %i\n", (size_t)malloc(1));    // c-4    =  4060
  printf("calling malloc -> %i\n", (size_t)malloc(2));    // c-4    =  4040
  printf("calling malloc -> %i\n", (size_t)malloc(3));    // c-4    =  4020
  printf("calling malloc -> %i\n", (size_t)malloc(4));    // c-4    =  4000
  printf("calling malloc -> %i\n", (size_t)malloc(5));    // c-8    =  3976
  printf("calling malloc -> %i\n", (size_t)malloc(4));    // c-4    =  3956
  printf("calling malloc -> %i\n", (size_t)malloc(1024)); // c-1024 =  3816
  printf("calling malloc -> %i\n", (size_t)malloc(320));  // c-320  =  2580
  printf("calling malloc -> %i\n", (size_t)malloc(2048)); // c-2048 =   516
  printf("calling malloc -> %i\n", (size_t)malloc(2049)); // c-2052 = -1536 (2528 in next space)

  // Register hservulation server
  if (!server.registry()->register_obj(&hserv, "channel").is_valid())
    {
      printf("Could not register my service, readonly namespace?\n");
      return 1;
    }

  printf("This is Hendriks hello world server,\nhaven't had much time to actually mess around with the actual code.\n");

  // Wait for client requests
  server.loop();

  return 0;
}
