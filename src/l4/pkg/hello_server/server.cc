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
static L4Re::Util::Registry_server<> server;

class SessionServer : public L4::Server_object
{
  unsigned session_id;
  public:
    SessionServer() : session_id(0)
    {
    }

    int dispatch ( l4_umword_t, L4::Ipc::Iostream &ios)
    {

      l4_msgtag_t tag;
      ios >> tag;

      switch(tag.label())
      {
        case L4::Factory::Protocol:
          {
            printf("\e[1;31mFactory Protocol\e[0;31m\n");
            unsigned op;
            ios >> op;
            if (op != 42) return -L4_EINVAL;

            /* creating my hello server*/
            HelloServer *helloServer = new HelloServer(++session_id);
            server.registry()->register_obj(helloServer);
            ios << helloServer->obj_cap();

            printf("\e[0m\n\n");
            return L4_EOK;
          }
        case L4::Meta::Protocol:
          {
            printf("\e[1;33mMeta Protocol\n");
            int hmr= L4Re::Util::handle_meta_request<L4::Factory>(ios);
            printf("handle meta request = %i \e[0m\n\n", hmr);
            return hmr;

          }
        default:
          {
            printf("\e[1;34mDefault\e[0m\n\n");
            return -L4_EINVAL;
          }
      }
    }
};


int main()
{
  // static Hello_server hserv;
  SessionServer sserver; 

  if (!server.registry()->register_obj(&sserver, "hello_server").is_valid())
  {
    printf("Could not register my service, readonly namespace?\n");
    return 1;
  }
  printf("server is up and running\n");

  server.loop();

  return 0;
}
