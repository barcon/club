#include "club_event.hpp"

namespace club
{
    EventPtr CreateEvent(cl_event event)
    {
        Error error;
        auto res = Event::Create();

        error = res->Init(event);
        if (error != CL_SUCCESS)
        {
            return nullptr;
        }

        return res;
    }
    Event::~Event()
    {
        clReleaseEvent(event_);
    }
    EventPtr Event::Create()
    {
        class MakeSharedEnabler : public Event
        {
        };

        auto res = std::make_shared<MakeSharedEnabler>();
        return res;
    }
    EventPtr Event::GetPtr()
    {
        return this->shared_from_this();
    }
    ConstEventPtr Event::GetPtr() const
    {
        return const_cast<Event*>(this)->GetPtr();
    }
    Error Event::Init(cl_event event)
    {
        if (initialized_)
        {
            return CL_SUCCESS;
        }

        event_ = event;
        eventInfo_ = GetInfoEvent(event_);
        initialized_ = true;

        return CL_SUCCESS;
    }
    const cl_event& Event::Get() const
    {
        return event_;
    }
    const EventInfo& Event::GetInfo()
    {
        eventInfo_.status = GetEventInfo<cl_int>(event_, CL_EVENT_COMMAND_EXECUTION_STATUS);

        return eventInfo_;
    }
    EventInfo Event::GetInfoEvent(cl_event event) const
    {
        EventInfo res;

        res.queue = GetEventInfo<cl_command_queue>(event, CL_EVENT_COMMAND_QUEUE);
        res.context = GetEventInfo<cl_context>(event, CL_EVENT_CONTEXT);
        res.type = GetEventInfo<cl_command_type>(event, CL_EVENT_COMMAND_TYPE);
        res.status = GetEventInfo<cl_int>(event, CL_EVENT_COMMAND_EXECUTION_STATUS);

        return res;
    }

    template <typename T> typename std::enable_if<!is_vector<T>::value, T>::type Event::GetEventInfo(cl_event event, cl_mem_info info) const
    {
        std::size_t size;
        T res;

        clGetEventInfo(event, info, 0, NULL, &size);
        clGetEventInfo(event, info, size, &res, 0);

        return res;
    }
    template <typename T> typename std::enable_if<is_vector<T>::value, T>::type Event::GetEventInfo(cl_event event, cl_mem_info info) const
    {
        std::size_t size;
        T res;

        clGetEventInfo(event, info, 0, NULL, &size);
        res.resize(size);
        clGetEventInfo(event, info, size, &res[0], 0);

        return res;
    }

} // namespace club