/*{ LICENCE
 * \file   libc_backends/l4re_mem/mem.cc
 *
 * (c) 2004-2009 Technische Universität Dresden
 * (c) 2012 Gilbert Röhrbein :P
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU Lesser General Public License 2.1.
 * Please see the COPYING-LGPL-2.1 file for details.
 }*/
#include <stdlib.h>/*{*/
#include <stdio.h>
#include <l4/sys/kdebug.h>
#include <l4/re/util/cap_alloc>
#include <l4/re/dataspace>
#include <l4/re/env>
#include <cmath>
/*}*/

#include "Chunk.h"
#include "Space.h"


void * operator new (size_t, void *) throw();

bool malloc_initialized;
Space* malloc_first_space;
Space* malloc_current_space;
L4::Cap<L4Re::Dataspace> malloc_ds_cap;


// get a dataspace from sigma0
// mem_alloc() ceilings to one page AKA 4096B
int allocate_dataspace(size_t &addr, size_t space_size=1)
{
  printf("malloc: allocate_dataspace(%i, %i)\n",addr, space_size);
  // already got dataspace cap

  space_size = Space::align_size(space_size);
  printf("malloc: allocate_dataspace(%i, %i) aligned\n",addr, space_size);


  // one page is 4kB
  long err =
    L4Re::Env::env()->mem_alloc()->alloc(space_size, malloc_ds_cap);
  if(err) return 0;

  // attach dataspace to locale address Space
  err = 
    L4Re::Env::env()->rm()->attach(
        (void**)&addr, space_size,
        L4Re::Rm::Search_addr, malloc_ds_cap, 0);
  if (err) return 0;

  printf("malloc: allocate_dataspace -> addr= %i \n",addr);
  return 0;
}

Space* create_space(size_t size = 1)
{
  size_t addr = 0;
  allocate_dataspace(addr);

  Space* created_space = (Space*) addr;
  created_space->init(size);

  printf("malloc: create_space() -> new Space %p at %i\n", created_space, addr);

  // alternative placement syntax XXX
  // Space *space = new (addr) (void*)Space(size);
  // created_space = &space;

  Chunk* first_chunk = created_space->first();
  first_chunk->prev = NULL;
  first_chunk->next = NULL;
  first_chunk->size = 0;

  printf("malloc: create_space() -> first Chunk begins at %p \n",first_chunk);

  return created_space;
}

// appends new space to malloc_current_space
Space* append_space(size_t size = 1)
{
  Space *newspace = create_space(size);
  malloc_current_space->next = newspace;
  newspace->prev = malloc_current_space;
  malloc_current_space = newspace;

  return newspace;
}

bool malloc_init()
{
  printf("malloc: malloc_init() \n");

  // get Dataspace Capability
  malloc_ds_cap = L4Re::Util::cap_alloc.alloc<L4Re::Dataspace>();
  if(!malloc_ds_cap.is_valid())
  {
    printf("malloc: malloc_init() -> malloc_ds_cap is invalid\n");
    return 0;
  }
  printf("malloc: malloc_init() -> malloc_ds_cap is valid\n");

  // init the first dataspace
  size_t addr = 0;
  malloc_current_space = create_space(addr);
  malloc_first_space = malloc_current_space;
  printf("malloc: malloc_init() -> malloc_current_space= %p \n", malloc_current_space);
  printf("malloc: malloc_init() -> malloc_first_space= %p \n", malloc_first_space);

  malloc_initialized = true;
  return malloc_initialized;
}

void print()
{
  malloc_first_space->print_chunks();
}

size_t align_size(size_t size) 
{
  if (size < sizeof(l4_umword_t))
    return sizeof(l4_umword_t);
  else
    return size + size % sizeof(l4_umword_t);
}

extern "C" void *malloc(size_t size) throw()
{
  printf("malloc: malloc(%i) ->  \n", size);
  // initialize if not yet initialized ;)
  if(!malloc_initialized)
    if(!malloc_init())
      return 0;


  printf("malloc: malloc() -> unaligned size= %i\n", size);
  size = align_size(size);
  printf("malloc: malloc() ->   aligned size= %i\n", size);

  printf("L4_PAGESIZE (%lu) !=  sizeof(l4_umword_t)(%u) ?\n", L4_PAGESIZE, sizeof(l4_umword_t));

  Chunk *used_chunk = NULL; 

  // 1.) use a free chunk
  if(used_chunk == NULL){
    used_chunk = malloc_current_space->find_chunk(size);
  }

  // 2.) append a new chunk to malloc_current_space
  if(used_chunk == NULL){
    printf("malloc: malloc() -> appending chunk space");
    used_chunk = malloc_current_space->append_chunk(size);
  }

  // 3.) reduce malloc_current_space
  // 4.) allocate new dataspace ( should return first chunk )

  if(used_chunk == NULL){ 
    printf("malloc: malloc() -> allocating new space");
    append_space();
    used_chunk = malloc_current_space->append_chunk(size);
  }

  // there's nothing left to gain
  if(used_chunk == NULL){
    printf("malloc: malloc() -> could not broker any chunks");
    return NULL;
  }

  // at this point we definitely have a Chunk we can use
  used_chunk->free = false;
  return used_chunk->addr();
}


// XXX TODO FIXME
extern "C" void free(void *p) throw() 
{
  enter_kdebug("free");
  p = p;
}
