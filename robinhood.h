#ifndef ROBINHOOD_H
#define ROBINHOOD_H


#include <functional>
#include <iostream>


template <typename T>
class rh_set {
    static const int LOAD_FACTOR = 80;
    static const int INITIAL_SIZE = 128;

    std::size_t _size;
    std::size_t _capacity;

    T* _array;
    std::size_t * _hashes;

    // If the hashtable is too full, we double in size and re-insert everything
    void grow() {
        auto old_array = _array;
        auto old_hashes = _hashes;
        auto old_capacity = _capacity;

        _capacity *= 2;
        _array = new T[_capacity];
        _hashes = new std::size_t[_capacity];

        // Set hashes to zero
        for (int i = 0; i < _capacity; i++) {
            _hashes[i] = 0;
        }
        
        // Insert all old elements
        for (int i = 0; i < old_capacity; i++) {
            if (old_hashes[i] != 0) {
                add_helper(std::move(old_array[i]));
            }
        }
        delete[] old_array;
        delete[] old_hashes;
    }

    static size_t my_hash(T & val) {
        static auto hash_function = std::hash<T>();
        auto hashval = hash_function(val);
        hashval |= (hashval == 0); // set to 1 if 0
        return hashval;
    }

    void add_helper(T&& val) {
        const int mask = _capacity - 1;

        auto hashval = my_hash(val); 
        int index = hashval & mask; // use mask instead of mod bc power of 2
        int distance = 0;

        while (_hashes[index] != 0) {
            // Compute the probing distance of the element at the current index
            int cur_distance = (index + _capacity - (_hashes[index] & mask) ) & mask;

            // Swap with the current element if its probing distance is smaller
            if (cur_distance < distance) {
                std::swap(_hashes[index], hashval);
                std::swap(_array[index], val);
                distance = cur_distance;
            }

            index = (index + 1) & mask;
            distance++;
        }

        _hashes[index] = hashval;
        _array[index] = val;
    }


public:
    
    // Default constructor
    rh_set() {
        _size = 0;
        _capacity = INITIAL_SIZE;
        _array = new T[INITIAL_SIZE];
        _hashes = new std::size_t[INITIAL_SIZE];
        for (int i = 0; i < INITIAL_SIZE; i++) {
            _hashes[i] = 0;
        }
    }

    // Destructor (calls destructor of each item)
    ~rh_set() {
        delete[] _array;
        delete[] _hashes;
    }


    // Adds element if not already in
    void add(T val) {
        // Grow if needed first
        int threshold = _capacity * LOAD_FACTOR / 100;
        if (++_size >= threshold) {
            grow();
        }

        add_helper(std::move(val));
    }

    // Returns true if val is in our hashtable, false otherwise
    bool contains(T val) {
        const int mask = _capacity - 1;

        auto hashval = my_hash(std::move(val));
        int index = hashval & mask;
        int distance = 0;

        while (_hashes[index] != 0 && index < _capacity) {
            int cur_distance = (index + _capacity - (_hashes[index] & mask) ) & mask;

            if (distance > cur_distance) {
                return false;
            }
            if (_array[index] == val) {
                return true;
            }
            index = (index + 1) & mask;
            distance++;
        }
        return false;
    }

    int size() {
        return _size;
    }

    void print() {
        for (int i = 0; i < _capacity; i++) {
            if (_hashes[i] != 0) {
                std::cout << _array[i] << std::endl;
            }
        }
    }
};


// TODO: need to add removing functionality
// TODO do I need to have copies of the elements in _array on the heap?
#endif
