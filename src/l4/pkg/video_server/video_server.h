#ifndef video_server
#define video_server
#include <l4/re/util/video/goos_fb>
#include <l4/re/video/goos>
#include <l4/re/video/colors>
#include <l4/re/video/view>
#include <l4/libgfxbitmap/bitmap.h>
#include <l4/libgfxbitmap/font.h>
#include <l4/cxx/ipc_server>


/** Video Server*/
class VideoServer : public L4::Server_object
{
  unsigned session_id; 
  private:
    L4Re::Util::Video::Goos_fb* fb;
  public:
    VideoServer(unsigned sessid);
    int dispatch( l4_umword_t, L4::Ipc::Iostream&  );
};
#endif
