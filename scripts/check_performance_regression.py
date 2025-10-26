#!/usr/bin/env python3
"""
Performance Regression Detection Script for EndKey

This script analyzes performance test results and detects regressions
compared to baseline performance metrics.
"""

import xml.etree.ElementTree as ET
import json
import sys
import os
from typing import Dict, List, Tuple
from dataclasses import dataclass

@dataclass
class PerformanceMetrics:
    test_name: str
    avg_latency_us: float
    p95_latency_us: float
    p99_latency_us: float
    min_latency_us: float
    max_latency_us: float
    sample_count: int

@dataclass
class RegressionThresholds:
    avg_latency_increase_percent: float = 20.0
    p95_latency_increase_percent: float = 30.0
    p99_latency_increase_percent: float = 50.0
    memory_increase_percent: float = 25.0

class PerformanceRegressionDetector:
    def __init__(self, baseline_file: str = None):
        self.baseline_file = baseline_file or "performance_baseline.json"
        self.thresholds = RegressionThresholds()
        self.current_metrics: List[PerformanceMetrics] = []
        self.baseline_metrics: Dict[str, PerformanceMetrics] = {}

    def parse_test_results(self, xml_file: str) -> List[PerformanceMetrics]:
        """Parse GoogleTest XML output and extract performance metrics."""
        tree = ET.parse(xml_file)
        root = tree.getroot()

        metrics = []

        for testcase in root.findall('testcase'):
            test_name = f"{testcase.get('classname')}.{testcase.get('name')}"

            # Extract performance metrics from test output
            output_elem = testcase.find('failure')
            if output_elem is None:
                output_elem = testcase.find('error')

            if output_elem is not None:
                message = output_elem.get('message', '')
                metrics.append(self._extract_metrics_from_message(test_name, message))
            else:
                # Look for performance metrics in system-out
                system_out = testcase.find('system-out')
                if system_out is not None:
                    metrics.append(self._extract_metrics_from_output(test_name, system_out.text))

        return metrics

    def _extract_metrics_from_message(self, test_name: str, message: str) -> PerformanceMetrics:
        """Extract performance metrics from test failure message."""
        # Parse metrics from message format
        lines = message.split('\n')
        metrics = {}

        for line in lines:
            if 'Average latency:' in line:
                metrics['avg_latency'] = float(line.split(':')[1].strip().split()[0])
            elif '95th percentile:' in line:
                metrics['p95_latency'] = float(line.split(':')[1].strip().split()[0])
            elif '99th percentile:' in line:
                metrics['p99_latency'] = float(line.split(':')[1].strip().split()[0])
            elif 'Min latency:' in line:
                metrics['min_latency'] = float(line.split(':')[1].strip().split()[0])
            elif 'Max latency:' in line:
                metrics['max_latency'] = float(line.split(':')[1].strip().split()[0])

        return PerformanceMetrics(
            test_name=test_name,
            avg_latency_us=metrics.get('avg_latency', 0.0),
            p95_latency_us=metrics.get('p95_latency', 0.0),
            p99_latency_us=metrics.get('p99_latency', 0.0),
            min_latency_us=metrics.get('min_latency', 0.0),
            max_latency_us=metrics.get('max_latency', 0.0),
            sample_count=100  # Default sample count
        )

    def _extract_metrics_from_output(self, test_name: str, output: str) -> PerformanceMetrics:
        """Extract performance metrics from system-out output."""
        # Similar to above but parse from system-out
        if output is None:
            return PerformanceMetrics(test_name, 0.0, 0.0, 0.0, 0.0, 0.0, 0)

        return self._extract_metrics_from_message(test_name, output)

    def load_baseline(self):
        """Load baseline performance metrics from file."""
        if os.path.exists(self.baseline_file):
            with open(self.baseline_file, 'r') as f:
                data = json.load(f)

            for test_data in data['tests']:
                self.baseline_metrics[test_data['test_name']] = PerformanceMetrics(
                    test_name=test_data['test_name'],
                    avg_latency_us=test_data['avg_latency_us'],
                    p95_latency_us=test_data['p95_latency_us'],
                    p99_latency_us=test_data['p99_latency_us'],
                    min_latency_us=test_data['min_latency_us'],
                    max_latency_us=test_data['max_latency_us'],
                    sample_count=test_data['sample_count']
                )

    def save_baseline(self, metrics: List[PerformanceMetrics]):
        """Save current metrics as baseline."""
        baseline_data = {
            'timestamp': os.times(),
            'tests': [
                {
                    'test_name': m.test_name,
                    'avg_latency_us': m.avg_latency_us,
                    'p95_latency_us': m.p95_latency_us,
                    'p99_latency_us': m.p99_latency_us,
                    'min_latency_us': m.min_latency_us,
                    'max_latency_us': m.max_latency_us,
                    'sample_count': m.sample_count
                }
                for m in metrics
            ]
        }

        with open(self.baseline_file, 'w') as f:
            json.dump(baseline_data, f, indent=2)

    def detect_regressions(self, current_metrics: List[PerformanceMetrics]) -> List[Dict]:
        """Detect performance regressions by comparing with baseline."""
        regressions = []

        for current in current_metrics:
            if current.test_name in self.baseline_metrics:
                baseline = self.baseline_metrics[current.test_name]

                regression = self._check_regression(current, baseline)
                if regression:
                    regressions.append(regression)

        return regressions

    def _check_regression(self, current: PerformanceMetrics, baseline: PerformanceMetrics) -> Dict:
        """Check if current metrics show regression compared to baseline."""
        regressions = []

        # Check average latency
        avg_increase = self._calculate_percent_increase(current.avg_latency_us, baseline.avg_latency_us)
        if avg_increase > self.thresholds.avg_latency_increase_percent:
            regressions.append({
                'metric': 'avg_latency',
                'current': current.avg_latency_us,
                'baseline': baseline.avg_latency_us,
                'increase_percent': avg_increase,
                'threshold': self.thresholds.avg_latency_increase_percent
            })

        # Check P95 latency
        p95_increase = self._calculate_percent_increase(current.p95_latency_us, baseline.p95_latency_us)
        if p95_increase > self.thresholds.p95_latency_increase_percent:
            regressions.append({
                'metric': 'p95_latency',
                'current': current.p95_latency_us,
                'baseline': baseline.p95_latency_us,
                'increase_percent': p95_increase,
                'threshold': self.thresholds.p95_latency_increase_percent
            })

        # Check P99 latency
        p99_increase = self._calculate_percent_increase(current.p99_latency_us, baseline.p99_latency_us)
        if p99_increase > self.thresholds.p99_latency_increase_percent:
            regressions.append({
                'metric': 'p99_latency',
                'current': current.p99_latency_us,
                'baseline': baseline.p99_latency_us,
                'increase_percent': p99_increase,
                'threshold': self.thresholds.p99_latency_increase_percent
            })

        if regressions:
            return {
                'test_name': current.test_name,
                'regressions': regressions
            }

        return None

    def _calculate_percent_increase(self, current: float, baseline: float) -> float:
        """Calculate percentage increase from baseline to current."""
        if baseline == 0:
            return 0.0
        return ((current - baseline) / baseline) * 100

    def generate_report(self, regressions: List[Dict]) -> str:
        """Generate a performance regression report."""
        report = []
        report.append("# Performance Regression Report\n")

        if not regressions:
            report.append("✅ No performance regressions detected!")
            return '\n'.join(report)

        report.append(f"🚨 {len(regressions)} performance regressions detected:\n")

        for regression in regressions:
            report.append(f"## {regression['test_name']}")

            for metric_reg in regression['regressions']:
                metric_name = metric_reg['metric'].replace('_', ' ').title()
                current = metric_reg['current']
                baseline = metric_reg['baseline']
                increase = metric_reg['increase_percent']
                threshold = metric_reg['threshold']

                report.append(f"- **{metric_name}**: {current:.2f}μs vs {baseline:.2f}μs "
                            f"(+{increase:.1f}% > {threshold:.0f}% threshold)")

            report.append("")

        return '\n'.join(report)

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 check_performance_regression.py <test_results.xml>")
        sys.exit(1)

    xml_file = sys.argv[1]

    if not os.path.exists(xml_file):
        print(f"Error: Test results file not found: {xml_file}")
        sys.exit(1)

    detector = PerformanceRegressionDetector()
    detector.load_baseline()

    # Parse current test results
    current_metrics = detector.parse_test_results(xml_file)

    if not current_metrics:
        print("Warning: No performance metrics found in test results")
        sys.exit(0)

    # Save current metrics as new baseline if none exists
    if not detector.baseline_metrics:
        print("No baseline found. Saving current metrics as baseline.")
        detector.save_baseline(current_metrics)
        sys.exit(0)

    # Detect regressions
    regressions = detector.detect_regressions(current_metrics)

    # Generate and print report
    report = detector.generate_report(regressions)
    print(report)

    # Update baseline with current metrics (optional)
    if os.environ.get('UPDATE_BASELINE', '').lower() == 'true':
        detector.save_baseline(current_metrics)
        print("\nBaseline updated with current metrics.")

    # Exit with error code if regressions detected
    if regressions:
        sys.exit(1)

if __name__ == "__main__":
    main()