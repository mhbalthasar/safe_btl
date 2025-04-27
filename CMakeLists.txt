
macro(include_btl TARGET) #定义一个宏批量设置依赖
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    if(WIN32)
        target_link_libraries(${TARGET} PRIVATE rpcrt4)
    else()
        target_link_libraries(${TARGET} PRIVATE rt uuid)
    endif()
endmacro()