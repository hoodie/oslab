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

// TODO malloc()
// TODO 1. search all spaces
// TODO free()
// TODO 1. splitting free chunks
// TODO 2. merging free  chunks

void * operator new (size_t, void *) throw();

bool malloc_initialized;
Space* malloc_first_space;
Space* malloc_current_space;
L4::Cap<L4Re::Dataspace> malloc_ds_cap;

bool do_print = false;

// get a dataspace from sigma0
// mem_alloc() ceilings to one page AKA 4096B
int allocate_dataspace(size_t &addr, size_t space_size=1)
{
  if(do_print) printf(" malloc: allocate_dataspace(%i, %i)\n",addr, space_size);
  // already got dataspace cap

  space_size = Space::align_size(space_size);
  if(do_print) printf(" malloc: allocate_dataspace(%i, %i) aligned\n",addr, space_size);


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

  if(do_print) printf(" malloc: allocate_dataspace -> addr= %i \n",addr);
  return 0;
}

Space* create_space(size_t size = 1)
{
  size_t addr = 0;
  allocate_dataspace(addr);

  size = Space::align_size(size);

  Space* created_space = (Space*) addr;
  created_space->init(size);

  if(do_print) printf(" malloc: create_space() -> new Space(%i) %i at %i\n", size, (size_t) created_space, addr);

  // alternative placement syntax XXX
  // Space *space = new (addr) (void*)Space(size);
  // created_space = &space;

  Chunk* first_chunk = created_space->first();
  if(do_print) printf(" malloc: create_space() -> first Chunk begins at %i \n",(size_t)first_chunk);

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
  if(do_print) printf(" malloc: malloc_init() \n");

  // get Dataspace Capability
  malloc_ds_cap = L4Re::Util::cap_alloc.alloc<L4Re::Dataspace>();
  if(!malloc_ds_cap.is_valid())
  {
    if(do_print) printf(" malloc: malloc_init() -> malloc_ds_cap is invalid\n");
    return 0;
  }
  if(do_print) printf(" malloc: malloc_init() -> malloc_ds_cap is valid\n");

  // init the first dataspace
  malloc_current_space = create_space(1);
  malloc_first_space = malloc_current_space;

  if(do_print){
    printf(" malloc: malloc_init() -> malloc_current_space= %i \n", (size_t) malloc_current_space);
    printf(" malloc: malloc_init() -> malloc_first_space= %i \n", (size_t) malloc_first_space);
    printf(" malloc: malloc_init() -> capacity= %i \n",  malloc_first_space->capacity());
  }

  malloc_initialized = true;
  if(do_print) printf("^-------------------------init done-------------------------^\n\n");
  return malloc_initialized;
}

extern "C" void malloc_overview()
{
  printf("v----------------------malloc overview----------------------v\n");
  Space* s = malloc_first_space;
  bool next = true;
  while(next)
  {

    printf("[[ Space-header: %i | Space-End: %i | Chunk-header: %i | Last-Chunk-End: %i ]]\n", sizeof(Space), s->get_end(), sizeof(Chunk), s->get_last()->get_end());
    s->print();
    s->print_chunks();
    printf("\n");

    if(s->next != NULL)
      s = s->next;
    
    else
      next = false;

  }

  printf("^----------------------malloc overview----------------------^\n");
}


extern "C" void *malloc(size_t size) throw()
{
  // initialize if not yet initialized ;)
  if(!malloc_initialized)
    if(!malloc_init())
      return 0;

  if(do_print) printf("\nv-------------------------malloc(%i)------------------------v\n", size);

  bool overview = false;
  if(size == 9) overview = true;

  if(do_print) printf(" malloc: malloc() -> unaligned size= %i\n", size);
  size = Chunk::align_size(size);
  if(do_print) printf(" malloc: malloc() ->   aligned size= %i\n", size);


  Chunk *used_chunk = NULL; 
  Space* s = malloc_first_space;
  bool space_proceed = true;

  // walking through existing spaces
  while(space_proceed)
  {
    if(s->can_hold(size)){
      // 1.) use a free chunk
      if(used_chunk == NULL){
        if(do_print) printf(" malloc: malloc() -> trying to reuse chunk in space %i\n",(size_t)s);
        used_chunk = s->find_chunk(size);
      }

      // 2.) append a new chunk to malloc_current_space
      if(used_chunk == NULL){
        if(do_print) printf(" malloc: malloc() -> trying to append chunk to space %i\n",(size_t)s);
        used_chunk = s->append_chunk(size);
      }
    }
    else if(do_print) printf(" malloc: malloc() -> space %i (%i) can't hold chunk(%i)\n",(size_t)s, s->capacity(), size);


    //////// proceed to next space or not?
    if(s->next != NULL && used_chunk == NULL)
      s = s->next;
    else
      space_proceed = false;
  }

  // 3.) reduce malloc_current_space

  // 4.) allocate new dataspace ( should return first chunk )
  if(used_chunk == NULL){ 
    if(do_print) printf(" malloc: malloc() -> allocating new space\n");
    append_space(size);
    used_chunk = malloc_current_space->append_chunk(size);
  }

  // there's nothing left to gain
  if(used_chunk == NULL){
    if(do_print) printf(" malloc: malloc() -> could not broker any chunks\n");
    return NULL;
  }

  // at this point we definitely have a Chunk we can use
  used_chunk->free = false;

  if(do_print) printf("^-----------------malloc done returning (%i)----------------^\n\n", used_chunk->addr());


  if(overview && do_print) 
  {
    malloc_overview();
    printf("\n\n\n");
  }
  return used_chunk->addr();
}


// XXX TODO FIXME
extern "C" void free(void *p) throw() 
{
  Chunk* c = Chunk::from_addr(p);
  c->free = true;
  enter_kdebug("free");
  p = p;
}
