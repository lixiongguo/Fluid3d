# Build Fluid3D WASM without requiring GNU make (Windows-friendly)
param(
    [string]$OutDir = ""
)

$ErrorActionPreference = "Stop"
$WasmDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$FluidRoot = Split-Path -Parent $WasmDir
$CppRoot = Split-Path -Parent $FluidRoot
$RepoRoot = Split-Path -Parent $CppRoot
if (-not $OutDir) { $OutDir = Join-Path $RepoRoot "assets\wasm" }

$EmsdkEnv = Join-Path $CppRoot "emsdk\emsdk_env.ps1"
if (Test-Path $EmsdkEnv) { . $EmsdkEnv }

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$OutJs = Join-Path $OutDir "fluid3d.js"

$srcs = @(
    (Join-Path $FluidRoot "fluidsim.cpp"),
    (Join-Path $FluidRoot "levelset_util.cpp"),
    (Join-Path $WasmDir "bindings_fluid3d_compat.cpp")
)

Write-Host "[Fluid3D] em++ -> $OutJs"
& em++ -O3 -std=c++17 -Wall -Wno-unused-parameter -DFLUID3D_WASM=1 "-I$FluidRoot" @srcs -o $OutJs `
    -s MODULARIZE=1 `
    -s ALLOW_MEMORY_GROWTH=1 `
    -s WASM=1 `
    "-sEXPORTED_FUNCTIONS=['_malloc','_free','_fluid3d_init','_fluid3d_advance','_fluid3d_reset','_fluid3d_dispose','_fluid3d_particle_count','_fluid3d_particle_radius','_fluid3d_particles_ptr','_fluid3d_resolution']" `
    "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','getValue','setValue','HEAPF32']" `
    -s EXPORT_NAME="Fluid3DSolver"

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "[Fluid3D] done"
