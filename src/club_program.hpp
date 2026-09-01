#ifndef CLUB_PROGRAM_HPP_
#define CLUB_PROGRAM_HPP_

#include "club_context.hpp"

namespace club
{
    ProgramPtr CreateProgram();
    ProgramPtr CreateProgramFromString(ContextPtr context, const String& source);
    ProgramPtr CreateProgramFromFile(ContextPtr context, const String& fileName);
    ProgramPtr CreateProgramFromFile(ContextPtr context, const char* fileName);

    class Program : public std::enable_shared_from_this<Program>
    {
    public:
        ~Program();

        static ProgramPtr Create();
        ProgramPtr GetPtr();
        ConstProgramPtr GetPtr() const;

        Error Init(ContextPtr context, const String& source);

        const cl_program& Get() const;
        const ProgramInfo& GetInfo() const;
        const String& GetSource() const;

        friend Kernel;

    protected:
        Program() = default;

        ProgramInfo GetProgramInfo(cl_program program, cl_device_id device) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetProgramInfo(cl_program program, cl_program_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetProgramInfo(cl_program program, cl_program_info info) const;

        template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type GetProgramBuildInfo(cl_program program, cl_device_id device, cl_program_build_info info) const;
        template <typename T> typename std::enable_if<is_vector<T>::value, T>::type GetProgramBuildInfo(cl_program program, cl_device_id device, cl_program_build_info info) const;

        String source_;
        cl_program program_{ nullptr };
        ProgramInfo programInfo_;
    };
} // namespace club

#endif /* CLUB_PROGRAM_HPP_ */