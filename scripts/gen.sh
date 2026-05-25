#!/bin/bash

echo " Generating coverage report with genhtml..."


rm -rf build-coverage
mkdir -p build-coverage
cd build-coverage

echo " Configuring CMake..."
cmake .. -DBUILD_TESTS=ON -DCOVERAGE=ON -DCMAKE_BUILD_TYPE=Debug

echo " Building..."
make -j$(nproc)

echo " Running tests..."
ctest --output-on-failure

echo " Generating coverage report with lcov/genhtml..."
lcov --capture --directory . --output-file coverage.info --no-external
lcov --extract coverage.info "*/Account.cpp" "*/Transaction.cpp" --output-file coverage_filtered.info

genhtml coverage_filtered.info \
        --output-directory coverage_report \
        --title "Banking Library Coverage Report" \
        --legend \
        --demangle-cpp \
        --function-coverage \
        --branch-coverage
echo ""
echo "========================================="
echo "Coverage Summary:"
echo "========================================="
gcovr --root .. --filter ../Account.cpp --filter ../Transaction.cpp --print-summary

echo ""
echo "HTML report generated at: build-coverage/coverage_report/index.html"
echo "gcovr report generated at: build-coverage/coverage_report_gcovr.html"

if command -v xdg-open &> /dev/null; then
    xdg-open coverage_report/index.html
fi
