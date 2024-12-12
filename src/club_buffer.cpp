#include "club_buffer.hpp"
#include <iostream>
#include <memory>

namespace club
{
    BufferPtr CreateBuffer()
    {
        return Buffer::Create();
    }
    BufferPtr CreateBuffer(ConstContextPtr context, std::size_t size, cl_mem_flags flags)
    {
        auto res = Buffer::Create();

        if (!res->Init(context, flags, size))
        {
            res = nullptr;
        }

        return res;
    }
    Buffer::~Buffer()
    {
        clReleaseMemObject(buffer_);
    }
    BufferPtr Buffer::Create()
    {
        class MakeSharedEnabler : public Buffer
        {
        };

        auto res = std::make_shared<MakeSharedEnabler>();
        return res;
    }
    BufferPtr Buffer::GetPtr()
    {
        return shared_from_this();
    }
    ConstBufferPtr Buffer::GetPtr() const
    {
        return const_cast<Buffer*>(this)->GetPtr();
    }
    bool Buffer::Init(ConstContextPtr context, cl_mem_flags flags, std::size_t size)
    {
        bool res = false;

        if (!initialized_)
        {
            res = Initialize(context, flags, size);
        }

        return res;
    }
    bool Buffer::Initialize(ConstContextPtr context, cl_mem_flags flags, std::size_t size)
    {
        bool res = false;
        Error error;

        if (context != nullptr)
        {
            context_ = context;
            size_ = size;
            buffer_ = clCreateBuffer(context_->Get(), flags, size_, nullptr, &error);

            if (error != CL_SUCCESS)
            {
                logger::Error(header, "Buffer could not be created: " + messages.at(error));
            }
            else
            {
                bufferInfo_ = GetBufferInfo(buffer_);
                initialized_ = true;

                res = true;
            }
        }
        else
        {
            logger::Error(header, "Buffer not created: context pointer is null");
        }

        return res;
    }
    EventPtr Buffer::Read(std::size_t offset, std::size_t size, void* ptr, cl_bool block)
    {
        EventPtr res {nullptr};
        cl_event event;
        Error error;

        error = clEnqueueReadBuffer(context_->GetQueue(), buffer_, block, offset, size, ptr, 0, NULL, &event);

        if (error != CL_SUCCESS)
        {
            logger::Error(header, "Error reading buffer: " + messages.at(error));
        }
        else
        {
            res = CreateEvent(event);
        }

        return res;
    }
    EventPtr Buffer::Write(std::size_t offset, std::size_t size, void* ptr, cl_bool block)
    {
        EventPtr res {nullptr};
        cl_event event;
        Error error;

        error = clEnqueueWriteBuffer(context_->GetQueue(), buffer_, block, offset, size, ptr, 0, NULL, &event);

        if (error != CL_SUCCESS)
        {
            logger::Error(header, "Error writing buffer: " + messages.at(error));
        }
        else
        {
            res = CreateEvent(event);
        }

        return res;
    }
    const cl_mem& Buffer::Get() const
    {
        return buffer_;
    }
    const cl_context& Buffer::GetContext() const
    {
        return context_->Get();
    }
    const BufferInfo& Buffer::GetInfo() const
    {
        return bufferInfo_;
    }
    BufferInfo Buffer::GetBufferInfo(cl_mem arg1) const
    {
        BufferInfo res;

        res.type = GetBufferInfo<cl_mem_info>(arg1, CL_MEM_TYPE);
        res.flags = GetBufferInfo<cl_mem_info>(arg1, CL_MEM_FLAGS);
        res.size = GetBufferInfo<std::size_t>(arg1, CL_MEM_SIZE);
        res.hostPtr = GetBufferInfo<void*>(arg1, CL_MEM_HOST_PTR);
        res.context = GetBufferInfo<cl_context>(arg1, CL_MEM_CONTEXT);

        return res;
    }
    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Buffer::GetBufferInfo(cl_mem buffer, cl_mem_info info) const
    {
        std::size_t size;
        T res;

        clGetMemObjectInfo(buffer, info, 0, NULL, &size);
        clGetMemObjectInfo(buffer, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Buffer::GetBufferInfo(cl_mem buffer, cl_mem_info info) const
    {
        std::size_t size;
        T res;

        clGetMemObjectInfo(buffer, info, 0, NULL, &size);
        res.resize(size);
        clGetMemObjectInfo(buffer, info, size, &res[0], 0);

        return res;
    }
} // namespace club