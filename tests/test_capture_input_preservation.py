"""Generating a capture report must not replace its JSON source."""

import json
from pathlib import Path
import subprocess
import sys

import pytest

SCRIPT = (
    Path(__file__).resolve().parents[1] / "scripts" / "extract-frame-from-btsnoop.py"
)


@pytest.mark.parametrize("name", ["capture.json", "capture.txt"])
def test_report_preserves_input(tmp_path, name):
    source = tmp_path / name
    original = json.dumps(
        [{"_source": {"layers": {"btatt": {"btatt.value": "55:aa:eb:90:02:01"}}}}]
    )
    source.write_text(original, encoding="utf-8")
    subprocess.run(
        [sys.executable, str(SCRIPT), str(source)],
        check=True,
        capture_output=True,
        text=True,
    )
    assert source.read_text(encoding="utf-8") == original
    report = tmp_path / (
        "capture-frames.txt" if name.endswith(".txt") else "capture.txt"
    )
    assert "000" in report.read_text(encoding="utf-8")
