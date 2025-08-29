#include "club_platform.hpp"

namespace club
{
    PlatformPtr CreatePlatform(bool initialize)
    {
        Error error;
        auto res = Platform::Create();

        if (initialize)
        {
            error = res->Init();
            if (error != CL_SUCCESS)
            {
                return nullptr;
            }
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

        if (initialized_)
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

        for (PlatformNumber i = 0; i < platforms_.size(); ++i)
        {
            error = InitializeDevices(i);
            if (error != CL_SUCCESS)
            {
                return error;
            }
        }

        initialized_ = true;

        return CL_SUCCESS;
    }
    NumberPlatforms Platform::GetNumberPlatforms() const
    {
        return static_cast<NumberPlatforms>(platforms_.size());
    }
    NumberDevices Platform::GetNumberDevices(const PlatformNumber& platformNumber) const
    {
        if (platformNumber > devices_.size())
        {
            return 0;
        }

        return static_cast<NumberDevices>(devices_[platformNumber].size());
    }
    const cl_platform_id& Platform::Get(const PlatformNumber& platformNumber) const
    {
        return platforms_[platformNumber];
    }
    const PlatformInfo& Platform::GetInfo(const PlatformNumber& platformNumber) const
    {
        return platformsInfo_[platformNumber];
    }
    const cl_device_id& Platform::GetDevice(const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber) const
    {
        return devices_[platformNumber][deviceNumber];
    }
    const DeviceInfo& Platform::GetDeviceInfo(const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber) const
    {
        return devicesInfo_[platformNumber][deviceNumber];
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
            logger::Error(header, "Platforms not initialized " + messages.at(error));
            return error;
        }

        logger::Info(header, "Number of platforms: %d", size);

        for (PlatformNumber i = 0; i < size; ++i)
        {
            platformsInfo_[i] = GetInfoPlatform(i);
        }

        return CL_SUCCESS;
    }
    Error Platform::InitializeDevices(const PlatformNumber& platformNumber)
    {
        Error error;
        Size size;

        error = clGetDeviceIDs(platforms_[platformNumber], CL_DEVICE_TYPE_ALL, 0, NULL, &size);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, "Devices not found in platform: %d " + messages.at(error), platformNumber);

            return error;
        }


        devices_[platformNumber].resize(size);
        devicesInfo_[platformNumber].resize(size);

        error = clGetDeviceIDs(platforms_[platformNumber], CL_DEVICE_TYPE_ALL, size, &devices_[platformNumber][0], NULL);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, "Devices not initialized in platform: %d" + messages.at(error), platformNumber);

            return error;
        }

        logger::Info(header, "Number of devices in platform %d: %d", platformNumber, size);
        for (DeviceNumber i = 0; i < devices_[platformNumber].size(); ++i)
        {
            devicesInfo_[platformNumber][i] = GetInfoDevice(platformNumber, i);
        }

        return CL_SUCCESS;
    }
    PlatformInfo Platform::GetInfoPlatform(const PlatformNumber& platformNumber) const
    {
        PlatformInfo res;
        cl_platform_id platform = platforms_[platformNumber];
        String message;

        res.profile = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_PROFILE);
        res.version = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_VERSION);
        res.name = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_NAME);
        res.vendor = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_VENDOR);
        res.extensions = GetPlatformInfo<std::vector<char>>(platform, CL_PLATFORM_EXTENSIONS);

        logger::Info(header, "Platform: %d", platformNumber);
        logger::Info(header, "\tPlatform vendor: " + String(res.vendor.begin(), res.vendor.end()));
        logger::Info(header, "\tPlatform name: " + String(res.name.begin(), res.name.end()));
        logger::Info(header, "\tPlatform version: " + String(res.version.begin(), res.version.end()));

        return res;
    }
    DeviceInfo Platform::GetInfoDevice(const PlatformNumber& platformNumber, const DeviceNumber& deviceNumber) const
    {
        DeviceInfo res;
        cl_device_id device = devices_[platformNumber][deviceNumber];
        String message;

        res.type = GetDeviceInfo<cl_device_type>(device, CL_DEVICE_TYPE);
        res.vendorID = GetDeviceInfo<cl_uint>(device, CL_DEVICE_VENDOR_ID);
        res.maxComputeUnits = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MAX_COMPUTE_UNITS);
        res.maxWorkItemDimensions = GetDeviceInfo<cl_uint>(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
        res.maxWorItemSizes = GetDeviceInfo<std::vector<std::size_t>>(device, CL_DEVICE_MAX_WORK_ITEM_SIZES);
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

        logger::Info(header, "\tDevice (%d) vendor: " + String(res.vendor.begin(), res.vendor.end()), deviceNumber);
        logger::Info(header, "\tDevice (%d) name: " + String(res.name.begin(), res.name.end()), deviceNumber);

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
} // namespace club