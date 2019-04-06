#pragma once

#include <cstring>
#include <cstdint>

namespace r2d2 {
    enum class queue_optimization : bool {
        READ = false,
        WRITE = true
    };

    /**
     * Simple queue implementation.
     *
     * @tparam T
     * @tparam MaxSize
     * @tparam Optimization
     */
    template<typename T, size_t MaxSize, queue_optimization Optimization = queue_optimization::WRITE>
    class queue_c {
    protected:
        T buffer[MaxSize] = {};
        size_t index = 0;

    public:
        /**
         * Put an item on the queue.
         *
         * @param item
         */
        void push(const T &item) {
            if constexpr (Optimization == queue_optimization::WRITE) {
                buffer[index] = item;
            } else {
                if constexpr (std::is_pod_v<T>) {
                    memmove(
                        (void *) (buffer + 1),
                        (const void *) buffer,
                        index
                    );
                } else {
                    for (size_t i = index; i != 0; i--) {
                        buffer[i] = buffer[i - 1];
                    }
                }

                buffer[0] = item;
            }

            index += 1;
        }

        /**
         * Pop an item from the queue.
         */
        void pop() {
            if constexpr (Optimization == queue_optimization::WRITE) {
                if constexpr (std::is_pod_v<T>) {
                    /*
                     * Since the destination address is before
                     * the source address, a memcpy can be used instead
                     * of memmove.
                     */
                    memcpy(
                        (void *) buffer,
                        (const void *) (buffer + 1),
                        index
                    );
                } else {
                    for (size_t i = 1; i < index; i++) {
                        buffer[i - 1] = buffer[i];
                    }
                }
            }

            index -= 1;
        }

        /**
         * Get the next item from the queue
         * and pop.
         */
        T copy_and_pop() {
            T item = front();
            pop();

            return item;
        }

        /**
         * Get the next in the queue.
         */
        T &front() {
            if constexpr (Optimization == queue_optimization::WRITE) {
                return buffer[0];
            } else {
                return buffer[index - 1];
            }
        }

        /**
         * Get the next in the queue.
         */
        T const &front() const {
            if constexpr (Optimization == queue_optimization::WRITE) {
                return buffer[0];
            } else {
                return buffer[index - 1];
            }
        }

        /**
         * Get the last item in the queue.
         */
        T &back() {
            if constexpr (Optimization == queue_optimization::WRITE) {
                return buffer[index - 1];
            } else {
                return buffer[0];
            }
        }

        /**
         * Get the last item in the queue.
         */
        T const &back() const {
            if constexpr (Optimization == queue_optimization::WRITE) {
                return buffer[index - 1];
            } else {
                return buffer[0];
            }
        }

        /**
         * Get the current size of the queue.
         *
         * @return size_t
         */
        size_t size() const {
            return index;
        }

        /**
         * Is the queue empty?
         *
         * @return
         */
        bool empty() const {
            return size() == 0;
        }

        /**
         * Is the queue full?
         *
         * @return
         */
        bool full() const {
            return size() == max_size();
        }

        /**
         * Get the maximum size of the queue.
         *
         * @return constexpr size_t
         */
        constexpr size_t max_size() const {
            return MaxSize;
        }

        /**
         * Is this queue write optimized or
         * read optimized?
         *
         * @return
         */
        constexpr queue_optimization optimized_for() const {
            return Optimization;
        }
    };
}