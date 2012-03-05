
/** Video Server*/
class VideoServer : public L4::Server_object
{
  unsigned session_id;
  public:
    VideoServer(unsigned sessid) : session_id(sessid)
    {
      printf ("session %i initiated\n",sessid);
    }

    int dispatch( l4_umword_t, L4::Ipc::Iostream &ios )
    {
      char *buf = NULL;
      unsigned long size;
      
      L4::Opcode opcode;
      ios >> size >> L4::Ipc::Buf_in<char>(buf,size);

      switch (opcode) {
        case Opcode::hello: 
          printf("opcode hello\n");
          break;

        default:
          printf("opcode unknown\n");
          break;
          //return -L4_ENOSYS;
      }

      printf("buf: \"%s\"\n", buf);
      return L4_EOK;
    }

};
