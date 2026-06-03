#!/bin/bash

echo "========================================="
echo "Generating coverage report with genhtml"
echo "========================================="

# Очистка и сборка
rm -rf build
mkdir build && cd build

# Конфигурация с покрытием
echo "Configuring CMake..."
cmake .. -DBUILD_TESTS=ON -DCOVERAGE=ON -DCMAKE_BUILD_TYPE=Debug

# Сборка
echo "Building..."
make -j$(nproc)

# Запуск тестов
echo "Running tests..."
ctest --output-on-failure

# Генерация отчета с lcov
echo "Collecting coverage data..."
lcov --capture --directory . --output-file coverage.info --no-external

echo "Filtering coverage data..."
lcov --extract coverage.info "*/Account.cpp" "*/Transaction.cpp" -o coverage_filtered.info

echo "Generating HTML report with genhtml..."
genhtml coverage_filtered.info \
        --output-directory coverage_report \
        --title "Banking Library Coverage Report" \
        --legend \
        --demangle-cpp \
        --function-coverage \
        --branch-coverage \
        --highlight \
        --prefix ../

echo "Coverage summary:"
gcovr --root .. --filter '../Account.cpp' --filter '../Transaction.cpp' --print-summary

echo ""
echo "========================================="
echo "Coverage report generated at: build/coverage_report/index.html"
echo "========================================="

# Открыть в браузере (если есть GUI)
if command -v xdg-open &> /dev/null; then
    read -p "Open report in browser? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        xdg-open coverage_report/index.html
    fi
fi
