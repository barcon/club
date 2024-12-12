#include "club_context.hpp"

namespace club
{
    ContextPtr CreateContext()
    {
        return Context::Create();
    }
    ContextPtr CreateContext(ConstPlatformPtr platform, const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber)
    {
        Error error;
        auto res = Context::Create();

        error = res->Init(platform, platformNumber, deviceNumber);
        if (error != CL_SUCCESS)
        {
            return nullptr;
        }

        return res;
    }
    Context::~Context()
    {
        clReleaseCommandQueue(queue_);
        clReleaseContext(context_);
    }
    ContextPtr Context::Create()
    {
        class MakeSharedEnabler : public Context
        {
        };

        auto res = std::make_shared<MakeSharedEnabler>();
        return res;
    }
    ContextPtr Context::GetPtr()
    {
        return shared_from_this();
    }
    ConstContextPtr Context::GetPtr() const
    {
        return const_cast<Context*>(this)->GetPtr();
    }
    Error Context::Init(ConstPlatformPtr platform, const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber)
    {
        Error error;
        Devices dev;

        if (initialized_)
        {
            return CL_SUCCESS;
        }

        if (!platform)
        {
            logger::Error(header, "Context (%d)(%d) not created: Platform pointer is null", platformNumber, deviceNumber);

            return CL_INVALID_PLATFORM;
        }

        platform_ = platform;
        if (platformNumber >= platform_->GetNumberPlatforms())
        {
            logger::Error(header, "Context (%d)(%d) not created: Platform number greater than existing ones",
                platformNumber, deviceNumber);

            return CL_INVALID_PLATFORM;
        }

        platformNumber_ = platformNumber;
        if (deviceNumber >= platform_->GetNumberDevices(platformNumber))
        {
            logger::Error(header, "Context (%d)(%d) not created: Device number greater than existing ones", platformNumber,
                deviceNumber);

            return CL_INVALID_DEVICE;
        }

        deviceNumber_ = deviceNumber;
        dev.resize(1);
        dev[0] = platform_->GetDevice(platformNumber_, deviceNumber_);
        contextProps_[1] = (cl_context_properties)platform->Get(platformNumber_);

        context_ = clCreateContext(contextProps_, 1, &dev[0], nullptr, nullptr, &error);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, "Context (%d)(%d) not created: " + messages.at(error), platformNumber, deviceNumber);

            return error;
        }

        contextInfo_ = GetContextInfo(context_);
        queue_ = clCreateCommandQueueWithProperties(context_, platform_->GetDevice(platformNumber, deviceNumber),
            queueProps_, &error);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, "Queue (%d)(%d) not created: " + messages.at(error), platformNumber, deviceNumber);
            clReleaseContext(context_);

            return error;
        }

        queueInfo_ = GetQueueInfo(queue_);
        initialized_ = true;

        return CL_SUCCESS;
    }
    PlatformNumber Context::GetPlatformNumber() const
    {
        return platformNumber_;
    }
    DeviceNumber Context::GetDeviceNumber() const
    {
        return deviceNumber_;
    }
    const cl_context& Context::Get() const
    {
        return context_;
    }
    const cl_command_queue& Context::GetQueue() const
    {
        return queue_;
    }
    const cl_device_id& Context::GetDevice() const
    {
        return platform_->GetDevice(platformNumber_, deviceNumber_);
    }
    const cl_platform_id& Context::GetPlatform() const
    {
        return platform_->Get(platformNumber_);
    }
    const ContextInfo& Context::GetInfo() const
    {
        return contextInfo_;
    }
    const QueueInfo& Context::GetQueueInfo() const
    {
        return queueInfo_;
    }
    ConstPlatformPtr Context::GetPlatformPtr() const
    {
        return platform_;
    }
    ContextInfo Context::GetContextInfo(cl_context context) const
    {
        ContextInfo res;

        res.referenceCount = GetContextInfo<cl_uint>(context, CL_CONTEXT_REFERENCE_COUNT);
        res.numberDevices = GetContextInfo<cl_uint>(context, CL_CONTEXT_NUM_DEVICES);
        res.devices = GetContextInfo<Devices>(context, CL_CONTEXT_DEVICES);
        res.properties = GetContextInfo<std::vector<cl_context_properties>>(context, CL_CONTEXT_PROPERTIES);

        return res;
    }
    QueueInfo Context::GetQueueInfo(cl_command_queue queue) const
    {
        QueueInfo res;

        res.context = GetQueueInfo<cl_context>(queue, CL_QUEUE_CONTEXT);
        res.device = GetQueueInfo<cl_device_id>(queue, CL_QUEUE_DEVICE);
        res.properties = GetQueueInfo<cl_command_queue_properties>(queue, CL_QUEUE_PROPERTIES);

        return res;
    }

    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Context::GetContextInfo(cl_context context, cl_context_info info) const
    {
        std::size_t size;
        T res;

        clGetContextInfo(context, info, 0, NULL, &size);
        clGetContextInfo(context, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Context::GetContextInfo(cl_context context, cl_context_info info) const
    {
        std::size_t size;
        T res;

        clGetContextInfo(context, info, 0, NULL, &size);
        res.resize(size);
        clGetContextInfo(context, info, size, &res[0], 0);

        return res;
    }

    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Context::GetQueueInfo(cl_command_queue queue, cl_command_queue_info info) const
    {
        std::size_t size;
        T res;

        clGetCommandQueueInfo(queue, info, 0, NULL, &size);
        clGetCommandQueueInfo(queue, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Context::GetQueueInfo(cl_command_queue queue, cl_command_queue_info info) const
    {
        std::size_t size;
        T res;

        clGetCommandQueueInfo(queue, info, 0, NULL, &size);
        res.resize(size);
        clGetCommandQueueInfo(queue, info, size, &res[0], 0);

        return res;
    }
} // namespace club