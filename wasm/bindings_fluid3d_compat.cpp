// ============================================================
//  Fluid3D compat layer — Batty free-surface solver (ccall API)
// ============================================================
#include "../fluidsim.h"
#include "../vec.h"
#include <emscripten.h>
#include <vector>
#include <cmath>

static FluidSim* g_sim = nullptr;
static int g_res = 0;
static std::vector<float> g_particle_buf;

static float sphere_phi(const Vec3f& position, const Vec3f& centre, float radius) {
   return dist(position, centre) - radius;
}

static Vec3f c0(0.5f, 0.5f, 0.5f);
static float rad0 = 0.35f;

static float boundary_phi(const Vec3f& position) {
   return -sphere_phi(position, c0, rad0);
}

static float liquid_phi(const Vec3f& position) {
   return sphere_phi(position, Vec3f(0.55f, 0.55f, 0.4f), 0.23f);
}

static void flatten_particles() {
   if (!g_sim) {
      g_particle_buf.clear();
      return;
   }
   const std::vector<Vec3f>& p = g_sim->particles;
   g_particle_buf.resize(p.size() * 3);
   for (size_t i = 0; i < p.size(); ++i) {
      g_particle_buf[i * 3 + 0] = p[i][0];
      g_particle_buf[i * 3 + 1] = p[i][1];
      g_particle_buf[i * 3 + 2] = p[i][2];
   }
}

static void setup_scene(int res) {
   if (g_sim) {
      delete g_sim;
      g_sim = nullptr;
   }
   g_res = res;
   g_sim = new FluidSim();
   g_sim->initialize(1.0f, res, res, res);
   g_sim->set_boundary(boundary_phi);
   g_sim->set_liquid(liquid_phi);
   flatten_particles();
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void fluid3d_dispose() {
   if (g_sim) {
      delete g_sim;
      g_sim = nullptr;
   }
   g_res = 0;
   g_particle_buf.clear();
}

EMSCRIPTEN_KEEPALIVE
int fluid3d_init(int res) {
   if (res < 16) res = 16;
   if (res > 48) res = 48;
   setup_scene(res);
   return g_sim ? (int)g_sim->particles.size() : 0;
}

EMSCRIPTEN_KEEPALIVE
void fluid3d_advance(float dt) {
   if (!g_sim) return;
   if (dt <= 0.f) dt = 0.01f;
   g_sim->advance(dt);
   flatten_particles();
}

EMSCRIPTEN_KEEPALIVE
int fluid3d_reset() {
   if (!g_sim || g_res <= 0) return fluid3d_init(32);
   g_sim->particles.clear();
   g_sim->u.set_zero();
   g_sim->v.set_zero();
   g_sim->w.set_zero();
   g_sim->set_liquid(liquid_phi);
   flatten_particles();
   return (int)g_sim->particles.size();
}

EMSCRIPTEN_KEEPALIVE
int fluid3d_particle_count() {
   return g_sim ? (int)g_sim->particles.size() : 0;
}

EMSCRIPTEN_KEEPALIVE
float fluid3d_particle_radius() {
   return g_sim ? g_sim->particle_radius : 0.f;
}

EMSCRIPTEN_KEEPALIVE
float* fluid3d_particles_ptr() {
   return g_particle_buf.empty() ? nullptr : g_particle_buf.data();
}

EMSCRIPTEN_KEEPALIVE
int fluid3d_resolution() {
   return g_res;
}

} // extern "C"
