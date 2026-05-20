function(enable_clang_tidy_for target)
    if(ENABLE_CLANG_TIDY)
        find_program(CLANG_TIDY_PATH clang-tidy REQUIRED)
        set_target_properties(${target}
            PROPERTIES
                CXX_CLANG_TIDY "${CLANG_TIDY_PATH};--warnings-as-errors=-*"
        )
    endif()
endfunction()