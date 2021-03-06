#pragma once

#include <assert.h>

#include <libgraphics/fxapi.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

/// forward
class Resource;

/// impl: Resource
class Resource : public libgraphics::fxapi::ApiResource,
    public libcommon::INonCopyable {
    public:
        struct Private;

        Resource();
        virtual ~Resource();

        /// returns the gpu-side memory consumption of the
        /// current resource object.
        virtual size_t getGpuMemoryConsumption() = 0;

        /// returns the cpu-side memroy consumption of the
        /// current resource object.
        virtual size_t getCpuMemoryConsumption() = 0;

        /// returns true, if the current resource was already
        /// acquired.
        virtual bool acquired();

        /// tries to acquire the resource for the
        /// current thread.
        virtual bool tryAcquire();

        /// acquires the current resource.
        virtual void acquire();

        /// releases the resource from the
        /// current thread.
        virtual bool release();

        /// forces the release of the
        /// current resource.
        virtual void forceRelease();

    private:
        std::shared_ptr<Private>   d;
};

}
}
}
