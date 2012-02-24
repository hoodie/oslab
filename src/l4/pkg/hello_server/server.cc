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

void test(int size){
  printf("^^ called malloc(%i) -> %i ^^\n",size, (size_t)malloc(size));
  printf("---------------------------------------------------\n");
}

void malloc_suite()
{
  //                             capacity : 4096 - SH(16) - fCH(16) =  4064
  test(1);    // c-4    =  4060
  test(2);    // c-4    =  4040
  test(3);    // c-4    =  4020
  test(4);    // c-4    =  4000
  test(5);    // c-8    =  3976
  test(4);    // c-4    =  3956
  test(1024); // c-1024 =  3816
  test(320);  // c-320  =  2580
  test(2048); // c-2048 =   516
  test(2049); // c-2052 = -1536 (2528 in next space)
  test(2049);
  test(12049);
  test(2049);
  test(2049);
  test(9);
}

int main()
{

  static Hello_server hserv;
  // int *a = new int();
  // int *b = new int();

  malloc_suite();


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
