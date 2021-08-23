find_program(SLANGC slangc
  DOC "Path to the slangc executable.")

# this macro defines cmake rules that execute the following three steps:
# 1) compile the given slang file ${slang_file} to an intermediary PTX file
# 2) use the 'bin2c' tool (that comes with CUDA) to
#    create a second intermediary (.c-)file which defines a const string variable
#    (named '${c_var_name}') whose (constant) value is the PTX output
#    from the previous step.
# 3) assign the name of the intermediary .c file to the cmake variable
#    'output_var', which can then be added to cmake targets.
macro(slang_compile_and_embed output_var slang_file)
  set(c_var_name ${output_var})
  set(ptx_file ${output_var}.ptx)
  add_custom_command(
    OUTPUT ${ptx_file}
    COMMAND ${SLANGC} ${slang_file} -o ${ptx_file} -dump-intermediates -line-directive-mode none
    DEPENDS ${slang_file}
    COMMENT "compile ptx from ${slang_file}"
  )
  set(embedded_file ${ptx_file}_embedded.c)
  add_custom_command(
    OUTPUT ${embedded_file}
    COMMAND ${BIN2C} -c --padd 0 --type char --name ${c_var_name} ${ptx_file} > ${embedded_file}
    DEPENDS ${ptx_file}
    COMMENT "embed ptx from ${slang_file}"
  )
  set(${output_var} ${embedded_file})
endmacro()
