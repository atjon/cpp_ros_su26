cmake_minimum_required(VERSION 3.10)
project(cpp_ros_su26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example example.cpp)
add_executable(print print.cpp)
add_executable(variables variables.cpp)
add_executable(functions functions.cpp)
add_executable(namespaces namespaces.cpp)
add_executable(conditional_compile conditional_compile.cpp)
add_executable(headers headers.cpp)
add_executable(datatypes datatypes.cpp)
add_executable(string string.cpp)
add_executable(operators operators.cpp)
add_executable(constexpr constexpr.cpp)
add_executable(scopes scopes.cpp)
add_executable(linkage linkage.cpp linkageCopy.cpp inline.cpp)
add_executable(constants constants.cpp constants.h inline.cpp) 
add_executable(goto goto.cpp)
add_executable(random random.cpp)
add_executable(casting casting.cpp)

# ------------------------------------------------------------------------------
# Numbered lesson files, named after their learncpp.com section.
# Everything above this line predates the numbering scheme.
# ------------------------------------------------------------------------------

# Chapter 10 - Type Conversion, Type Aliases, and Type Deduction (10.7 onward;
# 10.1-10.6 are covered by casting.cpp)
add_executable(10.7_typedefs_and_type_aliases 10.7_typedefs_and_type_aliases.cpp)
add_executable(10.8-10.9_type_deduction 10.8-10.9_type_deduction.cpp)

# Chapter 11 - Function Overloading and Function Templates
add_executable(11.1-11.3_function_overloading 11.1-11.3_function_overloading.cpp)
add_executable(11.4-11.5_deleting_functions_and_default_arguments 11.4-11.5_deleting_functions_and_default_arguments.cpp)
add_executable(11.6-11.7_function_templates 11.6-11.7_function_templates.cpp)
add_executable(11.8-11.9_multiple_template_types_and_nttp 11.8-11.9_multiple_template_types_and_nttp.cpp)
# two translation units + a header, to show why template definitions must live
# in the header (same multi-file pattern as the linkage target above)
add_executable(11.10_templates_in_multiple_files
               11.10_templates_in_multiple_files.cpp
               11.10_telemetry.cpp
               11.10_templates.h)

# Chapter 12 - Compound Types: References and Pointers
add_executable(12.1-12.2_compound_types_and_value_categories 12.1-12.2_compound_types_and_value_categories.cpp)
add_executable(12.3-12.6_lvalue_references 12.3-12.6_lvalue_references.cpp)
add_executable(12.7-12.9_pointers_null_and_const 12.7-12.9_pointers_null_and_const.cpp)
add_executable(12.10-12.13_pass_by_address_and_return_by_reference 12.10-12.13_pass_by_address_and_return_by_reference.cpp)
add_executable(12.14-12.15_type_deduction_with_pointers_and_optional 12.14-12.15_type_deduction_with_pointers_and_optional.cpp)

# Chapter 13 - Compound Types: Enums and Structs
add_executable(13.1-13.4_program_defined_types_and_unscoped_enums 13.1-13.4_program_defined_types_and_unscoped_enums.cpp)
add_executable(13.5-13.6_io_operators_and_scoped_enums 13.5-13.6_io_operators_and_scoped_enums.cpp)
add_executable(13.7-13.9_structs_and_initialization 13.7-13.9_structs_and_initialization.cpp)
add_executable(13.10-13.12_passing_structs_and_member_selection 13.10-13.12_passing_structs_and_member_selection.cpp)
add_executable(13.13-13.15_class_templates_ctad_and_alias_templates 13.13-13.15_class_templates_ctad_and_alias_templates.cpp)

# Chapter 14 - Introduction to Classes
add_executable(14.1-14.3_intro_to_classes_and_member_functions 14.1-14.3_intro_to_classes_and_member_functions.cpp)
add_executable(14.4-14.6_const_objects_access_specifiers_and_access_functions 14.4-14.6_const_objects_access_specifiers_and_access_functions.cpp)
add_executable(14.7-14.8_returning_references_and_data_hiding 14.7-14.8_returning_references_and_data_hiding.cpp)
add_executable(14.9-14.12_constructors 14.9-14.12_constructors.cpp)
add_executable(14.13-14.17_copy_constructors_and_conversions 14.13-14.17_copy_constructors_and_conversions.cpp)

# Chapter 15 - More on Classes
add_executable(15.1_this_pointer_and_chaining 15.1_this_pointer_and_chaining.cpp)
# class definition in the header, member function definitions in their own .cpp
add_executable(15.2-15.3_classes_in_headers_and_nested_types
               15.2-15.3_classes_in_headers_and_nested_types.cpp
               15.2_battery.cpp
               15.2_battery.h)
add_executable(15.4-15.5_destructors_and_class_templates_with_member_functions 15.4-15.5_destructors_and_class_templates_with_member_functions.cpp)
add_executable(15.6-15.7_static_members 15.6-15.7_static_members.cpp)
add_executable(15.8-15.10_friends_and_ref_qualifiers 15.8-15.10_friends_and_ref_qualifiers.cpp)

# Chapter 16 - Dynamic arrays: std::vector
add_executable(16.1-16.3_intro_to_containers_and_vectors 16.1-16.3_intro_to_containers_and_vectors.cpp)
add_executable(16.4-16.5_passing_and_returning_vectors 16.4-16.5_passing_and_returning_vectors.cpp)
add_executable(16.6-16.8_arrays_loops_and_range_based_for 16.6-16.8_arrays_loops_and_range_based_for.cpp)
add_executable(16.9_array_indexing_and_length_using_enumerators 16.9_array_indexing_and_length_using_enumerators.cpp)
add_executable(16.10-16.12_vector_resizing_capacity_and_stack_behavior 16.10-16.12_vector_resizing_capacity_and_stack_behavior.cpp)

# Chapter 17 - Fixed-size arrays (17.7 onward; 17.1-17.6 cover std::array)
add_executable(17.7-17.9_c_style_arrays_decay_and_pointer_arithmetic 17.7-17.9_c_style_arrays_decay_and_pointer_arithmetic.cpp)
add_executable(17.10-17.11_c_style_strings_and_symbolic_constants 17.10-17.11_c_style_strings_and_symbolic_constants.cpp)
add_executable(17.12-17.13_multidimensional_arrays 17.12-17.13_multidimensional_arrays.cpp)

# Chapter 20 - Functions
add_executable(20.1-20.2_function_pointers_and_the_stack_and_heap 20.1-20.2_function_pointers_and_the_stack_and_heap.cpp)
add_executable(20.3_recursion 20.3_recursion.cpp)
# takes command line arguments -- run it with some, e.g. `./build/20.4-... 42 hello`
add_executable(20.4-20.5_command_line_arguments_and_ellipsis 20.4-20.5_command_line_arguments_and_ellipsis.cpp)
add_executable(20.6-20.7_lambdas_and_captures 20.6-20.7_lambdas_and_captures.cpp)

# Chapter 22 - Move Semantics and Smart Pointers
add_executable(22.1-22.4_move_semantics 22.1-22.4_move_semantics.cpp)
add_executable(22.5-22.6_unique_ptr_and_shared_ptr 22.5-22.6_unique_ptr_and_shared_ptr.cpp)
add_executable(22.7_circular_dependencies_and_weak_ptr 22.7_circular_dependencies_and_weak_ptr.cpp)