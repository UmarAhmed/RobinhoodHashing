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

    uint32_t _mask;

    T* _array;
    std::size_t * _hashes;

    // If the hashtable is too full, we double in size and re-insert everything
    void grow() {
        auto old_array = _array;
        auto old_hashes = _hashes;
        auto old_capacity = _capacity;

        _capacity *= 2;
        _mask = _capacity - 1;
        _array = new T[_capacity];
        _hashes = new std::size_t[_capacity];

        // Set hashes to zero
        for (int i = 0; i < _capacity; ++i) {
            _hashes[i] = 0;
        }
        
        // Insert all old elements
        for (int i = 0; i < old_capacity; ++i) {
            if (old_hashes[i] != 0) {
                add_helper(std::move(old_array[i]));
            }
        }
        delete[] old_array;
        delete[] old_hashes;
    }

    size_t my_hash(const T & val) const {
        static auto hash_function = std::hash<T>();
        auto hashval = hash_function(val);
        hashval |= (hashval == 0); // set to 1 if 0
        return hashval;
    }

    void add_helper(T&& val) {
        auto hashval = my_hash(val); 
        int index = hashval & _mask; // use mask instead of mod bc power of 2
        int distance = 0;

        while (_hashes[index]) {
            // element already exists in the set so return
            if (_array[index] == val) {
                --_size;
                break;
            }

            // Compute the probing distance of the element at the current index
            const int cur_distance = (index + _capacity - (_hashes[index] & _mask) ) & _mask;

            // Swap with the current element if its probing distance is smaller
            if (cur_distance < distance) {
                std::swap(_hashes[index], hashval);
                std::swap(_array[index], val);
                distance = cur_distance;
            }

            index = (index + 1) & _mask;
            ++distance;
        }

        _hashes[index] = hashval;
        _array[index] = val;
    }


public:
    
    // Default constructor
    rh_set() {
        _size = 0;
        _capacity = INITIAL_SIZE;
        _mask = _capacity - 1;
        _array = new T[INITIAL_SIZE];
        _hashes = new std::size_t[INITIAL_SIZE];
        for (int i = 0; i < INITIAL_SIZE; ++i) {
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
    bool contains(const T val) const {
        const auto hashval = my_hash(std::move(val));
        int index = hashval & _mask;
        int distance = 0;

        while (_hashes[index]) {
            const int cur_distance = (index + _capacity - (_hashes[index] & _mask) ) & _mask;

            if (distance > cur_distance) {
                return false;
            }
            if (_array[index] == val) {
                return true;
            }
            index = (index + 1) & _mask;
            ++distance;
        }
        return false;
    }

    // Remove element from our table (true on removed, false otherwise)
    bool pop(const T val) {
        const auto hash_val = my_hash(std::move(val));
        int index = hash_val & _mask;
        int distance = 0;

        while (_hashes[index]) {
            const int cur_distance = (index + _capacity - (_hashes[index] & _mask) ) & _mask;

            if (distance > cur_distance) {
                return false; // not found
            }
            if (_array[index] == val) {
                shift(index);
                --_size;
                return true; // successfully removed
            }
            index = (index + 1) & _mask;
            ++distance;
        }
        return false; // not found
    }

    // called after deleting to shift elements back (fill in empty spot)
    void shift(int index) {
        int before = index;
        index = (index + 1) & _mask;

        while (_hashes[index]) {
            // compute probing distance for current element
            const int cur_distance = (index + _capacity - (_hashes[index] & _mask) ) & _mask;
            // if distance is 0, then we can stop
            if (cur_distance == 0) {
                break;
            }

            std::swap(_hashes[before], _hashes[index]);
            std::swap(_array[before], _array[index]);
            
            before = (before + 1) & _mask;
            index = (index + 1) & _mask;
        }
        
        // mark as erased
        _hashes[before] = 0;
        // NOTE not calling destructor
    }

    int size() const {
        return _size;
    }

    void print() const {
        for (int i = 0; i < _capacity; ++i) {
            if (_hashes[i]) {
                std::cout << _array[i] << std::endl;
            }
        }
    }
};


#endif
