# lib name
set(FULL_NAME ActGeant4)
# exec name
set(EXEC_NAME actgeant)

# set files to compile
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hh)

# LinkDef and dict generation
# set(LINKDEF_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/${FULL_NAME}LinkDef.hh)
# set(DICTIONARY_NAME G__${FULL_NAME})
# ROOT_GENERATE_DICTIONARY(${DICTIONARY_NAME} ${headers} LINKDEF  ${LINKDEF_LOCATION})

# build executable
add_executable(${EXEC_NAME} ${EXEC_NAME}.cc ${sources} ${headers})
target_link_libraries(${EXEC_NAME} ${Geant4_LIBRARIES} ActCore ActPhysics ActSimulation)
# install it in bin
install(TARGETS ${EXEC_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# # copy macros
# set(MACROS
#   gui.mac
#   init_vis.mac
#   vis.mac
#   )

# foreach(_macro ${MACROS})
#   configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/macros/${_macro}
#     ${CMAKE_INSTALL_PREFIX}/bin/${_macro}
#     COPYONLY
#     )
# endforeach()
