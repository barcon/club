#ifndef CLUB_PLATFORM_HPP_
#define CLUB_PLATFORM_HPP_

#include "club_messages.hpp"
#include "club_types.hpp"

namespace club
{
    PlatformPtr CreatePlatform();

    class Platform : public std::enable_shared_from_this<Platform>
    {
    public:
        virtual ~Platform() = default;

        static PlatformPtr Create();
        PlatformPtr GetPtr();
        ConstPlatformPtr GetPtr() const;

        Error Init();

        NumberPlatforms GetNumberPlatforms() const;
        NumberDevices GetNumberDevices(PlatformIndex platformIndex) const;

        const cl_platform_id& Get(PlatformIndex platformIndex) const;
        const PlatformInfo& GetInfo(PlatformIndex platformIndex) const;

        const cl_device_id& GetDevice(PlatformIndex platformIndex, DeviceIndex deviceIndex) const;
        const DeviceInfo& GetDeviceInfo(PlatformIndex platformIndex, DeviceIndex deviceIndex) const;

    protected:
        Platform() = default;

        Error InitializePlatforms();
        Error InitializeDevices(PlatformIndex platformIndex);

        PlatformInfo GetInfoPlatform(PlatformIndex platformIndex) const;
        DeviceInfo GetInfoDevice(PlatformIndex platformIndex, DeviceIndex deviceNumber) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetPlatformInfo(cl_platform_id platform, cl_platform_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetPlatformInfo(cl_platform_id platform, cl_platform_info info) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetDeviceInfo(cl_device_id device, cl_device_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetDeviceInfo(cl_device_id device, cl_device_info info) const;

        std::vector<cl_platform_id> platforms_;
        std::vector<PlatformInfo> platformsInfo_;

        std::vector<std::vector<cl_device_id>> devices_;
        std::vector<std::vector<DeviceInfo>> devicesInfo_;
    };

    void PrintInfoPlatform(const PlatformInfo& platformInfo, PlatformIndex platformIndex);
    void PrintInfoDevice(const DeviceInfo& deviceInfo, PlatformIndex platformIndex);

} // namespace club

#endif