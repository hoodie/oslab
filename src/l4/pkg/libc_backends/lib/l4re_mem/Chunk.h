struct Chunk {
  size_t size;
  Chunk* prev;
  Chunk* next;
  bool free;

  Chunk(size_t size)
  {
    init(size);
  }

  Chunk* init(size_t size)
  {
    this->prev = NULL;
    this->next = NULL;
    this->free = true;
    this->size = size-sizeof(Chunk);
    return this;
  }


  void *addr()
  {
    return (void*)(
        (size_t)this + sizeof(Chunk)
        ); 
  }

  size_t get_end()
  {
    return (size_t)this + sizeof(Chunk)  + size - 1;
  }

  size_t get_after()
  {
    return (size_t)this + sizeof(Chunk)  + size;
  }


  bool can_be_split(size_t size)
  {
    size_t cut_off  = this->size - (sizeof(Chunk) + size);
    size_t new_size = this->size - cut_off;

    bool long_enough     = size > cut_off;
    bool still_aligned   = this->align_size(new_size) == new_size;
    bool cut_off_aligned = this->align_size(cut_off) == cut_off;

    return false;
  }

  bool split_chunk(size_t size)
  {
    return false;
  }

  static size_t align_size(size_t size) 
  {
    size_t scale = sizeof(l4_umword_t);
    if (size % scale == 0)
      return size;
    else if (size < scale)
      return scale;
    else
    {
      // next bigger number of pages
      double r = size/scale;
      size_t aligned = (ceil(r)+1)*scale;
      printf("********** aligning %i to %i\n", size, aligned);
      return aligned;
    }
  }

  void print(size_t reference)
  {
    size_t p = (size_t) prev;
    size_t n = (size_t) next;

    if(n > reference) n -= reference;
    if(p > reference) p -= reference;
    printf("[C {%i} | ", (size_t) this - reference);
    printf( "size:%i | ", (size_t) size );
    printf( "prev:%i | ", (size_t) p);
    printf( "next:%i | ", (size_t) n);
    printf( "addr:%i ]", (size_t) this );
    printf("\n");
  }

  //  Chunk* next()
  //  {
  //    return (Chunk*)((size_t)this + sizeof(Chunk));
  //  }

  //  static size_t round(size_t size)
  //  {
  //    return (size >> 3) << 3;
  //  }

};
