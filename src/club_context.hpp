#ifndef CLUB_CONTEXT_HPP_
#define CLUB_CONTEXT_HPP_

#include "club_platform.hpp"

namespace club
{
    ContextPtr CreateContext();
    ContextPtr CreateContext(ConstPlatformPtr platform, const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber);

    class Context : public std::enable_shared_from_this<Context>
    {
    public:
        virtual ~Context();

        static ContextPtr Create();
        ContextPtr GetPtr();
        ConstContextPtr GetPtr() const;

        Error Init(ConstPlatformPtr platform, const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber);

        PlatformNumber GetPlatformNumber() const;
        DeviceNumber GetDeviceNumber() const;

        const cl_context& Get() const;
        const cl_command_queue& GetQueue() const;
        const cl_device_id& GetDevice() const;
        const cl_platform_id& GetPlatform() const;

        const ContextInfo& GetInfo() const;
        const QueueInfo& GetQueueInfo() const;

        ConstPlatformPtr GetPlatformPtr() const;

    protected:
        Context() = default;

        ContextInfo GetContextInfo(cl_context context) const;
        QueueInfo GetQueueInfo(cl_command_queue queue) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetContextInfo(cl_context context, cl_context_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetContextInfo(cl_context context,  cl_context_info info) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetQueueInfo(cl_command_queue queue, cl_command_queue_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetQueueInfo(cl_command_queue queue, cl_command_queue_info info) const;

        bool initialized_{ false };

        ConstPlatformPtr platform_{ nullptr };

        PlatformNumber platformNumber_{ 0 };
        DeviceNumber deviceNumber_{ 0 };

        cl_context context_;
        cl_command_queue queue_;

        ContextInfo contextInfo_;
        QueueInfo queueInfo_;

        cl_context_properties contextProps_[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
        cl_queue_properties queueProps_[1] = { 0 };
    };
} // namespace club

#endif