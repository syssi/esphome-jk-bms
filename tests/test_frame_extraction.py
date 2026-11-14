"""Frame assembly must not depend on a subsequent frame being captured."""

import ast
from pathlib import Path
import unittest

SCRIPT = (
    Path(__file__).resolve().parents[1] / "scripts" / "extract-frame-from-btsnoop.py"
)
tree = ast.parse(SCRIPT.read_text())
namespace = {}
exec(
    compile(
        ast.Module(
            body=[n for n in tree.body if isinstance(n, ast.FunctionDef)],
            type_ignores=[],
        ),
        str(SCRIPT),
        "exec",
    ),
    namespace,
)
assemble = namespace["append_values"]


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


if __name__ == "__main__":
    unittest.main()
