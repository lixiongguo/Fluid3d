# ============================================================
#  Fluid3D — WASM Build (Batty free-surface liquid)
# ============================================================
#
#  Usage:
#    make                 Build fluid3d.wasm / fluid3d.js
#    make clean           Remove outputs
#    make check           Verify em++
#

OUTPUT_DIR := ../../../assets/wasm
FLUID_ROOT := ..

CXX      := em++
CXXFLAGS := -O3 -std=c++17 -Wall -Wno-unused-parameter -DFLUID3D_WASM=1
INCLUDES := -I$(FLUID_ROOT)

EM_FLAGS_COMPAT := -s MODULARIZE=1 \
             -s ALLOW_MEMORY_GROWTH=1 \
             -s WASM=1 \
             -s EXPORTED_FUNCTIONS="['_malloc','_free','_fluid3d_init','_fluid3d_advance','_fluid3d_reset','_fluid3d_dispose','_fluid3d_particle_count','_fluid3d_particle_radius','_fluid3d_particles_ptr','_fluid3d_resolution']" \
             -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap','getValue','setValue','HEAPF32']"

SRCS := $(FLUID_ROOT)/fluidsim.cpp \
        $(FLUID_ROOT)/levelset_util.cpp \
        bindings_fluid3d_compat.cpp

.PHONY: all build-fluid3d clean check help

all: build-fluid3d

build-fluid3d: $(OUTPUT_DIR)/fluid3d.js

$(OUTPUT_DIR)/fluid3d.js: $(SRCS) $(FLUID_ROOT)/fluidsim.h
	@mkdir -p $(OUTPUT_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $@ $(EM_FLAGS_COMPAT) -s EXPORT_NAME="Fluid3DSolver"
	@echo "[fluid3d] wrote $(OUTPUT_DIR)/fluid3d.js + fluid3d.wasm"

clean:
	rm -f $(OUTPUT_DIR)/fluid3d.js $(OUTPUT_DIR)/fluid3d.wasm

check:
	@which em++ >/dev/null && em++ --version | head -1 || (echo "em++ not found; source emsdk_env first"; exit 1)

help:
	@echo "make / make build-fluid3d  — build assets/wasm/fluid3d.{js,wasm}"
	@echo "make clean                 — remove outputs"
	@echo "make check                 — verify em++"
