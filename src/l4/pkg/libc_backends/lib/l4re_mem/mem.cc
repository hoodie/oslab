/**
 * \file   libc_backends/l4re_mem/mem.cc
 */
/*
 * (c) 2004-2009 Technische Universität Dresden
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU Lesser General Public License 2.1.
 * Please see the COPYING-LGPL-2.1 file for details.
 */
#include <stdlib.h>
#include <l4/sys/kdebug.h>

void *malloc(unsigned size) throw()
{
	void *data = 0;
	enter_kdebug("malloc");
	return (void*)data;
}


void free(void *p) throw()
{
	enter_kdebug("free");
}
