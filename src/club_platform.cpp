#include "club_platform.hpp"

namespace club
{
    PlatformPtr CreatePlatform()
    {
        Error error;

        auto res = Platform::Create();

        error = res->Init();
        if (error != CL_SUCCESS)
        {
            return nullptr;
        }

        return res;
    }
    PlatformPtr Platform::Create()
    {
        class MakeSharedEnabler : public Platform
        {
        };

        auto res = std::make_shared<MakeSharedEnabler>();
        return res;
    }
    PlatformPtr Platform::GetPtr()
    {
        return this->shared_from_this();
    }
    ConstPlatformPtr Platform::GetPtr() const
    {
        return const_cast<Platform*>(this)->GetPtr();
    }
    Error Platform::Init()
    {
        Error error;

        if (platforms_.size() > 0)
        {
            return CL_SUCCESS;
        }

        error = InitializePlatforms();
        if (error != CL_SUCCESS)
        {
            return error;
        }

        devices_.resize(platforms_.size());
        devicesInfo_.resize(platforms_.size());

        for (PlatformIndex i = 0; i < platforms_.size(); ++i)
        {
            error = InitializeDevices(i);
            if (error != CL_SUCCESS)
            {
                return error;
            }
        }

        for (PlatformIndex i = 0; i < platforms_.size(); ++i)
        {
			PrintInfoPlatform(platformsInfo_[i], i);
            
            for (DeviceIndex j = 0; j < devices_[i].size(); ++j)
            {
                PrintInfoDevice(devicesInfo_[i][j], j);
			}
        }

        return CL_SUCCESS;
    }
    NumberPlatforms Platform::GetNumberPlatforms() const
    {
        return static_cast<NumberPlatforms>(platforms_.size());
    }
    NumberDevices Platform::GetNumberDevices(PlatformIndex platformIndex) const
    {
        if (platformIndex >= platforms_.size())
        {
            return 0;
        }

        return static_cast<NumberDevices>(devices_[platformIndex].size());
    }
    const cl_platform_id& Platform::Get(PlatformIndex platformIndex) const
    {
        return platforms_[platformIndex];
    }
    const PlatformInfo& Platform::GetInfo(PlatformIndex platformIndex) const
    {
        return platformsInfo_[platformIndex];
    }
    const cl_device_id& Platform::GetDevice(PlatformIndex platformIndex, DeviceIndex deviceIndex) const
    {
        return devices_[platformIndex][deviceIndex];
    }
    const DeviceInfo& Platform::GetDeviceInfo(PlatformIndex platformIndex, DeviceIndex deviceIndex) const
    {
        return devicesInfo_[platformIndex][deviceIndex];
    }
    Error Platform::InitializePlatforms()
    {
        Error error;
        Size size;

        error = clGetPlatformIDs(0, NULL, &size);

        if (error != CL_SUCCESS)
        {
            logger::Error(header, "Platforms not found");
            return error;
        }

        platforms_.resize(size);
        platformsInfo_.resize(size);

        error = clGetPlatformIDs(size, &platforms_[0], NULL);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, utils::string::Format("Platforms not initialized {}", messages.at(error)));
            return error;
        }

        logger::Info(header, utils::string::Format("Number of platforms: {:d}", size));

        for (PlatformIndex i = 0; i < size; ++i)
        {
            platformsInfo_[i] = GetInfoPlatform(i);
        }

        return CL_SUCCESS;
    }
    Error Platform::InitializeDevices(PlatformIndex platformIndex)
    {
        Error error;
        Size size;

        error = clGetDeviceIDs(platforms_[platformIndex], CL_DEVICE_TYPE_ALL, 0, NULL, &size);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, utils::string::Format("Devices not found in platform: {:d} {} ", platformIndex, messages.at(error)));

            return error;
        }


        devices_[platformIndex].resize(size);
        devicesInfo_[platformIndex].resize(size);

        error = clGetDeviceIDs(platforms_[platformIndex], CL_DEVICE_TYPE_ALL, size, &devices_[platformIndex][0], NULL);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, utils::string::Format("Devices not initialized in platform: {:d} {}", platformIndex, messages.at(error)));

            return error;
        }

        logger::Info(header, utils::string::Format("Number of devices in platform {:d}: {:d}", platformIndex, size));
        for (DeviceIndex i = 0; i < devices_[platformIndex].size(); ++i)
        {
            devicesInfo_[platformIndex][i] = GetInfoDevice(platformIndex, i);
        }

        return CL_SUCCESS;
    }
    
    PlatformInfo Platform::GetInfoPlatform(PlatformIndex platformIndex) const
    {
        PlatformInfo res;
        cl_platform_id platform = platforms_[platformIndex];
        String message;

        res.profile = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_PROFILE);
        res.version = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_VERSION);
        res.name = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_NAME);
        res.vendor = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_VENDOR);
        res.extensions = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_EXTENSIONS);

        return res;
    }
    DeviceInfo Platform::GetInfoDevice(PlatformIndex platformIndex, DeviceIndex deviceIndex) const
    {
        DeviceInfo res;
        cl_device_id device = devices_[platformIndex][deviceIndex];
        String message;

        res.type = GetDeviceInfo<cl_device_type>(device, CL_DEVICE_TYPE);
        res.vendorID = GetDeviceInfo<cl_uint>(device, CL_DEVICE_VENDOR_ID);
        res.maxComputeUnits = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MAX_COMPUTE_UNITS);
        res.maxWorkItemDimensions = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
        res.maxWorkItemSizes = GetDeviceInfo<std::vector<std::size_t>>(device, CL_DEVICE_MAX_WORK_ITEM_SIZES);
        res.maxWorkGroupSize = GetDeviceInfo<std::size_t>(device, CL_DEVICE_MAX_WORK_GROUP_SIZE);
        res.maxClockFrequency = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MAX_CLOCK_FREQUENCY);
        res.addressBits = GetDeviceInfo<cl_uint>(device, CL_DEVICE_ADDRESS_BITS);
        res.maxMemAllocSize = GetDeviceInfo<cl_ulong>(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE);
        res.maxSamplers = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MAX_SAMPLERS);
        res.maxParameterSize = GetDeviceInfo<std::size_t>(device, CL_DEVICE_MAX_PARAMETER_SIZE);
        res.memBaseAddrAlign = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MEM_BASE_ADDR_ALIGN);
        res.minDataTypeAlignSize = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
        res.singleFPConfig = GetDeviceInfo<cl_device_fp_config>(device, CL_DEVICE_SINGLE_FP_CONFIG);
        res.globalMemCacheType = GetDeviceInfo<cl_device_mem_cache_type>(device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
        res.globalMemCachelineSize = GetDeviceInfo<cl_uint>(device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
        res.globalMemCacheSize = GetDeviceInfo<cl_ulong>(device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
        res.globalMemSize = GetDeviceInfo<cl_ulong>(device, CL_DEVICE_GLOBAL_MEM_SIZE);
        res.maxConstantBufferSize = GetDeviceInfo<cl_ulong>(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
        res.maxConstantArgs = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MAX_CONSTANT_ARGS);
        res.maxGlobalMemSize = GetDeviceInfo<std::size_t>(device, CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE);
        res.localMemType = GetDeviceInfo<cl_device_local_mem_type>(device, CL_DEVICE_LOCAL_MEM_TYPE);
        res.localMemSize = GetDeviceInfo<cl_ulong>(device, CL_DEVICE_LOCAL_MEM_SIZE);
        res.errorCorrectionSupport = GetDeviceInfo<cl_bool>(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT);
        res.hostUnifiedMemory = GetDeviceInfo<cl_bool>(device, CL_DEVICE_HOST_UNIFIED_MEMORY);
        res.profilingTimerResolution = GetDeviceInfo<std::size_t>(device, CL_DEVICE_PROFILING_TIMER_RESOLUTION);
        res.endianLittle = GetDeviceInfo<cl_bool>(device, CL_DEVICE_ENDIAN_LITTLE);
        res.available = GetDeviceInfo<cl_bool>(device, CL_DEVICE_AVAILABLE);
        res.compilerAvailable = GetDeviceInfo<cl_bool>(device, CL_DEVICE_COMPILER_AVAILABLE);
        res.executionCapabilities = GetDeviceInfo<cl_device_exec_capabilities>(device, CL_DEVICE_EXECUTION_CAPABILITIES);
        res.queueProperties = GetDeviceInfo<cl_command_queue_properties>(device, CL_DEVICE_QUEUE_PROPERTIES);
        res.platformID = GetDeviceInfo<cl_platform_id>(device, CL_DEVICE_PLATFORM);
        res.name = GetDeviceInfo<std::vector<char>>(device, CL_DEVICE_NAME);
        res.vendor = GetDeviceInfo<std::vector<char>>(device, CL_DEVICE_VENDOR);
        res.driverVersion = GetDeviceInfo<std::vector<char>>(device, CL_DRIVER_VERSION);
        res.profile = GetDeviceInfo<std::vector<char>>(device, CL_DEVICE_PROFILE);
        res.version = GetDeviceInfo<std::vector<char>>(device, CL_DEVICE_VERSION);
        res.extensions = GetDeviceInfo<std::vector<char>>(device, CL_DEVICE_EXTENSIONS);

        return res;
    }
    
    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Platform::GetPlatformInfo(cl_platform_id platform, cl_platform_info info) const
    {
        std::size_t size;
        T res;

        clGetPlatformInfo(platform, info, 0, NULL, &size);
        clGetPlatformInfo(platform, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Platform::GetPlatformInfo(cl_platform_id platform, cl_platform_info info) const
    {
        std::size_t size;
        T res;

        clGetPlatformInfo(platform, info, 0, NULL, &size);
        res.resize(size);
        clGetPlatformInfo(platform, info, size, &res[0], 0);

        return res;
    }
    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Platform::GetDeviceInfo(cl_device_id device, cl_device_info info) const
    {
        std::size_t size;
        T res;

        clGetDeviceInfo(device, info, 0, NULL, &size);
        clGetDeviceInfo(device, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Platform::GetDeviceInfo(cl_device_id device, cl_device_info info) const
    {
        std::size_t size;
        T res;

        clGetDeviceInfo(device, info, 0, NULL, &size);
        res.resize(size);
        clGetDeviceInfo(device, info, size, &res[0], 0);

        return res;
    }

    void PrintInfoPlatform(const PlatformInfo& platformInfo, PlatformIndex platformIndex)
    {
        logger::Info(header, utils::string::Format("Platform: {:d}", platformIndex));
        logger::Info(header, utils::string::Format("\tVendor: {}", String(platformInfo.vendor.begin(), platformInfo.vendor.end())));
        logger::Info(header, utils::string::Format("\tName: {}", String(platformInfo.name.begin(), platformInfo.name.end())));
        logger::Info(header, utils::string::Format("\tVersion: {}", String(platformInfo.version.begin(), platformInfo.version.end())));
    }
    void PrintInfoDevice(const DeviceInfo& deviceInfo, DeviceIndex deviceIndex)
    {
        logger::Info(header, utils::string::Format("\tDevice ({:d}) vendor: {}", deviceIndex, String(deviceInfo.vendor.begin(), deviceInfo.vendor.end())));
        logger::Info(header, utils::string::Format("\tDevice ({:d}) name: {}", deviceIndex, String(deviceInfo.name.begin(), deviceInfo.name.end())));
    }
} // namespace club