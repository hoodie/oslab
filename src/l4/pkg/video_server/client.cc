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
#include <string.h>
#include <stdio.h>
#include "protocol.h"

int hello_call(L4::Cap<void> const &server, const char *message)
{
  L4::Ipc::Iostream stream(l4_utcb());

  unsigned long size = strlen(message);
  stream << l4_umword_t(Opcode::hello);
  stream << L4::Ipc::Buf_cp_out<const char>(message, size);
  printf("%s!!\n", message);

  l4_msgtag_t res = stream.call(server.cap());
  if (l4_ipc_error(res, l4_utcb())) {
    return 1;
  }
  return 0;
}

int server_call(L4::Cap<void> const &server, Opcode::Opcode command, const char *params = "")
{
  L4::Ipc::Iostream stream(l4_utcb());

  unsigned long size = strlen(params);

  stream << command;
  stream << L4::Ipc::Buf_cp_out<const char>(params, size);
  printf("%d - %s!!\n", command, params);

  l4_msgtag_t res = stream.call(server.cap());
  if (l4_ipc_error(res, l4_utcb())) {
    return 1;
  }
  return 0;
}

int main()
{

  // query IPC-Gate at name service > capability
  L4::Cap<void> server = L4Re::Env::env()->get_cap<void>("video_server");

  if (!server.is_valid()) {
    printf("Could not get server capability!\n");
    return 1;
  }
  if (server_call(server, Opcode::info, "0")) {
    printf("Server does not respond correctly\n");
    return 1;
  }
  else {
    printf("Sent successfully!\n");
  }
  return 0;
}
