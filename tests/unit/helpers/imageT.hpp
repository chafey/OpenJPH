#pragma once
#include <ojph_base.h>
#include <vector>

template<typename T> class ImageT {
    public:
        ImageT(size_t width, size_t height) : width_(width), height_(height) {
            data_.resize(width_ * height_);
        }

        bool operator== (const ImageT& rhs) {
            return  width_ == rhs.width_ &&
                    height_ == rhs.height_ &&
                    data_ == rhs.data_;
        }

        size_t width() const {return width_;}
        size_t height() const {return height_;}

        const T* data() const {return data_.data();}
        T* data() {return data_.data();}
    private:
        size_t width_;
        size_t height_;
        std::vector<T> data_;
};

typedef ImageT<unsigned char> Image8u;
typedef Image8u Image;
typedef ImageT<ojph::si32> Imagesi32;
typedef ImageT<ojph::ui32> Imageui32;