#ifndef __ARRAY2D_H__
#define __ARRAY2D_H__

#include <vector>
#include <cassert>

template<typename T>
class Array2D 
{
    public:
        typedef size_t size_type;
        typedef T value_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef typename std::vector<value_type>::iterator iterator;
        typedef typename std::vector<value_type>::const_iterator const_iterator;

        Array2D() : Array2D(0, 0, value_type())
        {

        }

        Array2D(size_type width, size_type height) : Array2D(width, height, value_type())
        {
        }

        Array2D(size_type width, size_type height, value_type val) : width(width), height(height), storage(width * height, val)
        {
        }

        virtual ~Array2D() { };

        size_type Width() const
        {
            return width;
        }

        size_type Height() const
        {
            return height;
        }

        reference operator()(size_type x, size_type y) 
        {
            assert(x < width && y < height);
            return storage[x + y * width];
        }

        const_reference operator()(size_type x, size_type y) const
        {
            assert(x < width && y < height);
            return storage[x + y * width];
        }

        void Clear()
        {
            std::fill(storage.begin(), storage.end(), T());
        }

        void Resize(size_type width, size_type height)
        {
            this->width = width;
            this->height = height;
            Clear();
            storage.resize(width * height);
        }

        const value_type *Data() const
        {
            return storage.data();
        }

        iterator begin() 
        {
            return storage.begin();
        }

        iterator end() 
        {
            return storage.end();
        }

        const_iterator begin() const
        {
            return storage.begin();
        }

        const_iterator end() const
        {
            return storage.end();
        }

    private:
        size_type width, height;
        std::vector<value_type> storage;
};

#endif