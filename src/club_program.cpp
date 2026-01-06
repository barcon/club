#include "club_program.hpp"
#include <iostream>

namespace club
{
    ProgramPtr CreateProgram()
    {
        return Program::Create();
    }
    ProgramPtr CreateProgramFromString(ConstContextPtr context, const String& source)
    {
        Error error;
        auto res = Program::Create();

        error = res->Init(context, source);
        if (error != CL_SUCCESS)
        {
            return nullptr;
        }

        return res;
    }
    ProgramPtr CreateProgramFromFile(ConstContextPtr context, const String& fileName)
    {
        File file;
        Error error;

        auto res = Program::Create();
        error = static_cast<Error>(file.Open(fileName));

        if (error)
        {
            logger::Error(header, utils::string::Format("Could not read file {}", fileName));
            return nullptr;
        }

        error = res->Init(context, file.GetFull());
        if (error != CL_SUCCESS)
        {
            return nullptr;
        }

        return res;
    }
    ProgramPtr CreateProgramFromFile(ConstContextPtr context, const char* fileName)
    {
        return CreateProgramFromFile(context, static_cast<String>(fileName));
    }
    Program::~Program()
    {
        clReleaseProgram(program_);
    }
    ProgramPtr Program::Create()
    {
        class MakeSharedEnabler : public Program
        {
        };

        auto res = std::make_shared<MakeSharedEnabler>();
        return res;
    }
    ProgramPtr Program::GetPtr()
    {
        return shared_from_this();
    }
    ConstProgramPtr Program::GetPtr() const
    {
        return const_cast<Program*>(this)->GetPtr();
    }
    Error Program::Init(ConstContextPtr context, const String& source)
    {
        Error error;
        Devices dev;

        if (initialized_)
        {
            return CL_SUCCESS;
        }

        if (!context)
        {
            logger::Error(header, "Invalid context to build program");

            return CL_INVALID_CONTEXT;
        }

        platform_ = context->GetPlatformPtr();
        context_ = context;
        source_ = source;
        auto src = source_.c_str();

        program_ = clCreateProgramWithSource(context_->Get(), 1, &src, NULL, &error);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, utils::string::Format("Program could not be created with source: {}", messages.at(error)));

            return error;
        }
        
        dev.resize(1);
        dev[0] = context_->GetDevice();

        error = clBuildProgram(program_, 1, &dev[0], "-cl-std=CL2.0", NULL, NULL);
        programInfo_ = GetProgramInfo(program_, context_->GetDevice());

        if (error != CL_SUCCESS)
        {
            logger::Error(header, utils::string::Format("Program could not be built: {}", messages.at(error)));
            logger::Error(header, String(programInfo_.buildLog.begin(), programInfo_.buildLog.end()));

            return error;
        }
 
        initialized_ = true;

        return CL_SUCCESS;
    }
    const cl_program& Program::Get() const
    {
        return program_;
    }
    const cl_context& Program::GetContext() const
    {
        return context_->Get();
    }
    const ProgramInfo& Program::GetInfo() const
    {
        return programInfo_;
    }
    const String& Program::GetSource() const
    {
        return source_;
    }
    ProgramInfo Program::GetProgramInfo(cl_program program, cl_device_id device) const
    {
        ProgramInfo res;

        res.context = GetProgramInfo<cl_context>(program, CL_PROGRAM_CONTEXT);
        res.numberDevices = GetProgramInfo<NumberDevices>(program, CL_PROGRAM_NUM_DEVICES);
        res.devices = GetProgramInfo<Devices>(program, CL_PROGRAM_DEVICES);
        res.source = GetProgramInfo<std::vector<char>>(program, CL_PROGRAM_SOURCE);
        res.binarySizes = GetProgramInfo<std::vector<std::size_t>>(program, CL_PROGRAM_BINARY_SIZES);
        res.numberKernels = GetProgramInfo<std::size_t>(program, CL_PROGRAM_NUM_KERNELS);
        res.kernelNames = GetProgramInfo<std::vector<char>>(program, CL_PROGRAM_KERNEL_NAMES);

        res.buildStatus = GetProgramBuildInfo<cl_build_status>(program, device, CL_PROGRAM_BUILD_STATUS);
        res.buildOptions = GetProgramBuildInfo<std::vector<char>>(program, device, CL_PROGRAM_BUILD_OPTIONS);
        res.buildLog = GetProgramBuildInfo<std::vector<char>>(program, device, CL_PROGRAM_BUILD_LOG);
        res.programBinaryType = GetProgramBuildInfo<cl_program_binary_type>(program, device, CL_PROGRAM_BINARY_TYPE);

        return res;
    }
    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Program::GetProgramInfo(cl_program program, cl_program_info info) const
    {
        std::size_t size;
        T res;

        clGetProgramInfo(program, info, 0, NULL, &size);
        clGetProgramInfo(program, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Program::GetProgramInfo(cl_program program, cl_program_info info) const
    {
        std::size_t size;
        T res;

        clGetProgramInfo(program, info, 0, NULL, &size);
        res.resize(size);
        clGetProgramInfo(program, info, size, &res[0], 0);

        return res;
    }
    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Program::GetProgramBuildInfo(cl_program program, cl_device_id device, cl_program_build_info info) const
    {
        std::size_t size;
        T res;

        clGetProgramBuildInfo(program, device, info, 0, NULL, &size);
        clGetProgramBuildInfo(program, device, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Program::GetProgramBuildInfo(cl_program program, cl_device_id device, cl_program_build_info info) const
    {
        std::size_t size;
        T res;

        clGetProgramBuildInfo(program, device, info, 0, NULL, &size);
        res.resize(size);
        clGetProgramBuildInfo(program, device, info, size, &res[0], 0);

        return res;
    }
} // namespace club