#!/usr/bin/env python3
"""
Vietnamese Input Validation Script for EndKey

This script validates Vietnamese input method accuracy by analyzing
test results and checking for correct character conversions.
"""

import xml.etree.ElementTree as ET
import sys
import os
import re
from typing import Dict, List, Tuple, Set
from dataclasses import dataclass
from unicodedata import normalize, name

@dataclass
class ValidationResult:
    test_name: str
    input_text: str
    expected: str
    actual: str
    is_correct: bool
    error_type: str = ""
    details: str = ""

class VietnameseInputValidator:
    def __init__(self):
        # Vietnamese character sets for validation
        self.vowel_bases = {'a', 'ă', 'â', 'e', 'ê', 'i', 'o', 'ô', 'ơ', 'u', 'ư', 'y'}
        self.tone_marks = {'́', '̀', '̉', '̃', '̣'}  # acute, grave, hook, tilde, dot
        self.special_chars = {'đ', 'Đ'}

        # Common Vietnamese words for validation
        self.common_words = {
            'viet', 'nguoi', 'quoc', 'thu', 'thoi', 'vien', 'toan',
            'thuong', 'thien', 'thanh', 'thuc', 'thuoc', 'thu',
            'xin', 'chao', 'cam', 'on', 'rat', 'vui', 'khi', 'gap', 'ban'
        }

        # Expected conversions for Telex input
        self.telex_conversions = {
            'aws': 'âu', 'awf': 'àu', 'awr': 'ảu', 'awx': 'ãu', 'awj': 'ạu',
            'oas': 'oá', 'oaf': 'oà', 'oar': 'oả', 'oax': 'oã', 'oaj': 'oạ',
            'uws': 'ứ', 'uwf': 'ủ', 'uwr': 'ủ', 'uwx': 'ũ', 'uwj': 'ụ',
            'dd': 'đ',
            'ow': 'ớ', 'oof': 'ờ', 'oor': 'ở', 'oox': 'ỡ', 'ooj': 'ợ',
            'as': 'á', 'af': 'à', 'ar': 'ả', 'ax': 'ã', 'aj': 'ạ'
        }

    def parse_test_results(self, xml_file: str) -> List[ValidationResult]:
        """Parse GoogleTest XML output and extract Vietnamese input test results."""
        tree = ET.parse(xml_file)
        root = tree.getroot()

        results = []

        for testcase in root.findall('testcase'):
            test_name = f"{testcase.get('classname')}.{testcase.get('name')}"

            if 'VietnameseInputTest' in test_name:
                # Extract test data from failure message or system-out
                failure = testcase.find('failure')
                if failure is not None:
                    result = self._extract_validation_from_failure(test_name, failure.get('message', ''))
                    if result:
                        results.append(result)

                system_out = testcase.find('system-out')
                if system_out is not None:
                    result = self._extract_validation_from_output(test_name, system_out.text)
                    if result:
                        results.append(result)

        return results

    def _extract_validation_from_failure(self, test_name: str, message: str) -> ValidationResult:
        """Extract validation data from test failure message."""
        # Parse message format: "Failed: description - Input: 'input' - Expected: 'expected' - Got: 'actual'"
        pattern = r"Failed: .*? - Input: '(.*?)' - Expected: '(.*?)' - Got: '(.*?)'"
        match = re.search(pattern, message)

        if match:
            input_text, expected, actual = match.groups()
            is_correct = (actual == expected)
            error_type = self._classify_error(expected, actual) if not is_correct else ""

            return ValidationResult(
                test_name=test_name,
                input_text=input_text,
                expected=expected,
                actual=actual,
                is_correct=is_correct,
                error_type=error_type
            )

        return None

    def _extract_validation_from_output(self, test_name: str, output: str) -> ValidationResult:
        """Extract validation data from system-out output."""
        # Similar to above but parse from system-out
        if output is None:
            return None

        pattern = r"Input: (.*?)\nExpected: (.*?)\nActual: (.*?)"
        match = re.search(pattern, output)

        if match:
            input_text, expected, actual = match.groups()
            is_correct = (actual == expected)
            error_type = self._classify_error(expected, actual) if not is_correct else ""

            return ValidationResult(
                test_name=test_name,
                input_text=input_text,
                expected=expected,
                actual=actual,
                is_correct=is_correct,
                error_type=error_type
            )

        return None

    def _classify_error(self, expected: str, actual: str) -> str:
        """Classify the type of Vietnamese input error."""
        if expected == actual:
            return ""

        # Check for missing tone mark
        if self._has_tone_mark(expected) and not self._has_tone_mark(actual):
            return "missing_tone_mark"

        # Check for wrong tone mark
        if (self._has_tone_mark(expected) and self._has_tone_mark(actual) and
            self._get_tone_mark(expected) != self._get_tone_mark(actual)):
            return "wrong_tone_mark"

        # Check for missing vowel modification
        if self._has_vowel_modification(expected) and not self._has_vowel_modification(actual):
            return "missing_vowel_modification"

        # Check for wrong vowel modification
        if (self._has_vowel_modification(expected) and self._has_vowel_modification(actual) and
            not self._same_vowel_base(expected, actual)):
            return "wrong_vowel_modification"

        # Check for character substitution
        if len(expected) == len(actual) == 1:
            return "character_substitution"

        # Check for missing special character
        if any(c in self.special_chars for c in expected) and not any(c in self.special_chars for c in actual):
            return "missing_special_character"

        # Check for incorrect conversion
        if self._is_telex_input(actual) and not self._is_valid_vietnamese(expected):
            return "incorrect_conversion"

        return "unknown_error"

    def _has_tone_mark(self, text: str) -> bool:
        """Check if text contains Vietnamese tone marks."""
        return any(char in self.tone_marks for char in normalize('NFD', text))

    def _get_tone_mark(self, text: str) -> str:
        """Extract the tone mark from Vietnamese text."""
        normalized = normalize('NFD', text)
        for char in normalized:
            if char in self.tone_marks:
                return char
        return ""

    def _has_vowel_modification(self, text: str) -> bool:
        """Check if text contains vowel modifications (horn, circumflex, breve)."""
        normalized = normalize('NFD', text)
        return any(char in {'̆', '̂', '̛'} for char in normalized)  # breve, circumflex, horn

    def _same_vowel_base(self, text1: str, text2: str) -> bool:
        """Check if two Vietnamese characters have the same vowel base."""
        base1 = normalize('NFD', text1)
        base2 = normalize('NFD', text2)

        # Remove tone marks and modifications
        for mark in self.tone_marks.union({'̆', '̂', '̛'}):
            base1 = base1.replace(mark, '')
            base2 = base2.replace(mark, '')

        return base1 == base2

    def _is_telex_input(self, text: str) -> bool:
        """Check if text looks like Telex input."""
        return any(pattern in text for pattern in ['aw', 'ow', 'uw', 'dd', 'aa', 'ee', 'oo', 'as', 'af', 'ar', 'ax', 'aj'])

    def _is_valid_vietnamese(self, text: str) -> bool:
        """Check if text is valid Vietnamese."""
        try:
            # Normalize and check if it's valid Unicode
            normalized = normalize('NFC', text)
            return all(ord(char) <= 0x1F9F for char in normalized)  # Unicode range check
        except:
            return False

    def validate_accuracy(self, results: List[ValidationResult]) -> Dict:
        """Calculate accuracy metrics for Vietnamese input validation."""
        if not results:
            return {'total_tests': 0, 'accuracy': 0.0, 'error_breakdown': {}}

        total_tests = len(results)
        correct_tests = sum(1 for r in results if r.is_correct)
        accuracy = (correct_tests / total_tests) * 100 if total_tests > 0 else 0.0

        # Break down errors by type
        error_breakdown = {}
        for result in results:
            if not result.is_correct and result.error_type:
                error_breakdown[result.error_type] = error_breakdown.get(result.error_type, 0) + 1

        return {
            'total_tests': total_tests,
            'correct_tests': correct_tests,
            'accuracy': accuracy,
            'error_breakdown': error_breakdown
        }

    def generate_report(self, results: List[ValidationResult], metrics: Dict) -> str:
        """Generate a comprehensive Vietnamese input validation report."""
        report = []
        report.append("# Vietnamese Input Validation Report\n")

        # Summary
        report.append("## 📊 Summary")
        report.append(f"- **Total Tests**: {metrics['total_tests']}")
        report.append(f"- **Correct**: {metrics['correct_tests']}")
        report.append(f"- **Accuracy**: {metrics['accuracy']:.1f}%")

        if metrics['accuracy'] >= 99.0:
            report.append("✅ **Excellent accuracy**")
        elif metrics['accuracy'] >= 95.0:
            report.append("🟡 **Good accuracy**")
        else:
            report.append("🔴 **Poor accuracy** - needs improvement")

        report.append("")

        # Error breakdown
        if metrics['error_breakdown']:
            report.append("## ❌ Error Breakdown")
            for error_type, count in sorted(metrics['error_breakdown'].items(), key=lambda x: x[1], reverse=True):
                percentage = (count / metrics['total_tests']) * 100
                description = self._get_error_description(error_type)
                report.append(f"- **{description}**: {count} ({percentage:.1f}%)")
            report.append("")

        # Failed tests details
        failed_results = [r for r in results if not r.is_correct]
        if failed_results:
            report.append("## 🔍 Failed Tests Details")

            for result in failed_results[:20]:  # Limit to first 20 failures
                report.append(f"### {result.test_name}")
                report.append(f"- **Input**: `{result.input_text}`")
                report.append(f"- **Expected**: `{result.expected}`")
                report.append(f"- **Actual**: `{result.actual}`")
                report.append(f"- **Error**: {self._get_error_description(result.error_type)}")
                report.append("")

            if len(failed_results) > 20:
                report.append(f"... and {len(failed_results) - 20} more failed tests")

        # Recommendations
        report.append("## 💡 Recommendations")
        if metrics['accuracy'] < 99.0:
            report.append("- Review and fix failing test cases")
            report.append("- Consider edge cases in character conversion")
            report.append("- Validate tone mark combination rules")

        if 'missing_tone_mark' in metrics['error_breakdown']:
            report.append("- Check tone mark application logic")
            report.append("- Verify tone mark priority rules")

        if 'wrong_vowel_modification' in metrics['error_breakdown']:
            report.append("- Review vowel modification combinations")
            report.append("- Check character mapping tables")

        report.append("- Run tests with various input scenarios")
        report.append("- Consider adding more real-world test cases")

        return '\n'.join(report)

    def _get_error_description(self, error_type: str) -> str:
        """Get human-readable description for error type."""
        descriptions = {
            'missing_tone_mark': 'Missing Tone Mark',
            'wrong_tone_mark': 'Wrong Tone Mark',
            'missing_vowel_modification': 'Missing Vowel Modification',
            'wrong_vowel_modification': 'Wrong Vowel Modification',
            'character_substitution': 'Character Substitution',
            'missing_special_character': 'Missing Special Character',
            'incorrect_conversion': 'Incorrect Conversion',
            'unknown_error': 'Unknown Error'
        }
        return descriptions.get(error_type, 'Unknown Error')

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 validate_vietnamese_input.py <test_results.xml>")
        sys.exit(1)

    xml_file = sys.argv[1]

    if not os.path.exists(xml_file):
        print(f"Error: Test results file not found: {xml_file}")
        sys.exit(1)

    validator = VietnameseInputValidator()

    # Parse test results
    results = validator.parse_test_results(xml_file)

    if not results:
        print("Warning: No Vietnamese input test results found")
        sys.exit(0)

    # Validate accuracy
    metrics = validator.validate_accuracy(results)

    # Generate and print report
    report = validator.generate_report(results, metrics)
    print(report)

    # Save report to file
    report_file = 'vietnamese_validation_report.md'
    with open(report_file, 'w', encoding='utf-8') as f:
        f.write(report)

    print(f"\n📄 Detailed report saved to: {report_file}")

    # Exit with error code if accuracy is below threshold
    if metrics['accuracy'] < 95.0:
        print(f"\n❌ Accuracy too low: {metrics['accuracy']:.1f}% (minimum: 95.0%)")
        sys.exit(1)

    print(f"\n✅ Validation passed: {metrics['accuracy']:.1f}% accuracy")

if __name__ == "__main__":
    main()