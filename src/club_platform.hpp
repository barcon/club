#ifndef CLUB_PLATFORM_HPP_
#define CLUB_PLATFORM_HPP_

#include "club_messages.hpp"
#include "club_types.hpp"

namespace club
{
    PlatformPtr CreatePlatform(bool initialize = true);

    class Platform : public std::enable_shared_from_this<Platform>
    {
    public:
        virtual ~Platform() = default;

        static PlatformPtr Create();
        PlatformPtr GetPtr();
        ConstPlatformPtr GetPtr() const;

        Error Init();

        NumberPlatforms GetNumberPlatforms() const;
        NumberDevices GetNumberDevices(const PlatformNumber& platformNumber) const;

        const cl_platform_id& Get(const PlatformNumber& platformNumber) const;
        const PlatformInfo& GetInfo(const PlatformNumber& platformNumber) const;

        const cl_device_id& GetDevice(const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber) const;
        const DeviceInfo& GetDeviceInfo(const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber) const;

    protected:
        Platform() = default;

        Error InitializePlatforms();
        Error InitializeDevices(const PlatformNumber& platformNumber);

        PlatformInfo GetInfoPlatform(const PlatformNumber& platformNumber) const;
        DeviceInfo GetInfoDevice(const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetPlatformInfo(cl_platform_id platform, cl_platform_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetPlatformInfo(cl_platform_id platform, cl_platform_info info) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetDeviceInfo(cl_device_id device, cl_device_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetDeviceInfo(cl_device_id device, cl_device_info info) const;

        bool initialized_{ false };

        std::vector<cl_platform_id> platforms_;
        std::vector<PlatformInfo> platformsInfo_;

        std::vector<std::vector<cl_device_id>> devices_;
        std::vector<std::vector<DeviceInfo>> devicesInfo_;
    };
} // namespace club

#endif