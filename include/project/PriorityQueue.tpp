#pragma once
#include <algorithm>
#include <stdexcept>
#include <utility>

template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K, V, Compare>::AdaptableHeapPriorityQueue() : heap_{}, comp_{}, nextId_{1} {}
template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K, V, Compare>::AdaptableHeapPriorityQueue(Compare comp) : heap_{}, comp_{std::move(comp)}, nextId_{1} {}
template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K, V, Compare>::~AdaptableHeapPriorityQueue()
{
    for (auto e : heap_)
        delete e;
}
template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K, V, Compare>::AdaptableHeapPriorityQueue(AdaptableHeapPriorityQueue &&other) noexcept : heap_(std::move(other.heap_)), comp_(std::move(other.comp_)), nextId_(other.nextId_)
{
    for (std::size_t i = 0; i < heap_.size(); ++i)
        heap_[i]->index = i;
    other.heap_.clear();
    other.nextId_ = 1;
}
template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K, V, Compare> &AdaptableHeapPriorityQueue<K, V, Compare>::operator=(AdaptableHeapPriorityQueue &&other) noexcept
{
    if (this == &other)
        return *this;
    for (auto e : heap_)
        delete e;
    heap_ = std::move(other.heap_);
    comp_ = std::move(other.comp_);
    nextId_ = other.nextId_;
    for (std::size_t i = 0; i < heap_.size(); ++i)
        heap_[i]->index = i;
    other.heap_.clear();
    other.nextId_ = 1;
    return *this;
}
template <typename K, typename V, typename Compare>
std::size_t AdaptableHeapPriorityQueue<K, V, Compare>::size() const noexcept { return heap_.size(); }
template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K, V, Compare>::empty() const noexcept { return heap_.empty(); }
template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K, V, Compare>::less(EntryPtr a, EntryPtr b) const { return comp_(a->key, b->key); }
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::swapEntries(std::size_t i, std::size_t j) noexcept
{
    std::swap(heap_[i], heap_[j]);
    heap_[i]->index = i;
    heap_[j]->index = j;
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::upHeap(std::size_t i)
{
    while (i > 0)
    {
        std::size_t p = (i - 1) / 2;
        if (!less(heap_[i], heap_[p]))
            break;
        swapEntries(i, p);
        i = p;
    }
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::downHeap(std::size_t i)
{
    while (true)
    {
        std::size_t l = 2 * i + 1;
        if (l >= heap_.size())
            break;
        std::size_t r = l + 1, c = l;
        if (r < heap_.size() && less(heap_[r], heap_[l]))
            c = r;
        if (!less(heap_[c], heap_[i]))
            break;
        swapEntries(i, c);
        i = c;
    }
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::bubble(std::size_t i)
{
    if (i > 0 && less(heap_[i], heap_[(i - 1) / 2]))
        upHeap(i);
    else
        downHeap(i);
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::validateHandle(EntryPtr e) const
{
    if (!e || std::find(heap_.begin(), heap_.end(), e) == heap_.end())
        throw std::invalid_argument("Invalid or stale priority-queue entry handle");
}
template <typename K, typename V, typename Compare>
typename AdaptableHeapPriorityQueue<K, V, Compare>::EntryPtr AdaptableHeapPriorityQueue<K, V, Compare>::insert(const K &key, const V &value)
{
    // TODO: allocate, append, assign index/id, then up-heap.
    EntryPtr e = new Entry{key, value, heap_.size()};
    heap_.push_back(e);
    upHeap(e->index);
    return e;
}
template <typename K, typename V, typename Compare>
const typename AdaptableHeapPriorityQueue<K, V, Compare>::Entry &AdaptableHeapPriorityQueue<K, V, Compare>::min() const
{
    if (empty())
        throw std::out_of_range("min() on empty priority queue");
    return *heap_.front();
}
template <typename K, typename V, typename Compare>
typename AdaptableHeapPriorityQueue<K, V, Compare>::Entry AdaptableHeapPriorityQueue<K, V, Compare>::removeMin()
{
    if (empty())
        throw std::out_of_range("removeMin() on empty priority queue");
    EntryPtr e = heap_.front();
    swapEntries(0, heap_.size() - 1);
    heap_.pop_back();
    downHeap(0);
    return *e;
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::remove(EntryPtr e)
{
    validateHandle(e);
    std::size_t i = e->index;
    swapEntries(i, heap_.size() - 1);
    heap_.pop_back();
    bubble(i);
    delete e;
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::replaceKey(EntryPtr e, const K &k)
{
    validateHandle(e);
    e->key = k;
    bubble(e->index);
    /* TODO */
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::replaceValue(EntryPtr e, const V &v)
{
    validateHandle(e);
    e->value = v;
    bubble(e->index);
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K, V, Compare>::buildHeap(std::vector<std::pair<K, V>> entries)
{
    heap_.clear();
    for (std::size_t i = 0; i < entries.size(); ++i)
    {
        EntryPtr e = new Entry{entries[i].first, entries[i].second, i};
        heap_.push_back(e);
    }
    for (std::size_t i = (heap_.size() - 2) / 2; i != std::size_t(-1); --i)
        downHeap(i);
}
template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K, V, Compare>::validHeap() const noexcept
{
    for (std::size_t i = 1; i < heap_.size(); ++i)
    {
        std::size_t p = (i - 1) / 2;
        if (less(heap_[i], heap_[p]) || heap_[i]->index != i)
            return false;
    }
    return true;
}
template <typename T, typename Compare>
void heapSort(std::vector<T> &values, Compare comp)
{
    // TODO: implement in-place heap sort. Default comparator must produce ascending order.
    const std::size_t n = values.size();
    if (n < 2)
        return;

    auto downHeap = [&](std::size_t i, std::size_t heapSize)
    {
        while (true)
        {
            std::size_t l = 2 * i + 1;
            std::size_t r = l + 1;
            std::size_t c = i;
            if (l < heapSize && comp(values[c], values[l]))
                c = l;
            if (r < heapSize && comp(values[c], values[r]))
                c = r;
            if (c == i)
                break;
            std::swap(values[i], values[c]);
            i = c;
        }
    };
    // Build heap in-place
    for (std::size_t i = n / 2; i > 0; --i)
        downHeap(i - 1, n);
    // Extract elements from heap one by one
    for (std::size_t end = n; end > 1; --end)
    {
        std::swap(values[0], values[end - 1]);
        downHeap(0, end - 1);
    }
}
