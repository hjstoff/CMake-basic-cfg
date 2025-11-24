#ifndef EVENT_TIMESTAMP_H
#define EVENT_TIMESTAMP_H
#include <ctime>
#include <cstdint>
#include <cerrno>
#include <type_traits>

namespace event_timestamp_detail {
/**
 * @brief is_valid_clock is a custom type trait to validate clock IDs for
 * the event_timestamp class template
 *
 * Only realtime clocks, clocks that keep calendar time,  make sense.
 * It is a peculiar language restriction of C++ that the explicit template
 * specializations are needed for this cannot be defined inside scope of the
 * class to which they are applied.
 * Explicit template specializations MUST be at namespace scope. Defining them
 * in the default global namespace is not a very clean approach, but since we
 * cannot encapsulate the type trait implementation inside the class to which
 * they belong, a separate namespace is introduced here, exclusively for this
 * purpose.
 */
template<clockid_t CLOCK>
struct is_valid_clock : std::false_type {};

template<>
struct is_valid_clock<CLOCK_REALTIME> : std::true_type {};

template<>
struct is_valid_clock<CLOCK_REALTIME_COARSE> : std::true_type {};
};

/**
 * @brief Template class event_timestamp is a lightweight packaged version of
 * the POSIX, C and C++, standards struct timespec.
 *
 * The timestamps of this class must denote a calendar time that can be
 * converted to human-readable date time string that is non-ambigous about
 * its timezone context. Only realtime clocks make sense for this purpose
 * and the templated class only accepts clocks as non-type template argument
 * that qualify as such.
 *
 * The template class event_timestamp is designed to be trivially copyable
 * and without dynamic allocation of resources. It is also designed to be
 * free of exceptions, as one reason for having the class is for use in
 * descriptions of events that reports some sort of failure and that may end
 * up as the content of exceptions. It would be complicated if the
 * construction of exception content would be interrupted by another
 * exception.
 *
 * The default constructor sets the state of the object either to the current
 * time (in struct timespec "format"), by means of clock_gettime(2), or,
 * if that fails, to a well-defined state denoting "invalid timestamp".
 * Note that no standard (POSIX, C, C++) specifies a specific value for a
 * struct timespec to denote an invalid timestamp. Here we can nevertheless
 * add such a specification. POSIX guarantees that a succesful
 * clock_gettime(2) call returns a normalized result, with tv_nsec being in
 * the range [0 - 999999999]. So, setting a it to a negative value is a good
 * way to denote the result of a failed call.
 *
 * Member function is_valid() returns false if this invalid state exist in
 * the object.
 *
 * Member function gettime_errorcode() either returns 0, if the object
 * contains a succesfully obtained timestamp, or the errno value set
 * by the unsuccsesful clock_gettime(2) call.
 *
 * Static member function nanoseconds_resolution() either returns the
 * resolution * in nanonseconds of the clock used, or a negative value if
 * that resolution could not be obtained. Changing the sign of a negative value
 * returned yields the errno value set when clock_getres(2) failed, or it
 * yields ERANGE. The latter value is set in the highly improbable case that
 * clock_getres(2) is successful, but the resolution retrieved is so coarse
 * that it cannot be represented in nanoseconds by the integral datatype
 * (int64_t) of the return value of nanoseconds_resolution.
 *
 * Two operators, for explicitly casting an event_timestamp object to a
 * pointer to  const struct timespec, and for explicitly casting an
 * event_timestamp object to a pointer to const time_t, are provided to
 * give read-only access to the the internal data.
 */
template <clockid_t CLOCK_ID = CLOCK_REALTIME_COARSE>
class event_timestamp {
    static_assert(event_timestamp_detail::is_valid_clock<CLOCK_ID>::value,
        "Invalid clockid_t template argument for template class event_timestamp"
    );

