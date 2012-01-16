PKGDIR		?= ../..
L4DIR		?= $(PKGDIR)/../..

TARGET		= server client

# list your .c or .cc files here
# SRC_C	          = 
# SRC_CC          = server.cc client.cc
server   = server.cc
client   = client.cc

# list requirements of your program here
REQUIRES_LIBS   += libc_be_mem

include $(L4DIR)/mk/prog.mk
