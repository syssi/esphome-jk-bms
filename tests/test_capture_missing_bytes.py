from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import ast


def test_short_frame_does_not_gain_fake_bytes():
    path = Path(__file__).resolve().parents[1] / "scripts/extract-frame-from-btsnoop.py"
    tree = ast.parse(path.read_text())
    namespace = {}
    exec(
        compile(
            ast.Module(
                body=[node for node in tree.body if isinstance(node, ast.FunctionDef)],
                type_ignores=[],
            ),
            str(path),
            "exec",
        ),
        namespace,
    )
    output = namespace["values_to_vertical_string"](["55:00:aa", "55"])
    rows = output.splitlines()
    assert rows[1].split("\t")[1] == "00    "
    assert rows[2].split("\t")[1] == "aa    "
    assert namespace["values_to_vertical_string"]([]) == ""
