#ifndef JET_EXAMPLE_IO_H_
#define JET_EXAMPLE_IO_H_

#include <jet/particle_system_data3.h>
#include <jet/scalar_grid3.h>

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <string>
#include <sys/stat.h>

inline bool ensureDirectory(const std::string& path) {
    if (mkdir(path.c_str(), 0755) == 0 || errno == EEXIST) {
        return true;
    }
    std::perror(path.c_str());
    return false;
}

inline std::string framePath(const std::string& directory, int index,
                             const char* extension) {
    char name[64];
    std::snprintf(name, sizeof(name), "/frame_%04d.%s", index, extension);
    return directory + name;
}

inline bool saveParticlesObj(const std::string& path,
                             const jet::ConstArrayAccessor1<jet::Vector3D>& positions) {
    std::FILE* file = std::fopen(path.c_str(), "w");
    if (!file) {
        std::perror(path.c_str());
        return false;
    }
    for (size_t i = 0; i < positions.size(); ++i) {
        const jet::Vector3D& p = positions[i];
        std::fprintf(file, "v %.6f %.6f %.6f\n", p.x, p.y, p.z);
    }
    std::fclose(file);
    return true;
}

// Mid-depth slice of a cell-centered density field, written as an 8-bit PPM.
inline bool saveDensitySlicePpm(const std::string& path,
                                const jet::ScalarGrid3Ptr& density) {
    const jet::Size3 size = density->dataSize();
    if (size.x == 0 || size.y == 0 || size.z == 0) {
        return false;
    }
    const size_t k = size.z / 2;
    double peak = 1e-6;
    for (size_t j = 0; j < size.y; ++j) {
        for (size_t i = 0; i < size.x; ++i) {
            peak = std::max(peak, (*density)(i, j, k));
        }
    }

    std::FILE* file = std::fopen(path.c_str(), "w");
    if (!file) {
        std::perror(path.c_str());
        return false;
    }
    std::fprintf(file, "P6\n%zu %zu\n255\n", size.x, size.y);
    for (size_t j = size.y; j-- > 0;) {
        for (size_t i = 0; i < size.x; ++i) {
            double t = (*density)(i, j, k) / peak;
            if (t < 0.0) t = 0.0;
            if (t > 1.0) t = 1.0;
            unsigned char gray = static_cast<unsigned char>(t * 255.0);
            unsigned char pixel[3] = {gray, gray, gray};
            std::fwrite(pixel, 1, 3, file);
        }
    }
    std::fclose(file);
    return true;
}

#endif
