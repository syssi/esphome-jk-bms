"""Frame assembly must not depend on a subsequent frame being captured."""

import importlib.util
from pathlib import Path
import unittest

SCRIPT = (
    Path(__file__).resolve().parents[1] / "scripts" / "extract-frame-from-btsnoop.py"
)
spec = importlib.util.spec_from_file_location("extract_frame", SCRIPT)
extract_frame = importlib.util.module_from_spec(spec)
spec.loader.exec_module(extract_frame)
assemble = extract_frame.append_values
vertical = extract_frame.values_to_vertical_string


class FrameExtractionTest(unittest.TestCase):
    def test_single_frame(self):
        self.assertEqual(assemble(["55:aa:eb:90:02:01"], 0), ["55:aa:eb:90:02:01"])

    def test_fragmented_last_frame(self):
        self.assertEqual(
            assemble(["55:aa:eb:90:02:01", "02:03", "04:05"], 0),
            ["55:aa:eb:90:02:01:02:03:04:05"],
        )

    def test_two_frames(self):
        frames = ["55:aa:eb:90:01:aa", "55:aa:eb:90:02:bb"]
        self.assertEqual(assemble(frames, 0), frames)

    def test_filter_ignores_other_frame_types(self):
        frames = ["55:aa:eb:90:01:aa", "01:02", "55:aa:eb:90:02:bb", "03:04"]
        self.assertEqual(assemble(frames, 2), ["55:aa:eb:90:02:bb:03:04"])

    def test_command_response_ends_frame(self):
        frames = ["55:aa:eb:90:02:aa", "aa:55:01", "99:88", "55:aa:eb:90:02:bb"]
        self.assertEqual(assemble(frames, 0), [frames[0], frames[3]])

    def test_no_frame(self):
        self.assertEqual(assemble(["aa:55:01", "12:34"], 0), [])


class VerticalStringTest(unittest.TestCase):
    def test_short_last_frame_does_not_gain_fake_bytes(self):
        """Absent bytes are padded with blanks, not rendered as zeros."""
        self.assertEqual(
            vertical(["55:00:aa", "55"]),
            "000 \t55 55 \n001 \t00    \n002 \taa    \n",
        )

    def test_short_leading_frame_keeps_columns_aligned(self):
        """A short frame before longer ones must not shift their columns."""
        self.assertEqual(
            vertical(["55", "55:00", "55:01"]),
            "000 \t55 55 55 \n001 \t   00 01 \n",
        )

    def test_no_frames(self):
        """An empty capture produces an empty string."""
        self.assertEqual(vertical([]), "")


if __name__ == "__main__":
    unittest.main()
