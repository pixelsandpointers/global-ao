# if we build in Debug mode, enable the global_ao_DEBUG compiler definition
add_compile_definitions(
        $<$<CONFIG:Debug>:global_ao_DEBUG>
)