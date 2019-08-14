#ifndef UTILS_VERSION_HPP
#define UTILS_VERSION_HPP

#include "external/semver.hpp"

namespace utils {
    namespace version {
        using prerelease = semver::prerelease;
    }
    using Version = semver::version;
}

#endif // UTILS_VERSION_HPP
