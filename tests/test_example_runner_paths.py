"""Example runners resolve local components beside the script, not the shell."""

import json
import os
from pathlib import Path
import subprocess

import pytest

ROOT = Path(__file__).resolve().parents[1]


@pytest.mark.parametrize("board", ["esp32", "esp8266"])
def test_runner_quotes_config_and_uses_local_components(tmp_path, board):
    capture = tmp_path / "arguments.json"
    executable = tmp_path / "esphome"
    executable.write_text(
        "#!/usr/bin/env python3\nimport json, os, sys\n"
        "with open(os.environ['ARGUMENTS'], 'w') as out: json.dump(sys.argv[1:], out)\n",
        encoding="utf-8",
    )
    executable.chmod(0o755)
    config = tmp_path / "config with spaces.yaml"
    config.write_text("", encoding="utf-8")
    env = dict(
        os.environ,
        PATH=str(tmp_path) + os.pathsep + os.environ["PATH"],
        ARGUMENTS=str(capture),
    )
    subprocess.run(
        ["bash", str(ROOT / f"test-{board}.sh"), "config", str(config)],
        cwd=tmp_path,
        env=env,
        check=True,
    )
    assert json.loads(capture.read_text(encoding="utf-8")) == [
        "-s",
        "external_components_source",
        str(ROOT / "components"),
        "config",
        str(config),
    ]
