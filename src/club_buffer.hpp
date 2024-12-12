#ifndef CLUB_BUFFER_HPP_
#define CLUB_BUFFER_HPP_

#include "club_context.hpp"
#include "club_event.hpp"

namespace club
{
    BufferPtr CreateBuffer();
    BufferPtr CreateBuffer(ConstContextPtr context, std::size_t size, cl_mem_flags flags = CL_MEM_READ_WRITE);

    class Buffer : public std::enable_shared_from_this<Buffer>
    {
    public:
        virtual ~Buffer();

        static BufferPtr Create();
        BufferPtr GetPtr();
        ConstBufferPtr GetPtr() const;

        bool Init(ConstContextPtr context, cl_mem_flags flags, std::size_t size);

        EventPtr Read(std::size_t offset, std::size_t size, void* ptr, cl_bool block = CL_FALSE);
        EventPtr Write(std::size_t offset, std::size_t size, void* ptr, cl_bool block = CL_FALSE);
        
        const cl_mem& Get() const;
        const cl_context& GetContext() const;

        const BufferInfo& GetInfo() const;

    protected:
        Buffer() = default;

        bool Initialize(ConstContextPtr context, cl_mem_flags flags, std::size_t size);

        BufferInfo GetBufferInfo(cl_mem arg1) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetBufferInfo(cl_mem buffer, cl_mem_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetBufferInfo(cl_mem buffer, cl_mem_info info) const;

        bool initialized_{ false };

        ConstContextPtr context_{ nullptr };
        std::size_t size_{ 0 };

        cl_mem buffer_;
        BufferInfo bufferInfo_;
    };
} // namespace club

#endif