#!/usr/bin/env python3
"""Inline CHARGE.BAS and SHAPES.TXT into template.html to make index.html."""
import pathlib
d = pathlib.Path(__file__).parent
t = (d / "template.html").read_text()
t = t.replace("/*PROG*/", "\n" + (d / "CHARGE.BAS").read_text())
t = t.replace("/*SHAPES*/", "\n" + (d / "SHAPES.TXT").read_text())
(d / "index.html").write_text(t)
print("wrote index.html", len(t), "bytes")
