"""Regression checks for conditional, relative and repeated local includes."""
from pathlib import Path
import tempfile
import unittest

from export_submission import expand_submission

ROOT = Path(__file__).resolve().parents[1]


class ExportTests(unittest.TestCase):
    def setUp(self):
        checks = ROOT / "build/checks"
        checks.mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="export 中文-", dir=checks)
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / "value.hpp").write_text("#pragma once\ninline int value() { return 7; }\n", encoding="utf-8")
        self.source = self.root / "main.cpp"
        self.output = self.root / "out.cpp"

    def test_conditional_includes_keep_guards_at_each_site(self):
        self.source.write_text('#if 0\n#include "value.hpp"\n#endif\n#include "value.hpp"\n'
                               '#include "value.hpp"\n', encoding="utf-8")
        self.assertEqual(expand_submission(self.source, self.output, [self.root], self.root), 1)
        text = self.output.read_text(encoding="utf-8")
        self.assertEqual(text.count("inline int value()"), 3)
        self.assertEqual(text.count("#ifndef CP_EXPANDED_"), 3)
        self.assertNotIn("#pragma once", text)

    def test_missing_header_preserves_previous_output(self):
        self.source.write_text('#include "missing.hpp"\n', encoding="utf-8")
        self.output.write_bytes(b"old export")
        with self.assertRaises(ValueError):
            expand_submission(self.source, self.output, [self.root], self.root)
        self.assertEqual(self.output.read_bytes(), b"old export")

    def test_refuses_to_overwrite_source_or_dependency(self):
        self.source.write_text('#include "value.hpp"\n', encoding="utf-8")
        header = self.root / "value.hpp"
        for path in (self.source, header):
            before = path.read_bytes()
            with self.assertRaises(ValueError):
                expand_submission(self.source, path, [self.root], self.root)
            self.assertEqual(path.read_bytes(), before)

    def test_relative_and_angle_local_includes(self):
        folder = self.root / "sub"
        folder.mkdir()
        (folder / "parent.hpp").write_text('#pragma once\n#include "../value.hpp"\n', encoding="utf-8")
        self.source.write_text('#include <sub/parent.hpp>\n#include <iostream>\n', encoding="utf-8")
        self.assertEqual(expand_submission(self.source, self.output, [self.root], self.root), 2)
        text = self.output.read_text(encoding="utf-8")
        self.assertIn("inline int value()", text)
        self.assertIn("#include <iostream>", text)


if __name__ == "__main__":
    unittest.main()
