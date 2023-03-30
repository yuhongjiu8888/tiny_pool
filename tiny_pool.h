#include <atomic>
#include <memory>

class tiny_mempool {
  protected:
    struct memNode {
        memNode* nextnode = nullptr;
    };

  protected:
    std::atomic<memNode*> m_free_head[16];

  private:
    tiny_mempool() {}

    ~tiny_mempool() {
        for (int i = 0; i < 16; i++) {
            if (m_free_head[i] != nullptr) {
                memNode* ptr = m_free_head[i];
                while (ptr != nullptr) {
                    auto nptr = ptr->nextnode;
                    free(ptr);
                    ptr = nptr;
                }
            }
            m_free_head[i] = nullptr;
        }
    }

    int getindex(int size) {
        static const unsigned int sizetable[16] = {8,  16, 24, 32, 40,  48,  56,  64,
                                                   72, 80, 88, 96, 104, 112, 120, 128};
        int __distance = 0;
        for (; __distance < 16; __distance++)
            if (sizetable[__distance] >= size) break;
        return __distance;
    }

  public:
    static tiny_mempool& instance() {
        static tiny_mempool pool;
        return pool;
    }

  public:
    void* alloc(int size) {
        if (size > 128) return malloc(size);
        int index = getindex(size);
        int realsize = (index + 1) << 3;
        memNode* p = m_free_head[index];
        if (p == nullptr)
            return malloc(realsize);
        else {
            while (!m_free_head[index].compare_exchange_weak(p, p->nextnode))
                if (p == nullptr) return malloc(realsize);
            return p;
        }
        return nullptr;
    }

    void delloc(void* ptr, int size) {
        if (ptr == nullptr) return;
        if (size > 128) return free(ptr);
        int index = getindex(size);
        memNode* pNew = (memNode*)ptr;
        pNew->nextnode = m_free_head[index];
        while (!(m_free_head[index].compare_exchange_weak(pNew->nextnode, pNew)))
            ;
    }

    /**
   * @brief report memory distribute in the pool.
   * @attention May cause undefined result if 
   * allocate memory use current pool before this
   * function return.
   */
    void report() {
        printf("\033[32m\033[1mtiny_mempool report\033[0m\n");
        printf("\033[34mindex\tnode size   node count\033[0m\n");
        for (int i = 0; i < 16; ++i) {
            int n = 0;
            memNode* p = m_free_head[i];
            while (p) {
                n++;
                p = p->nextnode;
            }
            printf("\033[31m%5d\t %3d \033[35mbyte\033[31m   %10d"
                   "\033[0m\n",
                   i, (i + 1) << 3, n);
        }
    }
};

template <class T>
class tiny_allocator {
  public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;

    tiny_allocator() {}

    tiny_allocator(tiny_allocator const&) {}

    tiny_allocator& operator=(tiny_allocator const&) { return *this; }

    template <class Other>
    tiny_allocator(tiny_allocator<Other> const&) {}

    template <class Other>
    tiny_allocator& operator=(tiny_allocator<Other> const&) {
        return *this;
    }

    pointer allocate(size_type count) {
        return (pointer)tiny_mempool::instance().alloc(count * sizeof(value_type));
    }

    void deallocate(pointer ptr, size_type count) {
        return tiny_mempool::instance().delloc(ptr, count * sizeof(value_type));
    }
};
