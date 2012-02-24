
class Hello_server : public L4::Server_object {
  public:
    int dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios);
    int dispatch2(l4_umword_t obj, L4::Ipc::Iostream &ios);
};

// new and more concise implementation
int Hello_server::dispatch(l4_umword_t, L4::Ipc::Iostream &ios) {
  unsigned long size;
  char *buf = NULL;

  ios >> size >> L4::Ipc::Buf_in<char>(buf,size);

  printf("\"%s\"\n", buf);
  return L4_EOK;
}
// old and more elaborate implementation
int Hello_server::dispatch2(l4_umword_t, L4::Ipc::Iostream &ios) {

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
