#include "club_kernel.hpp"
#include <iostream>
#include <algorithm>

namespace club
{
    KernelPtr CreateKernel()
    {
        return Kernel::Create();
    }
    KernelPtr CreateKernel(ProgramPtr program, const String& kernelName, const Dimension& dim)
    {
        Error error;
        auto res = Kernel::Create();

        error = res->Init(program, kernelName);
        if (error != CL_SUCCESS)
        {
            return nullptr;
        }

        res->SetDim(dim);

        return res;
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
 
    Error Kernel::Init(ProgramPtr program, const String& kernelName)
    {
        Error error;

        if (kernel_)
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

        return CL_SUCCESS;
    }
    const cl_kernel& Kernel::GetKernel() const
    {
        return kernel_;
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
        if (dim < 1 || dim > 3)
        {
            logger::Error(header, utils::string::Format("Invalid dimension {}. Dimension must be between 1 and 3", dim));
			
            return;
        }

		dim_ = dim;
    }
    Dimension Kernel::GetDim() const
    {
        return dim_;
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