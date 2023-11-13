cmake_minimum_required(VERSION 3.12)

project(radsan_examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fsanitize=realtime)
add_link_options(-fsanitize=realtime)

if(NOT APPLE)
    add_link_options(-latomic)
endif()

add_custom_target(radsan_examples)

function(radsan_example_add example_name)
    set(radsan_example_target radsan_example_${example_name})
    add_executable(radsan_example_${example_name} ${example_name}.cpp)
    add_dependencies(radsan_examples ${radsan_example_target})
endfunction()

radsan_example_add(atomic_store_2d_coords)
radsan_example_add(atomic_store_3d_coords)
radsan_example_add(lambda_templated_invoke_big_capture)
radsan_example_add(shared_ptr_assign)
radsan_example_add(shared_ptr_atomic_assign)
radsan_example_add(std_function_big_capture)
radsan_example_add(std_function_small_capture)
radsan_example_add(vector_construct)
