#include <l4/re/util/meta>
#include "video_server.h"


class SessionServer : public L4::Server_object
{
  unsigned session_id;
  L4Re::Util::Registry_server<> server;

  public:
    SessionServer() : session_id(0)
    {
      L4Re::Util::Registry_server<> server;
    }

    L4Re::Util::Registry_server<> get_server()
    {
      return this->server;
    }

    int dispatch ( l4_umword_t, L4::Ipc::Iostream &ios)
    {

      l4_msgtag_t tag;
      ios >> tag;

      switch(tag.label())
      {
        case L4::Factory::Protocol:
          {
            printf("\n\e[1;31mFactory Protocol\e[0;31m\n");
            unsigned op;
            ios >> op;
            if (op != 42) return -L4_EINVAL;
            printf("-----------------\n");

            /* creating my video server*/
            VideoServer *videoServer = new VideoServer(++session_id);
            server.registry()->register_obj(videoServer);
            ios << videoServer->obj_cap();

            printf("\n\e[0m\n");
            return L4_EOK;
          }
        case L4::Meta::Protocol:
          {
            printf("\n\e[1;33mMeta Protocol\n");
            int hmr= L4Re::Util::handle_meta_request<L4::Factory>(ios);
            printf("handle meta request = %i \e[0m\n", hmr);
            return hmr;
          }
        default:
          {
            printf("\n\e[1;34mDefault\e[0m\n");
            return -L4_EINVAL;
          }
      }
    }
};

