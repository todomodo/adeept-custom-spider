/*
 * based on Queue.h from By Steven de Salas
 * https://github.com/sdesalas/Arduino-Queue.h/blob/master/Queue.h
 * implements a circular queue
 */
#pragma once


#define TDM_MAX_QUEUE_SIZE  256

namespace tdm {

  template <typename T>
  class Queue  {
    
    private:
      int _front, _back, _count;
      T *_data;
      int _maxitems;

    public:
      Queue(int maxitems = TDM_MAX_QUEUE_SIZE) { 
        _front = 0;
        _back = 0;
        _count = 0;
        _maxitems = maxitems;
        _data = new T[maxitems + 1];   
      }
      
      ~Queue() {
        delete[] _data;  
      }
      
      int count();
      
      int front();
      
      int back();
      
      void push(const T &item);
      
      T peek();
      
      T pop();
      
      void clear();
      
  }; //class Queue

  template<typename T>
  int Queue<T>::count() {
    return _count;
  }
  
  template<typename T>
  int Queue<T>::front() {
    return _front;
  }
  
  template<typename T>
  int Queue<T>::back() {
    return _back;
  }
  
  template<typename T>
  void Queue<T>::push(const T &item) {
    if(_count < _maxitems) { // Drops out when full
      _data[_back++]=item;
      ++_count;
      // Check wrap around
      if (_back > _maxitems)
        _back -= (_maxitems + 1);
    }
  }
  
  template<typename T>
  T Queue<T>::pop() {
    if(_count <= 0) return T(); // Returns empty
    else {
      T result = _data[_front];
      _front++;
      --_count;
      // Check wrap around
      if (_front > _maxitems) 
        _front -= (_maxitems + 1);
      return result; 
    }
  }
  
  template<typename T>
  T Queue<T>::peek() {
    if(_count <= 0) return T(); // Returns empty
    else return _data[_front];
  }
  
  template<typename T>
  void Queue<T>::clear() {
    _front = _back;
    _count = 0;
  }
  
}; //namespace
