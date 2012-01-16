/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */
#include <stdio.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/re/util/object_registry>
#include <l4/cxx/ipc_server>

#include "shared.h"

static L4Re::Util::Registry_server<> server;

class Hello_server : public L4::Server_object {
  public:
    int dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios);
};

int Hello_server::dispatch(l4_umword_t, L4::Ipc::Iostream &ios) {
  l4_msgtag_t t;
  ios >> t;

  L4::Opcode opcode;
  ios >> opcode;

  switch (opcode) {
    case Opcode::func_hello: 

      l4_uint32_t val ;
      ios >> val;

      char c[val];
      for (l4_uint32_t i = 0; i<val; i++) {
        ios >> c[i];
      }



      printf("Hello Hendrik\n");
      printf("your message has %i characters\n", val-1);
      printf("\"%s\"\n", c);
      return L4_EOK;
    default:
      return -L4_ENOSYS;
    }
}

int main() {

  static Hello_server hserv;

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
