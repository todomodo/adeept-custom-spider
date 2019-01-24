/*
 * Vector is implemented as a dynamically allocated array. The memory for this array is allocated 
 * in the constructor. As more elements are inserted the array is dynamically increased in size. 
 * A constructor without parameter creates an array with a default size. Another constructor with
 * integer parameter creates an array of the specified size. The destructor deletes the memory
 * allocated for the array.
 * 
 * References:
 * 
 *      https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/std/vector
 *      https://codefreakr.com/how-is-c-stl-implemented-internally/
 *      http://www.cplusplus.com/reference/vector/vector/
 */

const uint8_t _DEFAULT_VECTOR_SIZE = 4;
 
template <typename T>
class vector {
  public :
  
    // constructors
    vector()
      : array_( new T[_DEFAULT_VECTOR_SIZE] ),
        reserved_size_( _DEFAULT_VECTOR_SIZE ),
        size_( 0 )
    { }
    
    vector(uint8_t n) // create vector with n default elements
      : array_( new T[n] ),
        reserved_size_( n ),
        size_( 0 )
      { }
  
    // destructor
    ~vector() { delete[] array_; }

    //member functions
    uint8_t size() const;
    bool empty() const;
    void push_back(const T &t);
    void pop_back();
    T &operator[](uint8_t index);
    vector<T> &operator = (const vector<T> &);
    void clear();

     /*
     * The forward iterator iterates through the vector elements starting from index zero and in increasing
     * index order. Because the elements of the vector are stored in an contiguous array, a pointer of element
     * type can function as a forward iterator. This shows that a simple pointer can work as an iterator hence
     * it is often said that anything that behaves like an iterator is an iterator.
     */
    typedef T* iterator;
    inline iterator begin() { return array_; }
    inline iterator end(){ return array_ + size_; }
    
  private :
  
    T *array_;
    uint8_t size_;
    uint8_t reserved_size_;
      
    void resize(uint8_t n); 
};


/* 
 * an auto pointer to use with vectors 
 */
template <typename T>
class auto_array {
    public :
    auto_array(T *t) : ptr_( t )
    { }

    ~auto_array()
    { delete[] ptr_; }

    T *operator->()
    { return ptr_; }

    T *release()
    { 
      T *tmp( ptr_ );
      ptr_ = 0;
      return tmp;
    }

    T &operator[](uint8_t i)
    { return ptr_[i]; }

    private :
    T *ptr_;
};

/*
 * The most frequently used function to add elements to vector is push_back. The function adds
 * the element at the end of the vector ie. after the current last element. This is accomplished
 * by putting the element at the size_th position. However that is not sufficient because vector
 * is a dynamically increasing container hence if the currently allocated memory is not sufficient
 * to hold the element then more memory should be allocated. So, see that there is sufficient memory
 * to hold the element, if not allocate more memory and then insert the element
 * 
 */
template <typename T>
void vector<T>::push_back(const T &t) {
  // if we've run out of space, allocate more memory
  if(size_ == reserved_size_)
      resize(reserved_size_ + _DEFAULT_VECTOR_SIZE);
  
  // size_ when used as an index, points to the next position after
  // the last element in the vector
  array_[size_] = t;
  
  // now there is one more element in the vector, so increase the size
  size_++;
}


/*
 * The resize function is used to set the size of the reserved memory. Although this function is public
 * and can be called by client code to change the actual size of the memory held by the vector it is
 * used internally for the same purpose. Here is the implementation of the function. It makes  the size
 * of the internal array exactly n
 */
template <typename T>
void vector<T>::resize(uint8_t n) {
  if(n > reserved_size_) {
    /* if requested size is more than the current size, allocate a new array of larger capacity
     * copy the old array to new array and destroy old array
     */
      auto_array<T> new_array( new T[n] );
      for(uint8_t i=0; i<size_; i++) {
          new_array[i] = array_[i];
      }  
      delete[] array_;
      array_ = new_array.release();
      reserved_size_ = n;
  }
}

/*
 * true if empty
 */
template <typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}

/*
 * return the size of the array
 */
template <typename T>
uint8_t vector<T>::size() const {
    return size_;
}

/*
 * remove last element
 */
template <typename T>
void vector<T>::pop_back() {
  if (size_>0) size_--;
}

/*
 * copy assignment
 */
template <typename T>
vector<T> &vector<T>::operator = (const vector<T> &other) {
    auto_array<T> new_array( new T[other.size_] );
    for(uint8_t i=0; i<other.size_; i++) {
      new_array[i] = array_[i];
    }  
    delete[] array_;
    array_ = new_array.release();
    return *this;
}

/*
 * clear the array
 */
template <typename T>
void vector<T>::clear() {
    delete[] array_;
    reserved_size_ = _DEFAULT_VECTOR_SIZE;
    array_ =  new T[reserved_size_];
    size_ = 0;    
}

/*
 * index operator
 */
template <typename T>
T& vector<T>::operator[](uint8_t index) {
    return array_[index];
}
