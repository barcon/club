#ifndef CLUB_EVENT_HPP_
#define CLUB_EVENT_HPP_

#include "club_messages.hpp"
#include "club_types.hpp"

namespace club
{
    EventPtr CreateEvent(cl_event event);

    class Event : public std::enable_shared_from_this<Event>
    {
    public:
        virtual ~Event();

        static EventPtr Create();
        EventPtr GetPtr();
        ConstEventPtr GetPtr() const;

        Error Init(cl_event event);

        const cl_event& Get() const;
        const EventInfo& GetInfo();

    protected:
        Event() = default;

        EventInfo GetInfoEvent(cl_event event) const;

        template <typename T>
        typename std::enable_if<!is_vector<T>::value, T>::type GetEventInfo(cl_event event, cl_mem_info info) const;
        template <typename T>
        typename std::enable_if<is_vector<T>::value, T>::type GetEventInfo(cl_event event, cl_mem_info info) const;

        bool initialized_{ false };
        cl_event event_;
        EventInfo eventInfo_;
    };
} // namespace club

#endif