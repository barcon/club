#ifndef CLUB_KERNEL_HPP_
#define CLUB_KERNEL_HPP_

#include "club_program.hpp"

namespace club
{
    KernelPtr CreateKernel();
    KernelPtr CreateKernel(ConstProgramPtr program, const String& kernelName, const Dimension& dim);
    KernelPtr CreateKernel(ConstProgramPtr program, const char* kernelName, const Dimension& dim);

    class Kernel : public std::enable_shared_from_this<Kernel>
    {
    public:
        virtual ~Kernel();

        static KernelPtr Create();
        KernelPtr GetPtr();
        ConstKernelPtr GetPtr() const;

        Error Init(ConstProgramPtr program, const String& kernelName);

        const cl_kernel& GetKernel() const;
        const cl_program& GetProgram() const;
        const KernelInfo& GetInfo() const;
        const String& GetName() const;

        void SetArg(const ArgNumber& argNumber, std::size_t size_type, const void* ptr);
        void SetDim(const Dimension& dim);
        void SetLocalSize(const Dimension& dim);
        void SetLocalSize(const LocalSize& localSize);
        void SetLocalSize(const Index& index, const std::size_t size);

        Dimension GetDim() const;
        const LocalSize& GetLocalSize() const;

    protected:
        Kernel() = default;

        KernelInfo GetKernelInfo(cl_kernel kernel) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetKernelInfo(cl_kernel kernel, cl_kernel_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetKernelInfo(cl_kernel kernel, cl_kernel_info info) const;

        bool initialized_{ false };

        ConstProgramPtr program_;

        String kernelName_;
        cl_kernel kernel_;
        KernelInfo kernelInfo_;
        LocalSize localSize_{ 1, 1, 1 };
    };
} // namespace club

#endif /* CLUB_KERNEL_HPP_ */