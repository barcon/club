#include "club_context.hpp"

namespace club
{
    ContextPtr CreateContext()
    {
        return Context::Create();
    }
    ContextPtr CreateContext(PlatformPtr platform, PlatformIndex platformIndex, const DeviceIndices& deviceIndices)
    {
        Error error;
        auto res = Context::Create();

        error = res->Init(platform, platformIndex, deviceIndices);
        if (error != CL_SUCCESS)
        {
            return nullptr;
        }

        return res;
    }
    Context::~Context()
    {
        for(auto &queue : queues_)
        {
            clFinish(queue);
            clReleaseCommandQueue(queue);
        }

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
    Error Context::Init(PlatformPtr platform, PlatformIndex platformIndex, const DeviceIndices& deviceIndices)
    {
        Error error;

        if (context_)
        {
            return CL_SUCCESS;
        }

        if (!platform)
        {
            logger::Error(header, utils::string::Format("Context {0} not created: Platform pointer is null", platformIndex));

            return CL_INVALID_PLATFORM;
        }

        if (platformIndex >= platform->GetNumberPlatforms())
        {
            logger::Error(header, utils::string::Format("Context {0} not created: Platform index greater than existing ones", platformIndex));

            return CL_INVALID_PLATFORM;
        }

        devices_.clear();
        devicesInfo_.clear();
        
        for (auto& deviceIndex : deviceIndices)
        {
            if (deviceIndex >= platform->GetNumberDevices(platformIndex))
            {
                logger::Error(header, utils::string::Format("Context {0}{1} not created: Device index greater than existing ones", platformIndex, deviceIndex));
             
                continue;
			}

			auto device = platform->GetDevice(platformIndex, deviceIndex);
            auto deviceInfo = platform->GetDeviceInfo(platformIndex, deviceIndex);

            devices_.push_back(device);
            devicesInfo_.push_back(deviceInfo);
        }

        contextProps_[1] = (cl_context_properties)platform->Get(platformIndex);
        context_ = clCreateContext(contextProps_, static_cast<cl_uint>(devices_.size()), &devices_[0], nullptr, nullptr, &error);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, utils::string::Format("Context {0} not created: {1}", platformIndex, messages.at(error)));
            return error;
        }
        contextInfo_ = GetContextInfo(context_);
      
        queues_.clear();
        queuesInfo_.clear();

        for (auto& deviceIndex : deviceIndices)
        {
            auto device = platform->GetDevice(platformIndex, deviceIndex);
            auto queue = clCreateCommandQueueWithProperties(context_, device, queueProps_, &error);

            if (error != CL_SUCCESS)
            {
                logger::Error(header, utils::string::Format("Queue not created for device {0}: {1}", deviceIndex, messages.at(error)));

                return error;
            }

            auto queueInfo = GetQueueInfo(queue);

            queues_.push_back(queue);
            queuesInfo_.push_back(queueInfo);
        }

        return CL_SUCCESS;
    }
    const cl_context& Context::Get() const
    {
        return context_;
    }
    const Queues& Context::GetQueues() const
    {
		return queues_;
    }
    const Devices& Context::GetDevices() const
    {
        return devices_;
    }
    LocalSize Context::GetLocalSize(DeviceIndex deviceIndex, Dimension dim) const
    {
        auto workGroupSize = devicesInfo_[deviceIndex].maxWorkGroupSize;

		LocalSize localSize(dim, 1);
        std::size_t aux{ 1 };

        if (devicesInfo_[deviceIndex].type == CL_DEVICE_TYPE_CPU)
        {
            aux = 1;
        }
        else if (devicesInfo_[deviceIndex].type == CL_DEVICE_TYPE_GPU)
        {
            aux = std::min(32u, utils::math::Power2Floor(static_cast<unsigned int>(std::pow(workGroupSize, 1. / dim))));
        }
        else
        {
            aux = 1;
        }

        for (auto& it : localSize)
        {
            it = aux;
        }

        return localSize;
    }
    
    const ContextInfo& Context::GetInfo() const
    {
        return contextInfo_;
    }
    const DeviceInfo& Context::GetDeviceInfo(DeviceIndex deviceIndex) const
    {
        return devicesInfo_[deviceIndex];
    }
    const QueueInfo& Context::GetQueueInfo(DeviceIndex deviceIndex) const
    {
        return queuesInfo_[deviceIndex];
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