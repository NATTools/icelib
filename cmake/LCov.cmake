# TODO: parameterize for reuse
add_custom_target(coverage_report
  COMMAND lcov --directory src/CMakeFiles/icelib.dir --capture --output-file icelib.info
  COMMAND genhtml --output-directory lcov icelib.info
  COMMAND echo "Coverage report in: file://${CMAKE_BINARY_DIR}/lcov/index.html"
)
