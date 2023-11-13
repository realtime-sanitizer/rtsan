cmake_minimum_required(VERSION 3.12)

project(radsan_examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fsanitize=realtime)
add_link_options(-fsanitize=realtime)

add_executable(radsan_example_atomic_store_2d_coords 
               atomic_store_2d_coords.cpp)               
add_executable(radsan_example_atomic_store_3d_coords 
               atomic_store_3d_coords.cpp)               
add_executable(radsan_example_lambda_templated_invoke_big_capture 
               lambda_templated_invoke_big_capture.cpp)               
add_executable(radsan_example_shared_ptr_assign 
               shared_ptr_assign.cpp)               
add_executable(radsan_example_shared_ptr_atomic_assign 
               shared_ptr_atomic_assign.cpp)               
add_executable(radsan_example_std_function_big_capture
               std_function_big_capture.cpp)               
add_executable(radsan_example_std_function_small_capture
               std_function_small_capture.cpp)               
add_executable(radsan_example_vector_construct
               vector_construct.cpp)               