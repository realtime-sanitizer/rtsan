cmake_minimum_required(VERSION 3.12)

project(rtsan_examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fsanitize=realtime)
add_link_options(-fsanitize=realtime)

if(NOT APPLE)
    add_link_options(-latomic)
endif()

add_custom_target(rtsan_examples)

function(rtsan_example_add example_name)
    set(rtsan_example_target rtsan_example_${example_name})
    add_executable(rtsan_example_${example_name} ${example_name}.cpp)
    add_dependencies(rtsan_examples ${rtsan_example_target})
endfunction()

rtsan_example_add(atomic_store_2d_coords)
rtsan_example_add(atomic_store_3d_coords)
rtsan_example_add(lambda_templated_invoke_big_capture)
rtsan_example_add(shared_ptr_assign)
rtsan_example_add(shared_ptr_atomic_assign)
rtsan_example_add(std_function_big_capture)
rtsan_example_add(std_function_small_capture)
rtsan_example_add(vector_construct)
