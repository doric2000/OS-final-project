#!/bin/bash

echo "=== DETAILED COVERAGE SUMMARY ===" > coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "Generated on: $(date)" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt

echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "PROJECT SOURCE FILES (.cpp) ANALYSIS:" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt

for gcov_file in coverage/*.cpp.gcov; do
    if [ -f "$gcov_file" ]; then
        filename=$(basename "$gcov_file" .gcov)
        echo "File: $filename" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        echo "----------------------------------------" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        
        # Extract source file info
        source_line=$(grep "Source:" "$gcov_file" | head -1)
        if [ -n "$source_line" ]; then
            echo "$source_line" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        fi
        
        # Count executable lines (lines with numbers or #####)
        total_executable=$(grep -E "^[[:space:]]*([0-9]+|#####):" "$gcov_file" | wc -l)
        executed_lines=$(grep -E "^[[:space:]]*[0-9]+:" "$gcov_file" | wc -l)
        missed_lines=$(grep -c "^[[:space:]]*#####:" "$gcov_file" 2>/dev/null || echo "0")
        
        # Calculate percentage
        if [ "$total_executable" -gt 0 ]; then
            coverage_percent=$(echo "scale=2; $executed_lines * 100 / $total_executable" | bc 2>/dev/null || echo "0")
            echo "Lines executed: $coverage_percent% of $total_executable" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        else
            coverage_percent="0.00"
            echo "Lines executed: No executable lines" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        fi
        
        echo "Total executable lines: $total_executable" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        echo "Executed lines: $executed_lines" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        echo "Missed lines: $missed_lines" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
    fi
done

echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "PROJECT HEADER FILES (.hpp) ANALYSIS:" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt

for gcov_file in coverage/*.hpp.gcov; do
    if [ -f "$gcov_file" ]; then
        filename=$(basename "$gcov_file" .gcov)
        echo "File: $filename" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        echo "----------------------------------------" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        
        # Extract source file info
        source_line=$(grep "Source:" "$gcov_file" | head -1)
        if [ -n "$source_line" ]; then
            echo "$source_line" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        fi
        
        # Count executable lines for headers
        total_executable=$(grep -E "^[[:space:]]*([0-9]+|#####):" "$gcov_file" | wc -l)
        executed_lines=$(grep -E "^[[:space:]]*[0-9]+:" "$gcov_file" | wc -l)
        
        if [ "$total_executable" -gt 0 ]; then
            coverage_percent=$(echo "scale=2; $executed_lines * 100 / $total_executable" | bc 2>/dev/null || echo "0")
            echo "Lines executed: $coverage_percent% of $total_executable" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        else
            echo "Lines executed: No executable lines" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        fi
        echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
    fi
done

echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "FILES ACHIEVING 80%+ COVERAGE:" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt

for gcov_file in coverage/*.gcov; do
    if [ -f "$gcov_file" ]; then
        filename=$(basename "$gcov_file" .gcov)
        
        # Count executable lines for percentage calculation
        total_executable=$(grep -E "^[[:space:]]*([0-9]+|#####):" "$gcov_file" | wc -l)
        executed_lines=$(grep -E "^[[:space:]]*[0-9]+:" "$gcov_file" | wc -l)
        
        if [ "$total_executable" -gt 0 ]; then
            coverage_percent=$(echo "scale=2; $executed_lines * 100 / $total_executable" | bc 2>/dev/null || echo "0")
            coverage_int=$(echo "$coverage_percent" | cut -d. -f1)
            if [ "$coverage_int" -ge 80 ]; then
                echo "✓ $filename: $coverage_percent%" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
            fi
        fi
    fi
done

echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "FILES BELOW 80% COVERAGE:" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt

for gcov_file in coverage/*.gcov; do
    if [ -f "$gcov_file" ]; then
        filename=$(basename "$gcov_file" .gcov)
        
        # Count executable lines for percentage calculation
        total_executable=$(grep -E "^[[:space:]]*([0-9]+|#####):" "$gcov_file" | wc -l)
        executed_lines=$(grep -E "^[[:space:]]*[0-9]+:" "$gcov_file" | wc -l)
        
        if [ "$total_executable" -gt 0 ]; then
            coverage_percent=$(echo "scale=2; $executed_lines * 100 / $total_executable" | bc 2>/dev/null || echo "0")
            coverage_int=$(echo "$coverage_percent" | cut -d. -f1)
            if [ "$coverage_int" -lt 80 ]; then
                echo "✗ $filename: $coverage_percent%" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
            fi
        else
            echo "? $filename: No executable lines" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
        fi
    fi
done

echo "" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "END OF DETAILED ANALYSIS" >> coverage/DETAILED_COVERAGE_SUMMARY.txt
echo "============================================" >> coverage/DETAILED_COVERAGE_SUMMARY.txt

echo "Detailed coverage summary generated in coverage/DETAILED_COVERAGE_SUMMARY.txt"
