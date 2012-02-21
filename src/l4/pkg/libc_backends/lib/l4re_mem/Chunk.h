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
    this->size = size;
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


  void print()
  {
    printf("[C this %p| ", this);
    printf( "C size %u| ", size);
    printf( "C next %p| ", next);
    printf( "C prev %p]\n", prev);
  }
//  Chunk* next(){
//    return (Chunk*)((size_t)this + sizeof(Chunk));
//  }

//  static size_t round(size_t size){
//    return (size >> 3) << 3;
//  }

};
