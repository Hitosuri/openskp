#include <chrono>
#include <exception>
#include <iostream>

#include <openskp/instanced_glb.hpp>
#include <openskp/openskp.hpp>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "usage: openskp_export_instanced_glb input.skp output.glb\n";
    return 2;
  }

  try {
    auto t0 = std::chrono::steady_clock::now();
    auto scene = openskp::SkpFile::open(argv[1]).build_instanced_scene();
    auto t1 = std::chrono::steady_clock::now();
    openskp::export_instanced_glb(scene, argv[2]);
    auto t2 = std::chrono::steady_clock::now();

    std::cout << "mesh_resources=" << scene.mesh_resources.size() << "\n"
              << "build_instanced_scene_ms="
              << std::chrono::duration<double, std::milli>(t1 - t0).count() << "\n"
              << "export_instanced_glb_ms="
              << std::chrono::duration<double, std::milli>(t2 - t1).count() << "\n";
  } catch (const std::exception& error) {
    std::cerr << "instanced GLB export failed: " << error.what() << '\n';
    return 1;
  }
}
