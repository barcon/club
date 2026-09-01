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

    using Number = std::size_t;
    using NumberDevices = std::size_t;
    using NumberPlatforms = std::size_t;
    using GlobalSize = std::vector<std::size_t>;
    using LocalSize = std::vector<std::size_t>;
    using NumberGroups = std::vector<cl_uint>;

    using PlatformIndex = Number;
    using DeviceIndex = Number;
    using DeviceIndices = std::vector<DeviceIndex>;

    using ArgNumber = cl_uint;
    using Error = cl_int;

    const String header = "CLUB";

    using Events = std::vector<cl_event>;
    using Platforms = std::vector<cl_platform_id>;
    using Devices = std::vector<cl_device_id>;
    using Queues = std::vector<cl_command_queue>;
    using Contexts = std::vector<cl_context>;
    using Programs = std::vector<cl_program>;

    struct PlatformInfo
    {
        std::vector<char> profile{};
        std::vector<char> version{};
        std::vector<char> name{};
        std::vector<char> vendor{};
        std::vector<char> extensions{};
    };
    struct DeviceInfo
    {
        cl_device_type type{ CL_DEVICE_TYPE_DEFAULT };
        cl_uint vendorID{ 0 };
        cl_uint maxComputeUnits{ 0 };
        cl_uint maxWorkItemDimensions{ 0 };
        std::vector<std::size_t> maxWorkItemSizes{};
        std::size_t maxWorkGroupSize{ 0 };
        cl_uint maxClockFrequency{ 0 };
        cl_uint addressBits{ 0 };
        cl_ulong maxMemAllocSize{ 0 };
        cl_uint maxSamplers{ 0 };
        std::size_t maxParameterSize{ 0 };
        cl_uint memBaseAddrAlign{ 0 };
        cl_uint minDataTypeAlignSize{ 0 };
        cl_device_fp_config singleFPConfig{ 0 };
        cl_device_mem_cache_type globalMemCacheType{ 0 };
        cl_uint globalMemCachelineSize{ 0 };
        cl_ulong globalMemCacheSize{ 0 };
        cl_ulong globalMemSize{ 0 };
        cl_ulong maxConstantBufferSize{ 0 };
        cl_uint maxConstantArgs{ 0 };
        std::size_t maxGlobalMemSize{ 0 };
        cl_device_local_mem_type localMemType{ 0 };
        cl_ulong localMemSize{ 0 };
        cl_bool errorCorrectionSupport{ false };
        cl_bool hostUnifiedMemory{ false };
        std::size_t profilingTimerResolution{ 0 };
        cl_bool endianLittle{ false };
        cl_bool available{ false };
        cl_bool compilerAvailable{ false };
        cl_device_exec_capabilities executionCapabilities{ 0 };
        cl_command_queue_properties queueProperties{ 0 };
        cl_platform_id platformID{ nullptr };
        std::vector<char> name{};
        std::vector<char> vendor{};
        std::vector<char> driverVersion{};
        std::vector<char> profile{};
        std::vector<char> version{};
        std::vector<char> extensions{};
    };
    struct ContextInfo
    {
        cl_uint referenceCount{0};
        cl_uint numberDevices{0};
        Devices devices{};
        std::vector<cl_context_properties> properties{};
    };
    struct ProgramInfo
    {
        cl_context context{ nullptr };
        NumberDevices numberDevices{0};
        Devices devices{};
        std::vector<char> source{};
        std::vector<std::size_t> binarySizes{};
        std::size_t numberKernels{0};
        std::vector<char> kernelNames{};

        cl_build_status buildStatus{ CL_BUILD_NONE };
        std::vector<char> buildOptions{};
        std::vector<char> buildLog{};
        cl_program_binary_type programBinaryType{ CL_PROGRAM_BINARY_TYPE_NONE };
    };
    struct BufferInfo
    {
        cl_mem_object_type type{ CL_MEM_OBJECT_BUFFER };
        cl_mem_flags flags{ 0 };
        std::size_t size{ 0 };
        void* hostPtr{ nullptr };
        cl_context context{ nullptr };
    };
    struct KernelInfo
    {
        std::vector<char> functionName{};
        cl_uint numberArgs{0};
        cl_uint referenceCount{0};
        cl_context context{ nullptr };
        cl_program program{ nullptr };
        std::vector<char> attributes{};
    };
    struct QueueInfo
    {
        cl_context context{ nullptr };
        cl_device_id device{ nullptr };
        cl_command_queue_properties properties{0};
    };
    struct EventInfo
    {
        cl_command_queue queue{ nullptr };
        cl_context context{ nullptr };
        cl_command_type type{ 0x00 };
        cl_int status{ CL_COMPLETE };
    };

    using DevicesInfo = std::vector<DeviceInfo>;
    using QueuesInfo = std::vector<QueueInfo>;

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
