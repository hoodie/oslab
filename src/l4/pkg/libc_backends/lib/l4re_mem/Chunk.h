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

  static Chunk *from_addr(void*p)
  {
    return *(Chunk*)p - sizeof(Chunk);
  }

  Chunk *operator + (size_t size)
  {
    return (Chunk*)((size_t)this + size);
  }

  Chunk *operator - (size_t size)
  {
    return (Chunk*)((size_t)this - size);
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

  // TODO split_chunk untested
  bool split_chunk(size_t size)
  {
    // should be big enough so that the cutoff can still hold a Chunkheader and at least one l4_umword_t
    if(this->free && this->size > (size+sizeof(Chunk)+sizeof(l4_umword_t)))
    {
      size_t old_size = this->size;
      Chunk* old_next = this->next;

      Chunk *new_next = (Chunk*) ((size_t)this + sizeof(Chunk) + size);
      new_next->init(this->size - size -sizeof(Chunk));

      new_next->prev = this;
      new_next->next = old_next;

      this->next->prev = new_next;
      this->next = new_next;


      this->size = size;
      return true;
    }
    return false;
  }

  // TODO user pointer to next so you don't create to many cavities
  bool merge_right()
  {
    if(this->next->free)
    {
      Chunk* right = this->next;
      this->next = right->next;
      this->next->prev = this;
      this->size = align_size_down(this->size + right->size + sizeof(Chunk));
      return true;
    }
    return false;
    
  }


  size_t align_size_down(size_t size)
  {
    size_t scale = sizeof(l4_umword_t);
    if (size % scale == 0)
      return size;
    else{
      size_t x = size/scale;
      return x*scale;
    }
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
      return aligned;
    }
  }

  void print(size_t reference)
  {
    size_t p = (size_t) prev;
    size_t n = (size_t) next;
    size_t a = (size_t) this->addr();

    if(n > reference) n -= reference;
    if(p > reference) p -= reference;
    printf(" [ C {%i} | ", (size_t) this);
    printf( "size:%i | ", size );
    printf( "prev:%i | ", p);
    printf( "next:%i | ", n);
    printf( "addr:%i | ", a );
    if(this->free)
      printf( "\e[1;34mfree \e[0;32m" );
    else
      printf( "\e[1;31mused \e[0;32m" );
    printf("] \n");
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
