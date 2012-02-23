struct Space {
  size_t size;
  Space* prev;
  Space* next;
  bool empty;

  Space(size_t size)
  {
    init(size);
  }

  Space* init(size_t size)
  {
    this->prev = NULL;
    this->next = NULL;
    this->size = size - sizeof(Space);
    this->empty = true;
    return this;
  }

  Chunk* get_last()
  {
    Chunk* c = this->first();
    // if(this->empty) return c;
    while(c->next != NULL)
      c = c->next;
    return c;
  }

  Chunk *first()
  {
    return (Chunk*)((size_t)this + sizeof(Space)); 
  }

  size_t get_end()
  {
    return (size_t)this + sizeof(Space)  + this->size - 1;
  }

  size_t capacity()
  {
    size_t start = (size_t) this;
    size_t end = this->get_end() - start;
    size_t last_end = this->get_last()->get_end() - start;

   //printf(" Space::capacity() size: %i  end : %i   last.end %i \n", size, end, last_end);
    return (end - last_end); 
  }

  bool can_hold(size_t needed_size)
  {
    size_t tail = capacity();
    size_t needed = needed_size + sizeof(Chunk);
    return tail > needed;
    // this->get_last()->get_after();
  }

  // searches current Space for reusable Chunk and returns reference
  Chunk *find_chunk(size_t size)
  {
    // printf(" malloc:Space::find_chunk() -> size= %i\n", size);
    Chunk* current = this->first();
    while (current->next != NULL)
    {
      if ( current->free && current->size == size)
        return current;
      current = current->next;
    }
    return NULL;
  }

  Chunk *append_chunk(size_t size)
  {
    // printf(" malloc:Space::append_chunk() -> size= %i\n", size);
    Chunk *last = this->get_last();
    Chunk* appended = NULL;

    // only true for first chunk ( created with Space ) 
    if( last->size == 0 )
    {
      last->size = size;
      appended = last;
    }
    // every other time
    else if( (this->can_hold(size)) )
    {
      appended = (Chunk*) last->get_after();
      appended->size = size;
      appended->prev = last;
      appended->next = NULL;
      last->next = appended;
    }

    return appended;
  }

  static size_t align_size(size_t size) 
  {
    if (size % L4_PAGESIZE == 0)
      return size;
    else if (size < L4_PAGESIZE)
      return L4_PAGESIZE;
    else
    {
      // next bigger number of pages
      double r = size/L4_PAGESIZE;
      return (ceil(r)+1)*L4_PAGESIZE;
    }
  }

  Chunk* print_chunks()
  {
    Chunk* c = this->first();
    bool next = true;
    while(next)
    {
      c->print((size_t) this);
      if(c->next != NULL)
        c = c->next;
      else
        next = false;
    }
    return c;
  }

  void print()
  {
    printf(" [ S {%i} | ", (size_t)this);
    printf("size:%u | ", size);
    printf("next:%i | ", (size_t)next);
    printf("prev:%i | ", (size_t)prev);
    printf("capacity:%i ]\n", (size_t)this->capacity());
  }

};
