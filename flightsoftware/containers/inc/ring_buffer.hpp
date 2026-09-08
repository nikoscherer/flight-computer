#pragma once

#include <array>
#include <cstddef>

template <typename T, size_t N> class RingBuffer {
public:
  RingBuffer() {}

  void push(T *in_data) {
    incrementPtr(head_);
    if (head_ == tail_) {
      incrementPtr(tail_);
    }
    buffer_[head_] = in_data;

    if (size_ != N) {
      if (size_ == 0) {
        ++tail_;
      }

      ++size_;
    }
  }

  bool pop(T *&out_data) {
    if (buffer_[tail_] == nullptr) {
      return false;
    }

    out_data = buffer_[tail_];
    buffer_[tail_] = nullptr;
    if (tail_ != head_) {
      incrementPtr(tail_);
    }

    if (size_ != 0) {
      --size_;
    }

    return true;
  }

  bool peakFront(T *&out_data) {
    if (buffer_[head_] == nullptr) {
      return false;
    }

    out_data = buffer_[head_];

    return true;
  }

  size_t getSize() { return size_; }

private:
  std::array<T *, N> buffer_{};

  size_t size_ = 0;
  size_t head_ = 0;
  size_t tail_ = 0;

  void incrementPtr(size_t &ptr) { ptr = (ptr + 1) % N; }
};