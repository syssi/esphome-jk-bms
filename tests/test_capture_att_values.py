"""Repeated ATT fields in JSON retain their individual byte boundaries."""

import importlib.util
from pathlib import Path

import pytest

SCRIPT = (
    Path(__file__).resolve().parents[1] / "scripts" / "extract-frame-from-btsnoop.py"
)
SPEC = importlib.util.spec_from_file_location("capture_att", SCRIPT)
capture = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(capture)


@pytest.mark.parametrize(
    "values, expected",
    [
        ("55:aa:eb:90:02:01", ["55:aa:eb:90:02:01"]),
        (
            ["55:aa:eb:90:01:10", "55:aa:eb:90:02:20"],
            ["55:aa:eb:90:01:10", "55:aa:eb:90:02:20"],
        ),
        (["55:aa:eb:90:02:01", "02:03"], ["55:aa:eb:90:02:01:02:03"]),
    ],
)
def test_extracted_values_assemble_without_merging_byte_text(values, expected):
    packets = [{"_source": {"layers": {"btatt": {"btatt.value": values}}}}]
    assert capture.append_values(capture.extract_btatt_values(packets), 0) == expected
