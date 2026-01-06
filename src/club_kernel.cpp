#include "club_kernel.hpp"
#include <iostream>
#include <algorithm>

namespace club
{
    KernelPtr CreateKernel()
    {
        return Kernel::Create();
    }
    KernelPtr CreateKernel(ConstProgramPtr program, const String& kernelName, const Dimension& dim)
    {
        Error error;
        auto res = Kernel::Create();

        error = res->Init(program, kernelName);
        if (error != CL_SUCCESS)
        {
            return nullptr;
        }

        res->SetLocalSize(dim);

        return res;
    }
    KernelPtr CreateKernel(ConstProgramPtr program, const char* kernelName, const Dimension& dim)
    {
        return CreateKernel(program, static_cast<String>(kernelName), dim);
    }
    Kernel::~Kernel()
    {
        clReleaseKernel(kernel_);
    }
    KernelPtr Kernel::Create()
    {
        class MakeSharedEnabler : public Kernel
        {
        };

        auto res = std::make_shared<MakeSharedEnabler>();
        return res;
    }
    KernelPtr Kernel::GetPtr()
    {
        return shared_from_this();
    }
    ConstKernelPtr Kernel::GetPtr() const
    {
        return const_cast<Kernel*>(this)->GetPtr();
    }
    Error Kernel::Init(ConstProgramPtr program, const String& kernelName)
    {
        Error error;

        if (initialized_)
        {
            return CL_SUCCESS;
        }

        if (!program)
        {
            logger::Error(header, "Invalid program to create kernel");

            return CL_INVALID_PROGRAM;
        }

        program_ = program;
        kernelName_ = kernelName;
        kernel_ = clCreateKernel(program->Get(), kernelName_.c_str(), &error);
        if (error != CL_SUCCESS)
        {
            logger::Error(header, utils::string::Format("Kernel {} could not be created from program: {}", kernelName_, messages.at(error)));

            return error;
        }

        kernelInfo_ = GetKernelInfo(kernel_);
        initialized_ = true;

        return CL_SUCCESS;
    }
    const cl_kernel& Kernel::GetKernel() const
    {
        return kernel_;
    }
    const cl_program& Kernel::GetProgram() const
    {
        return program_->Get();
    }
    const KernelInfo& Kernel::GetInfo() const
    {
        return kernelInfo_;
    }
    const String& Kernel::GetName() const
    {
        return kernelName_;
    }
    KernelInfo Kernel::GetKernelInfo(cl_kernel kernel) const
    {
        KernelInfo res;

        res.functionName = GetKernelInfo<std::vector<char>>(kernel, CL_KERNEL_FUNCTION_NAME);
        res.numberArgs = GetKernelInfo<cl_uint>(kernel, CL_KERNEL_NUM_ARGS);
        res.referenceCount = GetKernelInfo<cl_uint>(kernel, CL_KERNEL_REFERENCE_COUNT);
        res.context = GetKernelInfo<cl_context>(kernel, CL_KERNEL_CONTEXT);
        res.program = GetKernelInfo<cl_program>(kernel, CL_KERNEL_PROGRAM);
        res.attributes = GetKernelInfo<std::vector<char>>(kernel, CL_KERNEL_ATTRIBUTES);

        return res;
    }
    void Kernel::SetArg(const ArgNumber& argNumber, std::size_t size_type, const void* ptr)
    {
        Error error;

        if ((error = clSetKernelArg(kernel_, argNumber, size_type, ptr)) != CL_SUCCESS)
        {
            logger::Error(header, utils::string::Format("Kernel arguments could not be set: {}", messages.at(error)));

            return;
        }
    }
    void Kernel::SetDim(const Dimension& dim)
    {
        localSize_.resize(dim);
        for (auto& it : localSize_)
        {
            it = 1;
        }
    }
    void Kernel::SetLocalSize(const Dimension& dim)
    {
        if (dim != GetDim())
        {
            SetDim(dim);
        }

        auto platformNumber = program_->context_->GetPlatformNumber();
        auto deviceNumber = program_->context_->GetDeviceNumber();
        auto workGroupSize = program_->platform_->GetDeviceInfo(platformNumber, deviceNumber).maxWorkGroupSize;
        std::size_t localSize{ 1 };

        if (program_->platform_->GetDeviceInfo(platformNumber, deviceNumber).type == CL_DEVICE_TYPE_CPU)
        {
            localSize = 1;
        }
        else if (program_->platform_->GetDeviceInfo(platformNumber, deviceNumber).type == CL_DEVICE_TYPE_GPU)
        {
            localSize = std::min(32u, utils::math::Power2Floor(static_cast<unsigned int>(std::pow(workGroupSize, 1. / dim))));
        }

        for (auto& it : localSize_)
        {
            it = localSize;
        }
    }
    void Kernel::SetLocalSize(const LocalSize& localSize)
    {        
        localSize_ = localSize;
    }
    void Kernel::SetLocalSize(const Index& index, const std::size_t size)
    {
        localSize_[index] = size;
    }
    Dimension Kernel::GetDim() const
    {
        return static_cast<Dimension>(localSize_.size());
    }
    const LocalSize& Kernel::GetLocalSize() const
    {
        return localSize_;
    }
    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Kernel::GetKernelInfo(cl_kernel kernel, cl_kernel_info info) const
    {
        std::size_t size;
        T res;

        clGetKernelInfo(kernel, info, 0, NULL, &size);
        clGetKernelInfo(kernel, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Kernel::GetKernelInfo(cl_kernel kernel, cl_kernel_info info) const
    {
        std::size_t size;
        T res;

        clGetKernelInfo(kernel, info, 0, NULL, &size);
        res.resize(size);
        clGetKernelInfo(kernel, info, size, &res[0], 0);

        return res;
    }
} // namespace club