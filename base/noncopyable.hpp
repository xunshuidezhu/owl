/**
 * @brief noncopyable 
 * @author OWL
 * */
#ifndef OWL_BASE_NONCOPYABLE_H
#define OWL_BASE_NONCOPYABLE_H
namespace owl {
namespace base {

    class noncopyable {
    public:
        noncopyable(const noncopyable&) = delete;
        void operator=(const noncopyable&) = delete;

    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };

} // namespace base
} // namespace owl

#endif // OWL_BASE_NONCOPYABLE_H
