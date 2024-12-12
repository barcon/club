#ifndef CLUB_TYPES_HPP_
#define CLUB_TYPES_HPP_

#include "logger.hpp"
#include "utils.hpp"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <memory>
#include <type_traits>
#include <vector>

namespace club
{
    using Scalar = double;
    using Size = cl_uint;
    using File = utils::file::Text;
    using String = utils::String;
    using Dimension = cl_uint;
    using Index = std::size_t;

    using NumberDevices = std::size_t;
    using NumberPlatforms = std::size_t;
    using GlobalSize = std::vector<std::size_t>;
    using LocalSize = std::vector<std::size_t>;
    using NumberGroups = std::vector<cl_uint>;

    using PlatformNumber = NumberPlatforms;
    using DeviceNumber = NumberDevices;

    using Events = std::vector<cl_event>;
    using Platforms = std::vector<cl_platform_id>;
    using Devices = std::vector<cl_device_id>;
    using Contexts = std::vector<cl_context>;
    using Programs = std::vector<cl_program>;

    using ArgNumber = cl_uint;
    using Error = cl_int;

    const String header = "CLUB";

    struct PlatformInfo
    {
        std::vector<char> profile;
        std::vector<char> version;
        std::vector<char> name;
        std::vector<char> vendor;
        std::vector<char> extensions;
    };
    struct DeviceInfo
    {
        cl_device_type type;
        cl_uint vendorID;
        cl_uint maxComputeUnits;
        cl_uint maxWorkItemDimensions;
        std::vector<std::size_t> maxWorItemSizes;
        std::size_t maxWorkGroupSize;
        cl_uint maxClockFrequency;
        cl_uint addressBits;
        cl_ulong maxMemAllocSize;
        cl_uint maxSamplers;
        std::size_t maxParameterSize;
        cl_uint memBaseAddrAlign;
        cl_uint minDataTypeAlignSize;
        cl_device_fp_config singleFPConfig;
        cl_device_mem_cache_type globalMemCacheType;
        cl_uint globalMemCachelineSize;
        cl_ulong globalMemCacheSize;
        cl_ulong globalMemSize;
        cl_ulong maxConstantBufferSize;
        cl_uint maxConstantArgs;
        std::size_t maxGlobalMemSize;
        cl_device_local_mem_type localMemType;
        cl_ulong localMemSize;
        cl_bool errorCorrectionSupport;
        cl_bool hostUnifiedMemory;
        std::size_t profilingTimerResolution;
        cl_bool endianLittle;
        cl_bool available;
        cl_bool compilerAvailable;
        cl_device_exec_capabilities executionCapabilities;
        cl_command_queue_properties queueProperties;
        cl_platform_id platformID;
        std::vector<char> name;
        std::vector<char> vendor;
        std::vector<char> driverVersion;
        std::vector<char> profile;
        std::vector<char> version;
        std::vector<char> extensions;
    };
    struct ContextInfo
    {
        cl_uint referenceCount;
        cl_uint numberDevices;
        Devices devices;
        std::vector<cl_context_properties> properties;
    };
    struct ProgramInfo
    {
        cl_context context;
        NumberDevices numberDevices;
        Devices devices;
        std::vector<char> source;
        std::vector<std::size_t> binarySizes;
        std::size_t numberKernels;
        std::vector<char> kernelNames;

        cl_build_status buildStatus;
        std::vector<char> buildOptions;
        std::vector<char> buildLog;
        cl_program_binary_type programBinaryType;
    };
    struct BufferInfo
    {
        cl_mem_info type;
        cl_mem_info flags;
        std::size_t size;
        void* hostPtr;
        cl_context context;
    };
    struct KernelInfo
    {
        std::vector<char> functionName;
        cl_uint numberArgs;
        cl_uint referenceCount;
        cl_context context;
        cl_program program;
        std::vector<char> attributes;
    };
    struct QueueInfo
    {
        cl_context context;
        cl_device_id device;
        cl_command_queue_properties properties;
    };
    struct EventInfo
    {
        cl_command_queue queue;
        cl_context context;
        cl_command_type type;
        cl_int status;
    };

    class Platform;
    using PlatformPtr = std::shared_ptr<Platform>;
    using ConstPlatformPtr = std::shared_ptr<const Platform>;

    class Context;
    using ContextPtr = std::shared_ptr<Context>;
    using ConstContextPtr = std::shared_ptr<const Context>;

    class Program;
    using ProgramPtr = std::shared_ptr<Program>;
    using ConstProgramPtr = std::shared_ptr<const Program>;

    class Buffer;
    using BufferPtr = std::shared_ptr<Buffer>;
    using ConstBufferPtr = std::shared_ptr<const Buffer>;

    class Kernel;
    using KernelPtr = std::shared_ptr<Kernel>;
    using ConstKernelPtr = std::shared_ptr<const Kernel>;

    class Queue;
    using QueuePtr = std::shared_ptr<Queue>;
    using ConstQueuePtr = std::shared_ptr<const Queue>;

    class Event;
    using EventPtr = std::shared_ptr<Event>;
    using ConstEventPtr = std::shared_ptr<const Event>;

    template <typename T, typename _ = void> struct is_vector
    {
        static const bool value = false;
    };

    template <typename T>
    struct is_vector<T, typename std::enable_if<
        std::is_same<T, std::vector<typename T::value_type, typename T::allocator_type>>::value>::type>
    {
        static const bool value = true;
    };
} // namespace club

#endif