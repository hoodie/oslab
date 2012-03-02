/*{*/
/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 *
 * Adapted by Hendrik Sollich
 */
/*}*/
#include <l4/sys/err.h>
#include <l4/sys/types.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/cxx/ipc_stream>

#include <stdio.h>
#include "shared.h"

int hello_call(L4::Cap<void> const &server)
{
  printf("hello_call begin\n");
  L4::Ipc::Iostream stream(l4_utcb());

  stream << l4_umword_t(Opcode::hello);
  stream << "ping_pong!" ; // 10 Characters

  l4_msgtag_t res = stream.call(server.cap(), Protocol::Hello);

  if (l4_ipc_error(res, l4_utcb())){
    return 1; // failure
  }
  printf("hello_call end\n");
  return 0; // ok
}


int main()
{

  // query IPC-Gate at name service > capability
  L4::Cap<void> server = L4Re::Env::env()->get_cap<void>("hello_server");

  if (!server.is_valid()) {
    printf("Could not get server capability!\n");
    return 1;
  }
  if (hello_call(server)) {
    printf("Server does not respond correctly\n");
    return 1;
  }
  else {
    printf("Sent successfully!\n");
  }
  return 0;
}
