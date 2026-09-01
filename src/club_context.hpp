#ifndef CLUB_CONTEXT_HPP_
#define CLUB_CONTEXT_HPP_

#include "club_platform.hpp"

namespace club
{
    ContextPtr CreateContext();
    ContextPtr CreateContext(PlatformPtr platform, PlatformIndex platformIndex, const DeviceIndices& deviceIndices);

    class Context : public std::enable_shared_from_this<Context>
    {
    public:
        virtual ~Context();

        static ContextPtr Create();
        ContextPtr GetPtr();
        ConstContextPtr GetPtr() const;

        Error Init(PlatformPtr platform, PlatformIndex platformIndex, const DeviceIndices& deviceIndices);

        const cl_context& Get() const;
        const Queues& GetQueues() const;
        const Devices& GetDevices() const;
		LocalSize GetLocalSize(DeviceIndex deviceIndex, Dimension dim) const;

        const ContextInfo& GetInfo() const;
        const DeviceInfo& GetDeviceInfo(DeviceIndex deviceIndex) const;
        const QueueInfo& GetQueueInfo(DeviceIndex deviceIndex) const;

    protected:
        Context() = default;

        ContextInfo GetContextInfo(cl_context context) const;
        QueueInfo GetQueueInfo(cl_command_queue queue) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetContextInfo(cl_context context, cl_context_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetContextInfo(cl_context context,  cl_context_info info) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetQueueInfo(cl_command_queue queue, cl_command_queue_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetQueueInfo(cl_command_queue queue, cl_command_queue_info info) const;

        Devices devices_{};
        DevicesInfo devicesInfo_{};
        
        Queues queues_{};
        QueuesInfo queuesInfo_{};

        cl_context context_{ nullptr };
        ContextInfo contextInfo_;

        cl_context_properties contextProps_[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
        cl_queue_properties queueProps_[1] = { 0 };
    };
} // namespace club

#endif