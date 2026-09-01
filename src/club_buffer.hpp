#ifndef CLUB_BUFFER_HPP_
#define CLUB_BUFFER_HPP_

#include "club_context.hpp"
#include "club_event.hpp"

namespace club
{
    BufferPtr CreateBuffer();
    BufferPtr CreateBuffer(ContextPtr context, std::size_t size, cl_mem_flags flags = CL_MEM_READ_WRITE);

    class Buffer : public std::enable_shared_from_this<Buffer>
    {
    public:
        virtual ~Buffer();

        static BufferPtr Create();
        BufferPtr GetPtr();
        ConstBufferPtr GetPtr() const;

        bool Init(ContextPtr context, cl_mem_flags flags, std::size_t size);

        EventPtr Read(cl_command_queue queue, std::size_t offset, std::size_t size, void* ptr, cl_bool block = CL_FALSE);
        EventPtr Write(cl_command_queue queue, std::size_t offset, std::size_t size, const void* ptr, cl_bool block = CL_FALSE);
        
        const cl_mem& Get() const;
        const BufferInfo& GetInfo() const;

    protected:
        Buffer() = default;

        BufferInfo GetBufferInfo(cl_mem arg1) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetBufferInfo(cl_mem buffer, cl_mem_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetBufferInfo(cl_mem buffer, cl_mem_info info) const;

        std::size_t size_{ 0 };
        cl_mem buffer_{ nullptr };
        BufferInfo bufferInfo_;
    };
} // namespace club

#endif