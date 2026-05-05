# Operations on lists
function(ListLength myList len)
    set(_length 0)
    foreach(element IN LISTS ${myList})
        math(EXPR _length "${_length} + 1")
        message("Element : ${element}")
    endforeach()
    set(${len} ${_length} PARENT_SCOPE)
endfunction()

set(len 0)
set(myList 1 2 3 4)
message("myList : ${myList}")

ListLength(myList len)
message("Length = ${len}")


# Operations on lists
function(ListLength list_var_name length_var_name)
    set(_length 0)
    foreach(element IN LISTS ${list_var_name})
        math(EXPR _length "${_length} + 1")
        message("Element : ${element}")
    endforeach()
    set(${length_var_name} ${_length} PARENT_SCOPE)
endfunction()


set(myList 1 2 3 4)
message("myList : ${myList}")

ListLength(myList length)
message("Length = ${length}")