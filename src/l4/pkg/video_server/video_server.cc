#include "video_server.h"
#include <stdio.h>
#include "protocol.h"


/** Video Server*/
VideoServer::VideoServer(unsigned sessid) : session_id(sessid)
{
  fb =  new L4Re::Util::Video::Goos_fb("fb");
  int r = fb->view_info(&info);
  buffer = fb->attach_buffer();
  printf ("Video Server Session %i Initiated\n",sessid);
}

void VideoServer::get_info()
{
  L4Re::Video::View::Info info;
  int r = fb->view_info(&info);
  if(!r){
     printf("so there is a frame buffer after all\n"); 
     // printf("info: bytes_per_pixel %u\n", info.pixel_info.bytes_per_pixel); 
     printf("info: width %u \n", info.width); 
     printf("info: height %u \n", info.height); 
  }
}

void *VideoServer::get_pixel(int x, int y){
  void *addr =(void*) ((char*) buffer
  + ( y * info.bytes_per_line )
  + ( x * info.pixel_info.bytes_per_pixel()) );


  return addr;
}

int VideoServer::set_pixel(int x, int y){
  void *p = get_pixel(x,y);

  L4Re::Video::Pixel_info *pixel = (L4Re::Video::Pixel_info*) p;

  L4Re::Video::Color_component a;
  a.set(65535);
  pixel->r(a);
  pixel->g(a);
  pixel->b(a);
  printf("blue\n"); 

  return 0;
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

    case Opcode::info: 
      printf("opcode info- lets see what we got\n");
      this->get_info();
      this->get_pixel(15,15);
      this->set_pixel(15,15);
      this->get_pixel(15,15);
      break;

    default:
      printf("opcode unknown\n");
      break;
      //return -L4_ENOSYS;
  }

  printf("buf: \"%s\"\n", buf);
  return L4_EOK;
}

