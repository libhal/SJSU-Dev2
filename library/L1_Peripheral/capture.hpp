#pragma once
#include <cstdint>
#include "utility/status.hpp"

namespace sjsu
{
    class capture
    {
        public:
        virtual Status Initialize() const = 0;
    };


} /* namespace sjsu */