#include <string>
#ifdef __GNUC__
#include <filesystem>
#endif
#ifdef _MSC_VER
#include <filesystem>
#endif

namespace MuCplGen
{
#ifdef __GNUC__
namespace FileSystem = std::filesystem;
#endif
#ifdef _MSC_VER
namespace FileSystem = std::filesystem;
#endif 
}