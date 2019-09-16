#ifndef OWL_FILE_UTIL
#define OWL_FILE_UTIL
#include "../base/noncopyable.hpp"
#include <string>
using namespace owl::base;
using namespace std;
namespace owl {
namespace file {
    class WriteFile : noncopyable {
    public:
        explicit WriteFile(string filename);
        ~WriteFile();
        void write(char* log, size_t log_len);
        size_t write_(char*log, size_t log_len);
    private:
        int fd;
    };

} // namespace file
}
#endif // OWL_FILE_UTIL