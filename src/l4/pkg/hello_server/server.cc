/*{*//* LICENCE
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
#include <l4/re/util/meta>
#include <l4/cxx/ipc_server>
/*}*/

#include "shared.h"
#include "hello_server.h"
#include "session_server.h"

int main()
{
  // static Hello_server hserv;
  SessionServer *sserver = new SessionServer(); 
  L4Re::Util::Registry_server<> server = sserver->get_server();

  if (!server.registry()->register_obj(sserver, "hello_server").is_valid())
  {
    printf("Could not register my service, readonly namespace?\n");
    return 1;
  }
  printf("server is up and running\n");

  server.loop();

  return 0;
}
