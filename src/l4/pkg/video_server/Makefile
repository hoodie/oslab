PKGDIR	?= .
L4DIR	?= $(PKGDIR)/../..

TARGET = server client
REQUIRES_LIBS := libc_be_mem
SRC_CC_server = video_server.cc server.cc
SRC_CC_client = client.cc

include $(L4DIR)/mk/prog.mk