    timespec    ts;
public:
    static std::int64_t nanoseconds_resolution() noexcept
    {
        static std::int64_t cached_resolution = -1;
        if (cached_resolution < 0) {
            timespec res;
            if (clock_getres(CLOCK_ID, &res) == -1) {
                return (std::uint64_t) -errno;
            }
            if (res.tv_sec > INT64_MAX / 1000000000) {
                return -ERANGE;
            }
            cached_resolution = ((std::int64_t) res.tv_sec * 1000000000) +  res.tv_nsec;
        }
        return cached_resolution;
    }

    event_timestamp() noexcept
    {
            ts.tv_sec = 0;
            ts.tv_nsec = -1;
    }

    int gettime() noexcept
    {
        if (clock_gettime(CLOCK_ID, &ts) == -1) {
            ts.tv_nsec = -1;
            ts.tv_sec = 0;
            return errno;
        }
        return 0;
    }

    explicit operator const timespec *() const noexcept { return &ts; }

    explicit operator const time_t *() const noexcept { return &ts.tv_sec; }

    bool is_valid() const noexcept { return ts.tv_nsec >= 0; }

};

/**
 * @brief tm_extended is a plain old data structure that extends the standard
 * struct tm with an additional member, tm_nsec, specifically for holding a
 * fraction of a second expressed in nanoseconds.
 *
 * The nanoseconds precision is available in struct timespec.
 *
 * An operator for explicitily casting the tm_extended object to a pointer of
 * base class struct tm is added in order to facilitate initialization of the
 * original struct tm members (tm_year, tm_mon, tm_mday, etc.) by the C
 * language functions already available for this purpose, viz. gmtime_r(3) and
 * localtime_r(3).
 *
 * The cast is safe because tm_extended inherits from tm, placing tm in
 * accordance with C++ lay-out priciples at offset 0 of the tm_extended struct.
 * To be dead sure about this lay-out assumption, it is verified at compiletime
 * by the static_assert below (it must be outside of the definition of
 * tm_extended since it would otherwise pertain to the definition of a type
 * that is still incompletely defined).
 */
struct tm_extended : public tm {
    long        tm_nsec;
    explicit    operator tm *() noexcept { return static_cast<tm *>(this); }
};
static_assert(offsetof(tm_extended, tm_nsec) == sizeof(struct tm),
    "tm_extended layout assumption violated!"
);

/**
 * @brief set a tm_extended to the extended broken-down time representation of
 * the event_timestamp that is appropriate for the GMT timezone.
 * @param evts a const reference to aanevent_timestamp
 * @param brokendown_time a reference to a mutable tm_extended object
 * @return 0 on success. Return EINVAL if is invalid, or EOVERFLOW if
 * the time_t value in evts cannot be represented in (the tm_year field of)
 * a struct tm.
 */
template <clockid_t CLOCK_ID>
int get_gmtime(const event_timestamp<CLOCK_ID> &evts, tm_extended &brokendown_time)
{
    if (! evts.is_valid()) {
        return EINVAL;
    }
    if (::gmtime_r((const time_t *) evts, (struct tm *) brokendown_time) == nullptr) {
        return errno;
    }
    return 0;
}

/**
 * @brief set a tm_extended to the extended broken-down time representation of
 * the event_timestamp that is appropriate for the local timezone.
 * @param evts a const reference to aanevent_timestamp
 * @param brokendown_time a reference to a mutable tm_extended object
 * @return 0 on success. Return EINVAL if is invalid, or EOVERFLOW if
 * the time_t value in evts cannot be represented in (the tm_year field of)
 * a struct tm.
 */
template <clockid_t CLOCK_ID>
int get_localtime(const event_timestamp<CLOCK_ID> &evts, tm_extended &brokendown_time)
{
    if (! evts.is_valid()) {
        return EINVAL;
    }
    if (::localtime_r((const time_t *) evts, (struct tm *) brokendown_time) == nullptr) {
        return errno;
    }
    return 0;
}

#endif // EVENT_TIMESTAMP_H
