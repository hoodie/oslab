#include "video_server.h"
#include <stdio.h>
#include "protocol.h"


/** Video Server*/
VideoServer::VideoServer(unsigned sessid) : session_id(sessid)
{
  fb =  new L4Re::Util::Video::Goos_fb("fb");
  buffer = fb->attach_buffer();
  printf ("Video Server Session %i Initiated\n",sessid);
}

void VideoServer::draw()
{
  L4Re::Video::View::Info info;
}

int VideoServer::dispatch( l4_umword_t, L4::Ipc::Iostream &ios )
{
  char *buf = NULL;
  unsigned long size;

  int opcode;
  ios >> opcode >> L4::Ipc::Buf_in<char>(buf,size);

  switch (opcode) {
    case Opcode::hello: 
      printf("opcode hello\n");
      break;

    case Opcode::draw: 
      printf("opcode draw - now I'm getting creative\n");
      this->draw();
      break;

    default:
      printf("opcode unknown\n");
      break;
      //return -L4_ENOSYS;
  }

  printf("buf: \"%s\"\n", buf);
  return L4_EOK;
}

