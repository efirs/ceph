add_library(ceph_example SHARED compressor_plugin_example.cc)

# unittest_compression
add_executable(unittest_compression
  test_compression.cc
  )
add_ceph_unittest(unittest_compression ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unittest_compression)
target_link_libraries(unittest_compression global)
add_dependencies(unittest_compression ceph_example)
