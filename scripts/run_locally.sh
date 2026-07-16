#!/bin/bash

set -e

BINARY_HOME="./bin"
INPUT_HOME="./input"
INPUT_FILE="${INPUT_HOME}/AB_NYC_2019.csv"
OUTPUT_DIR="./output"

# Создаём директорию для выходных файлов
mkdir -p ${OUTPUT_DIR}

echo "========================================="
echo "MapReduce - Airbnb Price Statistics"
echo "========================================="
echo ""

# Проверяем наличие входного файла
if [ ! -f "${INPUT_FILE}" ]; then
    echo "Error: Input file not found: ${INPUT_FILE}"
    exit 1
fi

echo "1. Running MapReduce pipeline..."
echo "   Input:  ${INPUT_FILE}"
echo ""

# Запускаем MapReduce пайплайн
cat "${INPUT_FILE}" | \
    ${BINARY_HOME}/mapreduce mapper | \
    sort -k1 | \
    ${BINARY_HOME}/mapreduce reducer > "${OUTPUT_DIR}/result.txt"

echo "2. MapReduce Results:"
echo "   -----------------------------------------"
cat "${OUTPUT_DIR}/result.txt"
echo "   -----------------------------------------"
echo ""

echo "3. Direct computation (verification):"
echo "   -----------------------------------------"
${BINARY_HOME}/mapreduce stats "${INPUT_FILE}"
echo "   -----------------------------------------"
echo ""

echo "4. Output saved to: ${OUTPUT_DIR}/result.txt"
echo ""

echo "Done!"